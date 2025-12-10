#include "Engine/Engine.h"
#include "Core/Globals.h"
#include "Core/Core.h"
#include "Core/WindowSubsystem.h"
#include "Platform/Window.h"
#include "Render/FrameBuffer.h"
#include "Engine/AssetManager.h"
#include "Engine/TextureLoader.h"
#include "Engine/SpriteLoader.h"
#include "Utils/Paths.h"


#include "Core/App.h"
#include <iostream>
#include <memory>

Engine* g_Engine = nullptr;

Engine::Engine()
{

}

void Engine::Init()
{
    auto& assetManager = AssetManager::Get();
    assetManager.Initialize(Paths::EngineContentDir(), Paths::ProjectContentDir());
    assetManager.RegisterLoader(std::make_shared<TextureLoader>());
    assetManager.RegisterLoader(std::make_shared<SpriteLoader>());
    assetManager.MountPak("PongTerminal.pak"); // Temporary name  

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
    AssetManager::Get().Shutdown();
}