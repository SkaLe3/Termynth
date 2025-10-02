#pragma once

class FrameBuffer;

class IPlatformWindow
{
public:
    virtual ~IPlatformWindow() {}
    virtual void SwapBuffers() = 0;
    virtual void ForceRedraw() = 0;
    virtual FrameBuffer* GetWindowBuffer() = 0;
};
