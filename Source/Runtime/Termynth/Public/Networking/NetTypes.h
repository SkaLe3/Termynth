#pragma once
#include "Core/Core.h"

// per Object network role, mirroring Unreal's ENEtRole model

enum class ENetRole : uint8
{
    None, // Not networked
    SimulatedProxy, // Replicated to us; we don't drive it
    AutonomousProxy, // Replicated to us, but we own input authority for it
    Authority // We are the definitive owner (server, or singleplayer)
};

enum class ENetPacketType : uint8
{
    Handshake = 0,
    PropertyUpdate,
    Rpc,
};

enum class ERpcType : uint8
{
    Server, // Client -> Server, always executes with Authority
    Client, // Server -> one specific owning Client
    Multicast // Server -> every Client
};

enum class ERpcReliability : uint8
{
    Reliable,
    Unreliable
};

using RpcFunctionId = uint16;
using NetObjectId = uint32;