#include "Render/FrameBuffer.h"

#include "Render/Cell.h"

FrameBuffer::FrameBuffer(int32 width, int32 height)
    : Width(width), Height(height)
{
    Handle = new Cell[width * height];
}
FrameBuffer::~FrameBuffer()
{
    delete[] Handle;
}

Cell* FrameBuffer::GetHandle() const
{
    return Handle;
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