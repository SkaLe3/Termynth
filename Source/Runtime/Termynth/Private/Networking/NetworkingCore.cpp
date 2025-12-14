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
    static void check_socket_error(const char* funcName)
    {
#ifdef PLATFORM_WINDOWS
        int err = WSAGetLastError();
        if (err != 0)
        {
            LOG_ERROR(std::string(funcName) + " failed, WSA error: " + std::to_string(err));
        }
#elif defined(PLATFORM_LINUX)
        if (errno != 0)
        {
            LOG_ERROR(std::string(funcName) + " failed, errno: " + std::to_string(errno) + " (" + std::to_string(strerror(errno)) + ")");
        }
#endif
    }

    void Networking::Init()
    { 
        INIT_NETWORK();
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
            check_socket_error("send");
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
}