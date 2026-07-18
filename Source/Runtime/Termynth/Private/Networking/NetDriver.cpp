#include "Networking/NetDriver.h"
#include "Networking/NetObject.h"
#include "Core/Logger.h"

#include <cstring>

namespace
{
    // Wire format for every packet: [FPacketHeader][payload...], itself
    // prefixed on the socket with a uint16 total size
    struct FPacketHeader
    {
        ENetPacketType Type;
        NetObjectId ObjectId;
        uint16 SubId; // property index for PropertyUpdate, RpcFunctionId for Rpc, connection id for Handshake
    };
}

NetDriver& NetDriver::Get()
{
    static NetDriver instance;
    return instance;
}

bool NetDriver::Listen(uint16 port, int32 expectedConnections)
{
    m_bIsServer = true;
    m_Running = true;

    if (!m_ServerListenSocket.Bind(port))
    {
        LOG_ERROR("NetDriver: failed to bind port " + std::to_string(port));
        return false;
    }
    m_ServerListenSocket.Listen();
    LOG_INFO("NetDriver: listening on port " + std::to_string(port));

    m_Connections.reserve(expectedConnections);
    for (int32 i = 0; i < expectedConnections; ++i)
    {
        Net::Socket client = m_ServerListenSocket.Accept();
        if (!client.IsValid())
        {
            LOG_ERROR("NetDriver: accept failed");
            return false;
        }

        FNetConnection conn;
        conn.Socket = std::move(client);
        conn.ConnectionId = i;
        conn.Thread = std::thread(&NetDriver::ConnectionThread, this, i);
        m_Connections.push_back(std::move(conn));

        FPacketHeader header{ ENetPacketType::Handshake, 0, (uint16)i };
        uint16 size = sizeof(header);
        m_Connections.back().Socket.Send(&size, sizeof(size));
        m_Connections.back().Socket.Send(&header, size);

        LOG_INFO("NetDriver: connection " + std::to_string(i) + " established");
    }
    return true;
}

bool NetDriver::Connect(const std::string& ip, uint16 port)
{
    m_bIsServer = false;
    m_Running = true;

    if (!m_ClientSocket.Connect(ip, port))
    {
        LOG_ERROR("NetDriver: failed to connect to " + ip);
        return false;
    }
    m_ClientThread = std::thread(&NetDriver::ConnectionThread, this, -1);
    LOG_INFO("NetDriver: connected to " + ip);
    return true;
}

void NetDriver::Shutdown()
{
    m_Running = false;

    if (m_ClientSocket.IsValid()) m_ClientSocket.Close();
    if (m_ClientThread.joinable()) m_ClientThread.join();

    for (auto& c : m_Connections)
    {
        if (c.Socket.IsValid()) c.Socket.Close();
        if (c.Thread.joinable()) c.Thread.join();
    }
    m_Connections.clear();
    m_Objects.clear();
}

ENetRole NetDriver::ResolveLocalRole(int32 owningConnectionId) const
{
    if (m_bIsServer) return ENetRole::Authority;
    return (owningConnectionId == m_LocalConnectionId) ? ENetRole::AutonomousProxy : ENetRole::SimulatedProxy;
}

ENetRole NetDriver::ResolveRemoteRole(int32 owningConnectionId) const
{
    if (m_bIsServer) return owningConnectionId >= 0 ? ENetRole::AutonomousProxy : ENetRole::SimulatedProxy;
    return ENetRole::Authority;
}

void NetDriver::RegisterNetObject(NetObjectId id, NetObject* obj, int32 owningConectionId)
{
    obj->m_NetId = id;
    obj->m_OwningDriver = this;
    obj->m_OwningConnectionId = owningConectionId;
    obj->m_LocalRole = ResolveLocalRole(owningConectionId);
    obj->m_RemoteRole = ResolveRemoteRole(owningConectionId);
    m_Objects[id] = {obj, owningConectionId };
}

void NetDriver::UnregisterNetObject(NetObjectId id)
{
    m_Objects.erase(id);
}

void NetDriver::SendPacket(int32 connectionId, ENetPacketType type, NetObjectId objId, uint16 subId, const std::vector<Byte>& payload)
{
    FPacketHeader header{type, objId, subId };
    std::vector<Byte> full(sizeof(header) + payload.size());
    std::memcpy(full.data(), &header, sizeof(header));
    if (!payload.empty())
        std::memcpy(full.data() + sizeof(header), payload.data(), payload.size());

    uint16 size = (uint16)full.size();

    if  (m_bIsServer)
    {
        for (auto& c : m_Connections)
        {
            if (c.ConnectionId == connectionId)
            {
                c.Socket.Send(&size, sizeof(size));
                c.Socket.Send(full.data(), full.size());
                return;
            }
        }
    }
    else
    {
        m_ClientSocket.Send(&size, sizeof(size));
        m_ClientSocket.Send(full.data(), full.size());
    }
}

void NetDriver::BroadcastPacket(ENetPacketType type, NetObjectId objId, uint16 subId, const std::vector<Byte>& payload)
{
    if (!m_bIsServer) return;
    for (auto& c : m_Connections)
        SendPacket(c.ConnectionId, type, objId, subId, payload);
}

void NetDriver::SendRpc(NetObjectId objId, int32 owningConnectionId, RpcFunctionId fnId, ERpcType type, ERpcReliability /*reliability*/, FArchive&& payload)
{
    // The transport is TCP, so everything is already reliable/ordered.
    // 'reliability' is kept as a parameter/hook for UDP extension transport later.
    switch (type)
    {
    case ERpcType::Server:
        if (m_bIsServer) return; // servers fon't send Server RPCs to themselves;
        SendPacket(-1, ENetPacketType::Rpc, objId, fnId, payload.GetBuffer());
        break;
    case ERpcType::Client:
        if (!m_bIsServer) return;
        SendPacket(owningConnectionId, ENetPacketType::Rpc, objId, fnId, payload.GetBuffer());
        break;
    case ERpcType::Multicast:
        if (!m_bIsServer) return;
        BroadcastPacket(ENetPacketType::Rpc, objId, fnId, payload.GetBuffer());
        break;
    }
}

void NetDriver::ConnectionThread(int32 connectionId)
{
    Net::Socket* socket = nullptr;
    if (m_bIsServer)
    {
        for (auto& c : m_Connections)
            if (c.ConnectionId == connectionId) socket = &c.Socket;
    }
    else
    {
        socket = &m_ClientSocket;
    }
    if (!socket) return;

    while (m_Running)
    {
        uint16 size = 0;
        if (socket->Receive(&size, sizeof(size)) <= 0) break;

        FRawPacket packet;
        packet.ConnectionId = connectionId;
        packet.Data.resize(size);
        if (socket->Receive(packet.Data.data(), size) <= 0) break;

        std::lock_guard lock(m_InboxMutex);
        m_Inbox.push(std::move(packet));
    }
    LOG_INFO("NetDriver: connection thread ended (id " + std::to_string(connectionId) + ")");
}

void NetDriver::ProcessPacket(int32 /*connectionId*/, const std::vector<Byte>& raw)
{
    if (raw.size() < sizeof(FPacketHeader)) return;

    FPacketHeader header;
    std::memcpy(&header, raw.data(), sizeof(header));

    if (header.Type == ENetPacketType::Handshake)
    {
        m_LocalConnectionId = header.SubId;
        LOG_INFO("NetDriver: assigned local connection id " + std::to_string((int)header.SubId));
        return;
    }

    auto it = m_Objects.find(header.ObjectId);
    if (it == m_Objects.end())
    {
        LOG_WARNING("NetDriver: packcet for unkown NetObject " + std::to_string(header.ObjectId));
        return;
    }
    NetObject* obj = it->second.Object;
    
    std::vector<Byte> payload(raw.begin() + sizeof(header), raw.end());
    FArchive ar(false);
    ar.SetBuffer(std::move(payload));

    if (header.Type == ENetPacketType::Rpc)
    {
        obj->DispatchRpc(header.SubId, ar);
    }
    else if (header.Type == ENetPacketType::PropertyUpdate)
    {
        std::vector<IRepProp*> props;
        obj->CollectReplicatedProps(props);
        if (header.SubId < props.size())
        {
            props[header.SubId]->Serialize(ar);
        }
    }
}

void NetDriver::TickReplication()
{
    if (!m_bIsServer) return; // only Authority replicates outward

    for (auto& [id, reg] : m_Objects)
    {
        std::vector<IRepProp*> props;
        reg.Object->CollectReplicatedProps(props);

        for (uint16 i = 0; i < (uint16)props.size(); ++i)
        {
            if (props[i]->ConsumeDirty())
            {
                FArchive ar(true);
                props[i]->Serialize(ar);
                BroadcastPacket(ENetPacketType::PropertyUpdate, id, i, ar.GetBuffer());
            }
        }
    }
}

void NetDriver::Tick(float /*deltaTime*/)
{
    std::queue<FRawPacket> toProcess;
    {
        std::lock_guard lock(m_InboxMutex);
        std::swap(toProcess, m_Inbox);
    }
    while (!toProcess.empty())
    {
        ProcessPacket(toProcess.front().ConnectionId, toProcess.front().Data);
        toProcess.pop();
    }

    TickReplication();
}