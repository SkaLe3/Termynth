#pragma once
#include "Game/Game.h"
#include "Render/Console2D/Console2D.h"
#include "Render/Console3D/Console3D.h"
#include "Render/ConsoleUI/ConsoleUI.h"

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

    Console2D& Get2D() {return m_Console2D; }
    void SetCamera2D(const Camera2D& cam) {m_Console2D.SetCamera(cam);}

    ConsoleUI& GetUI() {return m_ConsoleUI; }

    Console3D& Get3D() {return m_Console3D; }
    void SetCamera3D(const Camera3D& cam) {m_Console3D.SetCamera(cam);}

private:
    Console2D m_Console2D;
    ConsoleUI m_ConsoleUI;
    Console3D m_Console3D;
    std::unique_ptr<IGame> m_GameInstance;
};

extern void ApplicationInit();
extern std::unique_ptr<IGame> CreateGameInstance();

extern class Engine* g_Engine;