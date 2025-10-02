#pragma once


class IGame 
{
public:
    virtual ~IGame() = default;
    virtual void Init() = 0;
    virtual void Start() = 0;
    virtual void Tick(float deltaTime) = 0;
    virtual void TickFrame(float deltaTime) = 0;
    virtual void End() = 0;
    virtual void Shutdown() = 0;
};