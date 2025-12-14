#pragma once
#include "Core/Core.h"

namespace Net
{
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