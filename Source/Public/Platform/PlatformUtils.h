#pragma once
#include "Core/Core.h"
#if defined(PLATFORM_WINDOWS)
#include "Windows/WindowsPlatformUtils.h"
using PlatformUtils = PlatformUtilsWindows;
//using PlatformUtils = PlatformUtilsGeneric;
#elif defined (PLATFORM_LINUX)
#include "Linux/LinuxPlatformUtils.h"
using PlatformUtils = PlatformUtilsLinux;
#endif


