#pragma once
#include "NetTypes.h"

// Compile-time FNV-1a hash, so every RPC ets a stable id derived from its own name with zero manual bookkeeping
// Consider using Google Hash
constexpr RpcFunctionId HashRpcName(const char* str, RpcFunctionId basis = 0x811C)
{
    return (*str) ? HashRpcName(str + 1, (RpcFunctionId)((basis ^ (RpcFunctionId)(*str)) * 0x0101)) : basis;
}

#define DECLARE_SERVER_RPC(Ret, Name, ...) \
    public: \
        Ret Name(__VA_ARGS__); \
    private: \
        Ret Name##_Implementation(__VA_ARGS__); \
        static constexpr RpcFunctionId Name##_Id = HashRpcName(#Name);

#define DECLARE_CLIENT_RPC(Ret, Name, ...) DECLARE_SERVER_RPC(Ret, Name, __VA_ARGS__)
#define DECLARE_MULTICAST_RPC(Ret, Name, ...) DECLARE_SERVER_RPC(Ret, Name, __VA_ARGS__)