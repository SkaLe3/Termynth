#pragma once

#include "Core/Core.h"
#include "NetTypes.h"
#include "NetArchive.h"
#include "Networking/NetworkingCore.h"

#include <thread>
#include <mutex>
#include <atomic>
#include <queue>
#include <vector>
#include <unordered_map>

class NetObject;

struct FNetConnection
{
    Net::Socket Socket;
    std::thread Thread;
    int32 ConnectionId = -1;
};

struct FRawPacket
{
    int32 ConnectionId = -1; // -1 on the client: there's only ever one connection, the server
    std::vector<Byte> Data;
};


/*
 *  Generic, game-agnostic network driver. One instance per process.
 *  Responsibilities:
 *      - own the socket(s)/threads and frame messages
 *      - assign ENetRole to every registered NetObject
 *      - each tick, diff replicated properties on Authority objects and push updates to clients
 *      - route incoming RPC / property packets to the right NetObject by id
 *
*/
class NetDriver
{
public:
    static NetDriver& Get();

    bool Listen(uint16 port, int32 expectedConnections = 1);
    bool Connect(const std::string& ip, uint16 port);
    void Shutdown();

    bool IsServer() const { return m_bIsServer; }
    bool IsConnected() const { return m_Running; }
    int32 GetLocalConnectionId() const { return m_LocalConnectionId; }

    // owningConnectionId: which client's input this object belongs to
    void RegisterNetObject(NetObjectId id, NetObject* obj, int32 owningConnectionId = -1);
    void UnregisterNetObject(NetObjectId id);

    // Used internally by NetObject::SendRpc
    void SendRpc(NetObjectId objId, int32 owningConnectionId, RpcFunctionId fnId, ERpcType type, ERpcReliability reliability, FArchive&& payload);

    // Call once per frame from the game loop, before ticking game objects
    void Tick(float deltaTime);

private:
    NetDriver() = default;
    NONCOPYABLE(NetDriver);

    void ConnectionThread(int32 connectionId);
    void SendPacket(int32 connectionId, ENetPacketType type, NetObjectId objId, uint16 subId, const std::vector<Byte>& payload);
    void BroadcastPacket(ENetPacketType type, NetObjectId objId, uint16 subId, const std::vector<Byte>& payload);
    void ProcessPacket(int32 connectionId, const std::vector<Byte>& raw);
    void TickReplication();

    ENetRole ResolveLocalRole(int32 owningConnectionId) const;
    ENetRole ResolveRemoteRole(int32 owningConnectionId) const;

private:
    bool m_bIsServer = false;
    std::atomic<bool> m_Running = false;
    std::atomic<int32> m_LocalConnectionId = -1;

    Net::Socket m_ServerListenSocket;
    Net::Socket m_ClientSocket;
    std::thread m_ClientThread;

    std::vector<FNetConnection> m_Connections;

    std::mutex m_InboxMutex;
    std::queue<FRawPacket> m_Inbox;

    struct FRegisteredObject
    {
        NetObject* Object = nullptr;
        int32 OwningConnectionId = -1;
    };
    std::unordered_map<NetObjectId, FRegisteredObject> m_Objects;
};