#pragma once
#include "Core/Core.h"

class Cell;

// Note: This comment was generated using GPT to correctly explain class purpose, but the class itself was implemented manually with no AI
/**
 * @brief Represents a generic framebuffer.
 *
 * This class stores a 2D grid of cells and can be used for rendering operations.
 * It provides a handle to access the underlying memory for reading or writing cells.
 *
 * FrameBuffer is suitable for offscreen rendering where double buffering is not required.
 * It simply manages a single buffer of the specified width and height.
 * 
 * Always use GenHandle() function to get correct data handle of the CurrentBuffer, 
 * since Handle variable may contain PreviousBuffer address if instance runtime type is NativeWindowFrameBuffer
 */
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

// Note: This comment was generated using GPT to correctly explain class purpose, but the class itself was implemented manually with no AI
/**
 * @brief Framebuffer specialized for use with windows.
 *
 * This class supports double buffering for window to optimize window redraws.
 * Only cells that have changed between frames are updated, improving performance.
 *
 * NativeWindowFrameBuffer maintains two FrameBuffer instances:
 * - CurrentBuffer: stores the current frame being drawn.
 * - PreviousBuffer: stores the last frame that was displayed.
 *
 * AllocatedMemory is used internally to hold the actual cell data and stores correct address to delete in the destructor,
 * since CurrentBuffer and PreviousBuffer constantly swap addresses
 *
 * Use this class when rendering to a window or interactive display where minimizing redraws is important.
 * For offscreen rendering or non-interactive buffers, the base FrameBuffer class is sufficient,
 * and double buffering is unnecessary.
 * 
 *
 */
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

