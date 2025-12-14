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

#if !defined(TERMYNTH_ENGINE_DIR)
#define TERMYNTH_ENGINE_DIR ""
#endif

#if !defined(TERMYNTH_PROJECT_DIR)
#define TERMYNTH_PROJECT_DIR ""
#endif

#if defined(_MSC_VER)
#define PACKED_STRUCT(name) __pragma(pack(push, 1)) struct name __pragma(pack(pop))
#else
#define PACKED_STRUCT(name) struct __attribute__((packed)) name
#endif

#define NONCOPYABLE(Type) \
private: \
    Type(const Type&) = delete; \
    Type& operator=(const Type&) = delete;

#define NONMOVABLE(Type) \
private:    \
    Type(Type&&) = delete; \
    Type& operator=(Type&&) = delete;


#include <cstdint>
#include <filesystem>

using int8 = int8_t;
using int32 = int32_t;
using int64 = int64_t;
using uint8 = uint8_t;
using uint16 = uint16_t;
using uint32 = uint32_t;
using uint64 = uint64_t;



namespace fs = std::filesystem;