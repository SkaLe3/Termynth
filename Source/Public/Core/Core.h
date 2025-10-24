#pragma once

#if defined (_WIN32) || defined(_WIN64)
    #define PLATFORM_WINDOWS
#elif defined(__linux__)
    #define PLATFORM_LINUX
#elif defined(__APPLE__) && defined(__MACH__)
    #define PLATOFORM_MACH
#else
    #define PLATFORM_UNKNOWN
#endif


#include <cstdint>
using int8 = int8_t;
using int32 = int32_t;
using int64 = int64_t;
using uint8 = uint8_t;
using uint32 = uint32_t;
using uint64 = uint64_t;

#include <filesystem>

namespace fs = std::filesystem;