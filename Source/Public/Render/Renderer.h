#pragma once

#include "Render/FrameBuffer.h"
#include "Core/Math/Math.h"

class IPlatformWindow;

class IRenderer
{
public:
    virtual ~IRenderer() = default;

    virtual void Init(IPlatformWindow* window) = 0;
    virtual void Shutdown() = 0;
    virtual void BeginFrame() = 0;
    virtual void EndFrame() = 0;

    virtual void SetRenderBuffer(FrameBuffer* renderBuffer) = 0;
    virtual FrameBuffer* GetFrameBuffer() = 0;

    virtual void Clear() = 0;
    virtual void SetFgClearColor(const Vector& color) = 0;
    virtual void SetBgClearColor(const Vector& color) = 0;
    virtual void SetClearChar(char clearChar) = 0; 
    virtual void SetUseDefaultFgColor(bool bUse) = 0;
    virtual void SetUseDefaultBgColor(bool bUse) = 0;
};

class ConsoleRenderer2D : public IRenderer
{
public:
    virtual void Init(IPlatformWindow* window) override;
    virtual void Shutdown() override;
    virtual void BeginFrame() override;
    virtual void EndFrame() override;

    virtual void SetRenderBuffer(FrameBuffer* renderBuffer) override;
    virtual FrameBuffer* GetFrameBuffer() override;

    virtual void Clear() override;
    virtual void SetFgClearColor(const Vector& color) override;
    virtual void SetBgClearColor(const Vector& color) override;
    virtual void SetClearChar(char clearChar) override;
    virtual void SetUseDefaultFgColor(bool bUse) override;
    virtual void SetUseDefaultBgColor(bool bUse) override;

private:
    IPlatformWindow* m_Window;
    FrameBuffer* m_Framebuffer;
    Vector m_FgClearColor;
    Vector m_BgClearColor;
    bool m_bUseDefaultFgColor;
    bool m_bUseDefaultBgColor;
    char m_ClearChar;
};

class Renderer
{
public:
    static IRenderer& Get()
    {
        static ConsoleRenderer2D s_Renderer;
        return s_Renderer;
    }
private:
    Renderer() = default;
    ~Renderer() = default;

    Renderer(const Renderer&) = delete;
    Renderer(Renderer&&) = delete;
};