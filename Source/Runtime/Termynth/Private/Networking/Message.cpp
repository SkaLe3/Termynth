#include "Networking/Message.h"

/*
bool Net::MessageSocket::SendMessage(const void *data, uint16 size)
{
    m_Socket.Send(&size, sizeof(size));
    m_Socket.Send(data, size);
    return true;
}
bool Net::MessageSocket::ReceiveMessage(Message &out)
{
    uint16 size;
    if (m_Socket.Receive(&size, sizeof(size)) <= 0)
        return false;

    out.Size = size;
    out.Data.resize(size);

    return m_Socket.Receive(out.Data.data(), size) == size;
}
*/