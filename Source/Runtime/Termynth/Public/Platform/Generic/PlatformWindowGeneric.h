#pragma once
#include "Platform/Window.h"
#include "Render/FrameBuffer.h"
#include "Render/Cell.h"

#include <cstdint>
#include <string>

class PlatformWindowGeneric : public IPlatformWindow
{
public:
    PlatformWindowGeneric(int32_t width, int32_t height);
    virtual ~PlatformWindowGeneric();
    virtual void SwapBuffers() override;
    virtual void ForceRedraw() override;

    virtual FrameBuffer* GetWindowBuffer() override {return m_Framebuffer;} 
private:
    std::string BuildEscapeSequence(const Cell* cc, const Cell* cl);
    void FlushBatchRun(int32& beforeBatchX, int32 lastBatchX, int32 lastBatchY);
    void ResetBatchRun(); 
    void StartNewBatch(int32 newStart, const Cell& newProps);
    bool StartNewBatchIfNeeded(int32 newStart, const Cell& newProps);
    bool IsBatchRunning() const;
    void UpdateBatchBuffer(const Cell& cell); 

protected:
    NativeWindowFrameBuffer* m_Framebuffer;
    bool m_bDirty = false;

    int32 m_BatchStart;
    Cell m_BatchProperties;
    Cell m_LastBatchProperties;
    std::string m_BatchBuffer;
};