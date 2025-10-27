#include "Engine/Engine.h"
#include "Core/Globals.h"
#include "Core/Core.h"
#include "Core/WindowSubsystem.h"
#include "Platform/Window.h"
#include "Render/FrameBuffer.h"


#include "Core/App.h"
#include <iostream>

Engine* g_Engine = nullptr;

Engine::Engine()
{

}

void Engine::Init()
{
    m_GameInstance = CreateGameInstance();
    m_GameInstance->Init();
}

void Engine::Start()
{
    m_GameInstance->Start();
}

void Engine::Tick(float deltaTime)
{
    m_GameInstance->Tick(deltaTime);

}

void Engine::TickFrame(float deltaTime)
{
    m_GameInstance->TickFrame(deltaTime);
}

void Engine::Exit()
{
    m_GameInstance->End();
    m_GameInstance->Shutdown();
}