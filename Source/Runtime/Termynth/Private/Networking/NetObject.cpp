#include "Networking/NetObject.h"
#include "Networking/NetDriver.h"
#include "Core/Logger.h"

void NetObject::SendRpc(RpcFunctionId id, ERpcType type, ERpcReliability reliability, FArchive&& payload)
{
    if (!m_OwningDriver)
    {
        LOG_WARNING("SendRpc called on a NetObject that isn't registered with a NetDriver yet");
        return;
    }
    m_OwningDriver->SendRpc(m_NetId, m_OwningConnectionId, id, type, reliability, std::move(payload));
}

void NetObject::RegisterRpcHandler(RpcFunctionId id, std::function<void(FArchive&)> handler)
{
    m_RpcHandlers[id] = std::move(handler);
}

void NetObject::DispatchRpc(RpcFunctionId id, FArchive& ar)
{
    auto it = m_RpcHandlers.find(id);
    if (it == m_RpcHandlers.end())
    {
        LOG_WARNING("Received RPC with unregistered function id for NetObject " + std::to_string(m_NetId));
        return;
    }
    it->second(ar);
}