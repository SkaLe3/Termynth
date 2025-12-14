#include "Core/EngineLoop.h"
#include "Core/Core.h"
#include "Platform/PlatformUtils.h"
#include "Core/Globals.h"
#include "Core/App.h"
#include "Engine/Engine.h"
#include "Core/WindowSubsystem.h"
#include "Platform/Window.h"
#include "Render/Renderer.h"
#include "Input/InputManager.h"
#include "Core/Logger.h"
#include "Assets/AssetManager.h"
#include "Utils/Paths.h"
#include "Networking/NetworkingCore.h"

#include <chrono>

int64 g_FrameCounter = 0;


EngineLoop::EngineLoop() {}
EngineLoop::~EngineLoop() {}

int32 EngineLoop::Init()
{
    InitTime();
    PlatformUtils::Init();
    //PlatformUtils::Get().EnableVirtualTerminalProcessing();
    PlatformUtils::Get().InitInput();
    PlatformUtils::Get().HideCursor();

    // Default viewport size
    GlobalParameters::g_ViewportWidth = 80;
    GlobalParameters::g_ViewportHeight = 25;
    GlobalParameters::g_bUseWindowsApiOutput = false;
    GlobalParameters::g_ColorMode = EColorMode::Ansi16;

    // User Init. May override viewport size
    ApplicationInit();

    Net::Networking::Init();

    // @TODO: Parse commnadline arguments to override viewport size here and read port

    #ifndef DEDICATED_SERVER
    WindowSubsystem::Get().Init(GlobalParameters::g_ViewportWidth, GlobalParameters::g_ViewportHeight);
    Renderer::Get().Init(WindowSubsystem::Get().GetWindow());
    #endif
    g_Engine = new Engine();
    // Engine parse command line if needed
    // Make Init profiling
    g_Engine->Init();
    // Make Start profiling
    g_Engine->Start();

    return 0;
}

void EngineLoop::InitTime()
{
    App::SetCurrentTime(std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count());
    App::SetFixedDeltaTime(1 / 30.f);
    App::SetUseFixedTimeStep(true);
}

void EngineLoop::Tick()
{
    BeginExitIfRequested();
    //int64 currentFrame = g_FrameCounter;

    auto DoTick = [&](float deltaTime)
    {
        #ifndef DEDICATED_SERVER
        InputManager::Get().PollInput();
        Renderer::Get().BeginFrame();
        #endif
        g_Engine->Tick(deltaTime);
        #ifndef DEDICATED_SERVER
        Renderer::Get().EndFrame();
        WindowSubsystem::Get().GetWindow()->SwapBuffers();
        #endif
        g_FrameCounter++;
    };

    UpdateTimeStep();
  
    if (App::UseFixedTimeStep())
    {
        while (App::FixedTimeStepReached())
        {
            DoTick(App::GetFixedDeltaTime());
            App::SubtructDeltaTimeAccumulator();
        }
    }
    else
    {
        DoTick(App::GetDeltaTime());
    }

}

void EngineLoop::Exit()
{
    Net::Networking::Shutdown();
    g_Engine->Exit();
    delete g_Engine;
    // Shutdown everything here
#ifndef DEDICATED_SERVER
    PlatformUtils::Get().ShowCursor();
    PlatformUtils::Get().RestoreInput();
#endif
}

void EngineLoop::UpdateTimeStep()
{
    UpdateApplicationLastTime();
    UpateAppTimes();
}

void EngineLoop::UpateAppTimes()
{
    App::SetDeltaTime(App::GetCurrentTime() - App::GetLastTime());
    App::UpdateDeltaTimeAccumulator();
}

void EngineLoop::UpdateApplicationLastTime()
{
    App::UpdateLastTime();
    App::SetCurrentTime(std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count());
}