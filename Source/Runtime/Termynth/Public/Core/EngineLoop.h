#pragma once
#include "Core/Core.h"

#include <memory>

class EngineLoop
{
public:
    EngineLoop();
    ~EngineLoop();
    int32 Init();
    void InitTime();
    void Exit();
    void Tick();

private:
    void UpdateTimeStep();
    void UpateAppTimes();
    void UpdateApplicationLastTime();
};

extern EngineLoop g_EngineLoop;
