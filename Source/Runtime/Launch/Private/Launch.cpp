#include "Platform/PlatformUtils.h"
#include "Platform/Window.h"
#include "Render/Renderer.h"
#include "Core/Globals.h"
#include "Core/EngineLoop.h"
#include "Core/Logger.h"

#include <iostream>
#include <memory>

#include <thread>
#include <chrono>
#include <fstream>

//#include <conio.h>

using PU = PlatformUtils;

EngineLoop g_EngineLoop;

int32 EngineInit()
{
    int32 errorResult = g_EngineLoop.Init();
    return errorResult;
}

void EngineTick()
{
    g_EngineLoop.Tick();
}

void EngineExit()
{
    RequestEngineExit();
    g_EngineLoop.Exit();
}

int32 Launch(int argc, char* argv[])
{
    //@TODO: Parse commnad line
    (void)argc;
    (void)argv;
    #ifdef DEDICATED_SERVER
    LOG_INIT_LEVEL(true, true, "server.log", ELogLevel::Debug);
    #else
    LOG_INIT_LEVEL_FILE("game.log", ELogLevel::Debug);
    #endif
    int32 errorResult = EngineInit();

    while (!IsExitRequested())
    {
        EngineTick();
    }

    EngineExit();

    return errorResult;
}

void Shutdown()
{
    LOG_SHUTDOWN();
}

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;
    int32 result = Launch(argc, argv);
    Shutdown();
    return result;
}