#pragma once
#include "Game/Game.h"
#include <memory>

class Engine
{
public:
    Engine();
    void Init();
    void Start();
    void Tick(float deltaTime);
    void TickFrame(float deltaTime);
    void Exit();

private:
    float m_Pos = 0;

    std::unique_ptr<IGame> m_GameInstance;
};

extern void ApplicationInit();
extern std::unique_ptr<IGame> CreateGameInstance();

extern class Engine* g_Engine;