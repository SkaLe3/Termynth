#include "Render/FrameBuffer.h"

#include "Render/Cell.h"

FrameBuffer::FrameBuffer(int32 width, int32 height)
    : Width(width), Height(height)
{
    Handle = new Cell[width * height];
    DepthBuffer = new uint8[width * height](0);
}
FrameBuffer::~FrameBuffer()
{
    delete[] Handle;
}

Cell* FrameBuffer::GetHandle() const
{
    return Handle;
}

uint8* FrameBuffer::GetDepth(int32 x, int32 y) const
{
    return &DepthBuffer[x + y * Width];
}

NativeWindowFrameBuffer::NativeWindowFrameBuffer(int32 width, int32 height)
    : FrameBuffer(width, height)
{
    CurrentBuffer = this;
    PreviousBuffer = new FrameBuffer(width, height);
    AllocatedMemory = PreviousBuffer;
}

NativeWindowFrameBuffer::~NativeWindowFrameBuffer()
{
    delete AllocatedMemory;
}

Cell* NativeWindowFrameBuffer::GetHandle() const
{
    return CurrentBuffer->Handle;
}

uint8* NativeWindowFrameBuffer::GetDepth(int32 x, int32 y) const
{
    return &CurrentBuffer->DepthBuffer[x + y * Width];
}

