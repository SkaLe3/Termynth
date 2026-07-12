#pragma once
#include "Core/Core.h"
#include <type_traits>

enum class ENetRole : uint8
{
    None,
    Server,
    Client
};

class NetContext
{
public:
    static inline ENetRole Role = ENetRole::None;
    // Server sets immediately to Server
    // Client sets to Client when connects to the server.
};

namespace Net
{
    inline bool IsServer()
    {
        return NetContext::Role == ENetRole::Server;

    }

    inline bool IsClient()
    {
        return NetContext::Role == ENetRole::Client;
    }

    inline bool HasAuthority()
    {
        return static_cast<uint8>(NetContext::Role) < static_cast<uint8>(ENetRole::Client);
    }





    using Byte = uint8;

    class Networking
    {
        public:
        static void Init();
        static void Shutdown();
    };

    class Socket
    {
        NONCOPYABLE(Socket);
    public:
        Socket();
        ~Socket();

        Socket(Socket&& other) noexcept;
        Socket& operator=(Socket&& other) noexcept;
             
        bool IsValid() const;
        bool Connect(const std::string& ip, uint16 port);
        bool Bind(uint16 port);
        bool Listen(int32 backlog = 8);
        Socket Accept();

        int32 Send(const void* data, size_t size);
        int32 Receive(void* buffer, size_t size);

        void Close();


    private:
        void* m_Handle;
    };
}