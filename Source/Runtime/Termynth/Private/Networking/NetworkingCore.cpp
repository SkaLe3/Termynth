#include "Core/Core.h"
#include "Core/Logger.h"
#include "Networking/NetworkingCore.h"

#if defined(PLATFORM_WINDOWS)
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma commect(lib, "Ws2_32.lib")
    using socket_t = SOCKET;
    #define CLOSESOCKET closesocket
    #define INIT_NETWORK() { WSADATA wsaData; WSAStartup(MAKEWORD(2,2), &wsaData); }
    #define CLEANUP_NETWORK() WSACleanup()
#elif defined(PLATFORM_LINUX)
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <termios.h>
    #include <fcntl.h>
    #include <errno.h>
    using socket_t = int;
    #define INVALID_SOCKET -1;
    #define SOCKET_ERROR -1;
    #define CLOSESOCKET close
    #define INIT_NETWORK()
    #define CLEANUP_NETWORK()
#endif

namespace Net
{
    static std::string get_wsa_error_description(int32 errorCode);
    static ENetErrorResponse get_wsa_error_response(int32 errorCode);

    static ENetErrorResponse check_socket_error(const char* funcName)
    {
        ENetErrorResponse Response = ENetErrorResponse::Retry;
#ifdef PLATFORM_WINDOWS
        int err = WSAGetLastError();
        if (err != 0)
        {
            LOG_ERROR(std::string(funcName) + "function failed, WSA error: " + std::to_string(err) + " " + get_wsa_error_description(err));
            Response = get_wsa_error_response(err);
        }
#elif defined(PLATFORM_LINUX)
        if (errno != 0)
        {
            LOG_ERROR(std::string(funcName) + "function failed, errno: " + std::to_string(errno) + " (" + std::to_string(strerror(errno)) + ")");
        }
#endif
        return Response;
    }

    void Networking::Init()
    { 
        INIT_NETWORK();
        #ifdef DEDICATED_SERVER
            //NetContext::Role = ENetRole::Server;
        #endif
    }
    void Networking::Shutdown()
    {
        CLEANUP_NETWORK();
    }

    Socket::Socket()
    {
        socket_t s = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        m_Handle = (s == INVALID_SOCKET) ? nullptr : reinterpret_cast<void*>(s);
    }

	Socket::Socket(Socket&& other) noexcept
	{
		m_Handle = other.m_Handle;
		other.m_Handle = nullptr;
	}

	Socket& Socket::operator=(Socket&& other) noexcept
	{
		if (this != &other)
		{
			Close();
			m_Handle = other.m_Handle;
			other.m_Handle = nullptr;
		}
		return *this;
	}

	Socket::~Socket()
    {
        Close();
    }

    bool Socket::IsValid() const 
    {
        return m_Handle != nullptr;
    }

    bool Socket::Connect(const std::string& ip, uint16 port)
    {
        if (!IsValid()) return false;

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        inet_pton(AF_INET, ip.data(), &addr.sin_addr);

        socket_t s = reinterpret_cast<socket_t>(m_Handle);
        return ::connect(s, (sockaddr*)&addr, sizeof(addr)) == 0;
    }

    bool Socket::Bind(uint16 port)
    {
        if (!IsValid()) return false;

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = INADDR_ANY;

        socket_t s = reinterpret_cast<socket_t>(m_Handle);
        return ::bind(s, (sockaddr*)&addr, sizeof(addr)) == 0;
    }

    bool Socket::Listen(int32 backlog)
    {
        if (!IsValid()) return false;

        socket_t s = reinterpret_cast<socket_t>(m_Handle);
        return ::listen(s, backlog) == 0;
    }

    Socket Socket::Accept()
    {
        socket_t s = reinterpret_cast<socket_t>(m_Handle);
        socket_t client = ::accept(s, nullptr, nullptr);

		if (client == INVALID_SOCKET)
			return {}; // invalid socket

        Socket result;
        result.m_Handle = reinterpret_cast<void*>(client);
        return result;
    }

    int32 Socket::Send(const void* data, size_t size)
    {
        if (!IsValid()) return -1;
        socket_t s = reinterpret_cast<socket_t>(m_Handle);
        int32 bytesSent = ::send(s, (const char*)data, (int32)size, 0);
        if (bytesSent == -1)
        {
            ENetErrorResponse Response = check_socket_error("send");
            if (Response == ENetErrorResponse::CloseConnection)
            {
                LOG_WARNING("Connection lost");
                Close();
            }
        }
        else if (bytesSent == 0)
        {
            LOG_WARNING("Connection lost");
            Close();
        }
        return bytesSent;
    }

    int32 Socket::Receive(void* buffer, size_t size)
    {
        if (!IsValid()) return -1;
        socket_t s = reinterpret_cast<socket_t>(m_Handle);
        int32 bytesSent = ::recv(s, (char*)buffer, (int)size, 0);
        if (bytesSent == -1)
        {
            check_socket_error("recv"); 
        }
        else if (bytesSent == 0)
        {
            LOG_WARNING("Connection lost");
            Close();
        }
        return bytesSent;
    }

    void Socket::Close()
    {
        if (!IsValid()) return;

        socket_t s = reinterpret_cast<socket_t>(m_Handle);
        CLOSESOCKET(s);
        m_Handle = nullptr;
    }

#ifdef PLATFORM_WINDOWS
    static std::string get_wsa_error_description(int32 errorCode)
    {
        switch (errorCode)
        {
        case 6:
            return std::string("WSA_INVALID_HANDLE: Specified event object handle is invalid.");
        case 8:
            return std::string("WSA_NOT_ENOUGH_MEMORY: Insufficient memory available");
        case 87:
            return std::string("WSA_INVALID_PARAMETER: One or more parameters are invalid");
        case 995:
            return std::string("WSA_OPERATION_ABORTED: An overlapped operation was canceled due to the closure of the socket");
        case 996:
            return std::string("WSA_IO_INCOMPLETE: Overlapped I/O event object not in signaled state");
        case 997:
            return std::string("WSA_IO_PENDING: Overlapped operations will complete later");
        case 10004:
            return std::string("WSAEINTR: Interrupted function call");
        case 10009:
            return std::string("WSAEBADF: File handle is not valid.");
        case 10013:
            return std::string("WSAEACCES: Permission denied.");
        case 10014:
            return std::string("WSAEFAULT: Bad address.");
        case 10022:
            return std::string("WSAEINVAL: Invalid argument.");
        case 10024:
            return std::string("WSAEMFILE: Too many open files.");
        case 10035:
            return std::string("WSAEWOULDBLOCK: Resource temporarily unavailable.");
        case 10036:
            return std::string("WSAEINPROGRESS: Operation now in progress.");
        case 10037:
            return std::string("WSAEALREADY: Operation already in progress.");
        case 10038:
            return std::string("WSAENOTSOCK: Socket operation on nonsocket.");
        case 10039:
            return std::string("WSAEDESTADDRREQ: Destination address required.");
        case 10040:
            return std::string("WSAEMSGSIZE: Message too long.");
        case 10041:
            return std::string("WSAEPROTOTYPE: Protocol wrong type for socket.");
        case 10042:
            return std::string("WSAENOPROTOOPT: Bad protocol option.");
        case 10043:
            return std::string("WSAEPROTONOSUPPORT: Protocol not supported.");
        case 10044:
            return std::string("WSAESOCKTNOSUPPORT: Socket type not supported.");
        case 10045:
            return std::string("WSAEOPNOTSUPP: Operation not supported.");
        case 10046:
            return std::string("WSAEPFNOSUPPORT: Protocol family not supported.");
        case 10047:
            return std::string("WSAEAFNOSUPPORT: Address family not supported by protocol family.");
        case 10048:
            return std::string("WSAEADDRINUSE: Address already in use.");
        case 10049:
            return std::string("WSAEADDRNOTAVAIL: Cannot assign requested address.");
        case 10050:
            return std::string("WSAENETDOWN: Network is down.");
        case 10051:
            return std::string("WSAENETUNREACH: Network is unreachable.");
        case 10052:
            return std::string("WSAENETRESET: Network dropped connection on reset.");
        case 10053:
            return std::string("WSAECONNABORTED: Software caused connection abort.");
        case 10054:
            return std::string("WSAECONNRESET: Connection reset by peer.");
        case 10055:
            return std::string("WSAENOBUFS: No buffer space available.");
        case 10056:
            return std::string("WSAEISCONN: Socket is already connected.");
        case 10057:
            return std::string("WSAENOTCONN: Socket is not connected.");
        case 10058:
            return std::string("WSAESHUTDOWN: Cannot send after socket shutdown.");
        case 10059:
            return std::string("WSAETOOMANYREFS: Too many references.");
        case 10060:
            return std::string("WSAETIMEDOUT: Connection timed out.");
        case 10061:
            return std::string("WSAECONNREFUSED: Connection refused.");
        case 10062:
            return std::string("WSAELOOP: Cannot translate name.");
        case 10063:
            return std::string("WSAENAMETOOLONG: Name too long.");
        case 10064:
            return std::string("WSAEHOSTDOWN: Host is down.");
        case 10065:
            return std::string("WSAEHOSTUNREACH: No route to host.");
        case 10066:
            return std::string("WSAENOTEMPTY: Directory not empty.");
        case 10067:
            return std::string("WSAEPROCLIM: Too many processes.");
        case 10068:
            return std::string("WSAEUSERS: User quota exceeded.");
        case 10069:
            return std::string("WSAEDQUOT: Disk quota exceeded.");
        case 10070:
            return std::string("WSAESTALE: Stale file handle reference.");
        case 10071:
            return std::string("WSAEREMOTE: Item is remote.");
        case 10091:
            return std::string("WSASYSNOTREADY: Network subsystem is unavailable.");
        case 10092:
            return std::string("WSAVERNOTSUPPORTED: Winsock.dll version out of range.");
        case 10093:
            return std::string("WSANOTINITIALISED: Successful WSAStartup not yet performed.");
        case 10101:
            return std::string("WSAEDISCON: Graceful shutdown in progress.");
        case 10102:
            return std::string("WSAENOMORE: No more results.");
        case 10103:
            return std::string("WSAECANCELLED: Call has been canceled.");
        case 10104:
            return std::string("WSAEINVALIDPROCTABLE: Procedure call table is invalid.");
        case 10105:
            return std::string("WSAEINVALIDPROVIDER: Service provider is invalid.");
        case 10106:
            return std::string("WSAEPROVIDERFAILEDINIT: Service provider failed to initialize.");
        case 10107:
            return std::string("WSASYSCALLFAILURE: System call failure.");
        case 10108:
            return std::string("WSASERVICE_NOT_FOUND: Service not found.");
        case 10109:
            return std::string("WSATYPE_NOT_FOUND: Class type not found.");
        case 10110:
            return std::string("WSA_E_NO_MORE: No more results.");
        case 10111:
            return std::string("WSA_E_CANCELLED: Call was canceled.");
        case 10112:
            return std::string("WSAEREFUSED: Database query was refused.");
        case 11001:
            return std::string("WSAHOST_NOT_FOUND: Host not found.");
        case 11002:
            return std::string("WSATRY_AGAIN: Nonauthoritative host not found.");
        case 11003:
            return std::string("WSANO_RECOVERY: This is a nonrecoverable error.");
        case 11004:
            return std::string("WSANO_DATA: Valid name, no data record of requested type.");
        case 11005:
            return std::string("WSA_QOS_RECEIVERS: QoS receivers.");
        case 11006:
            return std::string("WSA_QOS_SENDERS: QoS senders.");
        case 11007:
            return std::string("WSA_QOS_NO_SENDERS: No QoS senders.");
        case 11008:
            return std::string("WSA_QOS_NO_RECEIVERS: QoS no receivers.");
        case 11009:
            return std::string("WSA_QOS_REQUEST_CONFIRMED: QoS request confirmed.");
        case 11010:
            return std::string("WSA_QOS_ADMISSION_FAILURE: QoS admission error.");
        case 11011:
            return std::string("WSA_QOS_POLICY_FAILURE: QoS policy failure.");
        case 11012:
            return std::string("WSA_QOS_BAD_STYLE: QoS bad style.");
        case 11013:
            return std::string("WSA_QOS_BAD_OBJECT: QoS bad object.");
        case 11014:
            return std::string("WSA_QOS_TRAFFIC_CTRL_ERROR: QoS traffic control error.");
        case 11015:
            return std::string("WSA_QOS_GENERIC_ERROR: QoS generic error.");
        case 11016:
            return std::string("WSA_QOS_ESERVICETYPE: QoS service type error.");
        case 11017:
            return std::string("WSA_QOS_EFLOWSPEC: QoS flowspec error.");
        case 11018:
            return std::string("WSA_QOS_EPROVSPECBUF: Invalid QoS provider buffer.");
        case 11019:
            return std::string("WSA_QOS_EFILTERSTYLE: Invalid QoS filter style.");
        case 11020:
            return std::string("WSA_QOS_EFILTERTYPE: Invalid QoS filter type.");
        case 11021:
            return std::string("WSA_QOS_EFILTERCOUNT: Incorrect QoS filter count.");
        case 11022:
            return std::string("WSA_QOS_EOBJLENGTH: Invalid QoS object length.");
        case 11023:
            return std::string("WSA_QOS_EFLOWCOUNT: Incorrect QoS flow count.");
        case 11024:
            return std::string("WSA_QOS_EUNKOWNPSOBJ: Unrecognized QoS object.");
        case 11025:
            return std::string("WSA_QOS_EPOLICYOBJ: Invalid QoS policy object.");
        case 11026:
            return std::string("WSA_QOS_EFLOWDESC: Invalid QoS flow descriptor.");
        case 11027:
            return std::string("WSA_QOS_EPSFLOWSPEC: Invalid QoS provider-specific flowspec.");
        case 11028:
            return std::string("WSA_QOS_EPSFILTERSPEC: Invalid QoS provider-specific filterspec.");
        case 11029:
            return std::string("WSA_QOS_ESDMODEOBJ: Invalid QoS shape discard mode object.");
        case 11030:
            return std::string("WSA_QOS_ESHAPERATEOBJ: Invalid QoS shaping rate object.");
        case 11031:
            return std::string("WSA_QOS_RESERVED_PETYPE: Reserved policy QoS element type.");
        default:
            return std::string("Unknown WSA error");
        }
    }

    static ENetErrorResponse get_wsa_error_response(int32 errorCode)
    {
        switch (errorCode)
        {
        case 10054:
            return ENetErrorResponse::CloseConnection;
        default:
            return ENetErrorResponse::Retry;
        }
    }
#endif
}