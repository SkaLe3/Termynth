#pragma once
#include "Core/Core.h"



class Cell;

class FrameBuffer
{
public:
    FrameBuffer(int32_t width, int32_t height);
    virtual ~FrameBuffer();

    virtual Cell* GetHandle() const;

    int32_t Width;
    int32_t Height;
    Cell* Handle;
};


class NativeWindowFrameBuffer : public FrameBuffer
{
public:
    NativeWindowFrameBuffer(int32 width, int32 height);
    ~NativeWindowFrameBuffer();

    virtual Cell* GetHandle() const override;
public:
    FrameBuffer* CurrentBuffer;
    FrameBuffer* PreviousBuffer;

private:
    FrameBuffer* AllocatedMemory; 
};

