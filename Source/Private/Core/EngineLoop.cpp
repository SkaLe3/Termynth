#include "Core/EngineLoop.h"
#include "Platform/PlatformUtils.h"
#include "Core/Globals.h"
#include "Core/App.h"
#include "Engine/Engine.h"
#include "Core/WindowSubsystem.h"
#include "Platform/Window.h"
#include "Render/Renderer.h"
#include "Input/InputManager.h"
#include "Core/Logger.h"

#include <chrono>

int64 g_FrameCounter = 0;

EngineLoop::EngineLoop() {}
EngineLoop::~EngineLoop() {}

int32 EngineLoop::Init()
{
    InitTime();
    PlatformUtils::Init();
    PlatformUtils::Get().InitInput();
    PlatformUtils::Get().HideCursor();

    // Default viewport size
    GlobalParameters::g_ViewportWidth = 80;
    GlobalParameters::g_ViewportHeight = 25;
    GlobalParameters::g_bUseWindowsApiOutput = false;
    GlobalParameters::g_ColorMode = EColorMode::Ansi16;

    // User Init. May override viewport size
    ApplicationInit();

    // TODO: Parse commnadline arguments to override viewport size here

    WindowSubsystem::Get().Init(GlobalParameters::g_ViewportWidth, GlobalParameters::g_ViewportHeight);
    Renderer::Get().Init(WindowSubsystem::Get().GetWindow());
    g_Engine = new Engine();
    // Engine parse command line if needed in the future
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

    InputManager::Get().PollInput();

    UpdateTimeStep();
  
    Renderer::Get().BeginFrame();
    if (App::UseFixedTimeStep())
    {
        while (App::FixedTimeStepReached())
        {
            g_Engine->Tick(App::GetFixedDeltaTime());
            App::SubtructDeltaTimeAccumulator();
        }
    }
    else
    {
        g_Engine->Tick(App::GetDeltaTime());
    }
    g_Engine->TickFrame(App::GetDeltaTime());

    Renderer::Get().EndFrame();

    WindowSubsystem::Get().GetWindow()->SwapBuffers();

    g_FrameCounter++;
}

void EngineLoop::Exit()
{
    g_Engine->Exit();
    delete g_Engine;
    // Shutdown everything here
    PlatformUtils::Get().ShowCursor();
    PlatformUtils::Get().RestoreInput();
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