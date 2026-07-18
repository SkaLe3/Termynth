#pragma once

#include "Core/Core.h"
#include "NetTypes.h"
#include "NetArchive.h"
#include "RepProperty.h"

#include <vector>
#include <unordered_map>
#include <functional>

class NetDriver;

// BAse class for anything that participates in replication/RPCs

class NetObject
{
public:
    virtual ~NetObject() = default;

    ENetRole GetLocalRole() const { return m_LocalRole; }
    ENetRole GetRemoteRole() const { return m_RemoteRole; }

    bool HasAuthority() const { return m_LocalRole == ENetRole::Authority; }
    bool IsAutonomousProxy() const { return m_LocalRole == ENetRole::AutonomousProxy; }
    bool IsSimulatedProxy() const { return m_LocalRole == ENetRole::SimulatedProxy; }

    // True if the local machine should be reading local input for this object right now.
    // (owning client or the server itself, when nobody owns it(singleplayer/listen-server))
    bool IsLocallyControlled() const 
    {
        return m_LocalRole == ENetRole::AutonomousProxy ||
            (m_LocalRole == ENetRole::Authority && m_RemoteRole != ENetRole::AutonomousProxy);
    }

    NetObjectId GetNetId() const { return m_NetId; }

    virtual void CollectReplicatedProps(std::vector<IRepProp*>& OutProps) {(void)OutProps;}

protected:
    // Call from an RPC called stub to push the call across the write instead of running it in-process
    void SendRpc(RpcFunctionId, ERpcType type, ERpcReliability reliability, FArchive&& payload);

    // Call once per RPC. Wires the function id to the code that deserializes params and calls the _Implementation.
    void RegisterRpcHandler(RpcFunctionId id, std::function<void(FArchive&)> handler);

private:
    void DispatchRpc(RpcFunctionId id, FArchive& ar);

    ENetRole m_LocalRole = ENetRole::None;
    ENetRole m_RemoteRole = ENetRole::None;
    NetObjectId m_NetId = 0;
    NetDriver* m_OwningDriver = nullptr;
    int32 m_OwningConnectionId = -1;

    std::unordered_map<RpcFunctionId, std::function<void(FArchive&)>> m_RpcHandlers;

    friend class NetDriver;
};