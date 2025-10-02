#include "Sandbox.h"
#include "Engine/Engine.h"
#include "Core/WindowSubsystem.h"
#include "Render/FrameBuffer.h"
#include "Core/Globals.h"
#include "Platform/Window.h"
#include "Render/Cell.h"
#include "Render/Color.h"
#include "Core/Logger.h"

#include <fstream>


void ApplicationInit()
{
    GlobalParameters::g_ViewportWidth = 20;
    GlobalParameters::g_ViewportHeight = 20;    
    GlobalParameters::g_ColorMode = EColorMode::TrueColor;
    GlobalParameters::g_bUseWindowsApiOutput = true;
}

class Sandbox : public IGame
{
public:

    virtual void Init() override;
    virtual void Start() override {}
    virtual void Tick(float deltaTime) override;
    virtual void TickFrame(float deltaTime) override;
    virtual void End() override {}
    virtual void Shutdown() override {}

private:
    float m_Pos = 0;
};

void Sandbox::Init()
{

}

void Sandbox::Tick(float deltaTime)
{
    float speed = 5.f;
    m_Pos += speed * deltaTime;
}

void Sandbox::TickFrame(float deltaTime)
{
    float divisor = 5;
    for (int i = 0; i < 500; i += divisor)
    {
        int idx = ((int32)(m_Pos) + i) % (GlobalParameters::g_ViewportWidth * GlobalParameters::g_ViewportHeight);
        Cell* cell = WindowSubsystem::Get().GetWindow()->GetWindowBuffer()->GetHandle();
        cell[idx].Char = '0' + ((int)(i / divisor) % 10);
        cell[idx].Attributes = ECellAttribute::None;
        //cell[idx].Attributes |= ECellAttribute::DefaultBg;
        //cell[idx].Attributes |= ECellAttribute::DefaultFg;
        //cell[idx].Attributes |= ECellAttribute::Bold;
        //cell[idx].Attributes |= ECellAttribute::Italic;
        cell[idx].SetBgColor(Vector(20, 45, 60));
        cell[idx].SetFgColor(+EColorAnsi16::Ruby);
        if (idx == 1)
        {
            //LOG_INFO("Idx 1 raw attributes: " + std::to_string((uint8)cell[idx].Attributes));
        }
    } 
}

std::unique_ptr<IGame> CreateGameInstance()
{
    return std::make_unique<Sandbox>();
}