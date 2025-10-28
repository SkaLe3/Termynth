#include "Platform/Generic/PlatformWindowGeneric.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <vector>

#include "Platform/PlatformUtils.h"
#include "Render/FrameBuffer.h"
#include "Core/Globals.h"
#include "Core/Logger.h"

PlatformWindowGeneric::PlatformWindowGeneric(int32 width, int32 height)
{
    m_Framebuffer = new NativeWindowFrameBuffer(width, height);

    m_BatchStart = -1;
}

PlatformWindowGeneric::~PlatformWindowGeneric()
{
    delete m_Framebuffer;
}

void PlatformWindowGeneric::SwapBuffers()
{
    int32 lastEditedX;
    
    for (int32 y = 0; y < m_Framebuffer->Height; ++y)
    {
        lastEditedX = -2;

        for (int32 x = 0; x < m_Framebuffer->Width; ++x)
        {
            int32 idx = y * m_Framebuffer->Width + x;

            Cell &cc = m_Framebuffer->CurrentBuffer->Handle[idx];
            Cell &cp = m_Framebuffer->PreviousBuffer->Handle[idx];

            bool bDifferent = false;
            switch (GlobalParameters::g_ColorMode)
            {
            case EColorMode::TrueColor:
                bDifferent = cc != cp;
                break;
            case EColorMode::Ansi16:
                bDifferent = !Cell::EqualMapped(cc, cp);
            }
    
            if (bDifferent || m_bDirty)
            {
                bool bNoActiveBatch = StartNewBatchIfNeeded(x, cc);
                if (!bNoActiveBatch && !Cell::MatchPropertiesExact(cc, m_BatchProperties))
                { 
                    FlushBatchRun(lastEditedX, x-1, y);
                    StartNewBatch(x, cc);
                }
                UpdateBatchBuffer(cc);
            }
            else if (IsBatchRunning())
            {
                FlushBatchRun(lastEditedX, x-1, y);
                ResetBatchRun();
            }
        }
        if (IsBatchRunning())
        {
            FlushBatchRun(lastEditedX, m_Framebuffer->Width - 1, y);
            ResetBatchRun();
        }
    }

    std::cout << "\x1b[0m";
    std::cout.flush();
    std::swap(m_Framebuffer->CurrentBuffer, m_Framebuffer->PreviousBuffer);
    if (m_bDirty) m_bDirty = false;
}

void PlatformWindowGeneric::ForceRedraw()
{
    m_bDirty = true;
}
std::string PlatformWindowGeneric::BuildEscapeSequence(const Cell* cprops, const Cell *lprops)
{
    uint8 escapeSequence[16];
    int32 escapeCount = 0;
    ECellAttribute attrToReset = static_cast<ECellAttribute>(~static_cast<uint8>(ECellAttribute::None));
    bool bResetFg = true;
    bool bResetBg = true;
    if (lprops)
    {
        if (!Cell::MatchAttributes(*cprops, *lprops))
            attrToReset = CellAttribute::Difference(cprops->Attributes, lprops->Attributes);
        
        switch (GlobalParameters::g_ColorMode)
        {
        case EColorMode::TrueColor:
            bResetFg = !Cell::MatchFgColorExact(*cprops, *lprops)|| !CellAttribute::MatchDefaultFg(cprops->Attributes, lprops->Attributes);
            bResetBg = !Cell::MatchBgColorExact(*cprops, *lprops) || !CellAttribute::MatchDefaultBg(cprops->Attributes, lprops->Attributes);
            break;
        case EColorMode::Ansi16:
        default:
            bResetFg = !Cell::MatchFgColorMapped(*cprops, *lprops) || !CellAttribute::MatchDefaultFg(cprops->Attributes, lprops->Attributes);
            bResetBg = !Cell::MatchBgColorMapped(*cprops, *lprops) || !CellAttribute::MatchDefaultBg(cprops->Attributes, lprops->Attributes);
            break;
        }
    }
    if (attrToReset != ECellAttribute::None)
    {
        for (size_t i = 0; i < sizeof(CellAttribute::AttributeMasks); ++i)
        {
            ECellAttribute test = static_cast<ECellAttribute>(CellAttribute::AttributeMasks[i]);
            if (CellAttribute::HasFlag(attrToReset, test))
                escapeSequence[escapeCount++] = (cprops->HasAttribute(test) ? CellAttribute::AttrOn[i] : CellAttribute::AttrOff[i]);
        }
    }
    if (bResetFg)
    {
        if (cprops->HasAttribute(ECellAttribute::DefaultFg))
        {
            escapeSequence[escapeCount++] = 39;
        }
        else
        {
            switch (GlobalParameters::g_ColorMode)
            {
            case EColorMode::TrueColor:
                escapeSequence[escapeCount++] = 38;
                escapeSequence[escapeCount++] = 2;
                escapeSequence[escapeCount++] = cprops->FgR;
                escapeSequence[escapeCount++] = cprops->FgG;
                escapeSequence[escapeCount++] = cprops->FgB;
                break;
            case EColorMode::Ansi16:
            default:
                escapeSequence[escapeCount++] = Color::MapBaseCodeToSGR(Color::RGBToAnsiCode(cprops->FgR, cprops->FgG, cprops->FgB), false);
                break;
            }

        }
    }
    if (bResetBg)
    {
        if (cprops->HasAttribute(ECellAttribute::DefaultBg))
        {
            escapeSequence[escapeCount++] = 49;
        }
        else
        {
            switch (GlobalParameters::g_ColorMode)
            {
            case EColorMode::TrueColor:
                escapeSequence[escapeCount++] = 48;
                escapeSequence[escapeCount++] = 2;
                escapeSequence[escapeCount++] = cprops->BgR;
                escapeSequence[escapeCount++] = cprops->BgG;
                escapeSequence[escapeCount++] = cprops->BgB;
                break;
            case EColorMode::Ansi16:
            default:
                escapeSequence[escapeCount++] = Color::MapBaseCodeToSGR(Color::RGBToAnsiCode(cprops->BgR, cprops->BgG, cprops->BgB), true);
                break;
            }

        }
    }

    char finalSequenceBuffer[64];
    int finalSequenceLength = 0;
    finalSequenceLength += std::sprintf(finalSequenceBuffer + finalSequenceLength, "\x1b[");
    for (int32 i = 0; i < escapeCount; ++i)
    {
        if (i > 0)
            finalSequenceLength += std::sprintf(finalSequenceBuffer + finalSequenceLength, ";");
        finalSequenceLength += std::sprintf(finalSequenceBuffer + finalSequenceLength, "%d", escapeSequence[i]);
    }
    finalSequenceLength += std::sprintf(finalSequenceBuffer + finalSequenceLength, "m");
    return std::string(finalSequenceBuffer);
}

void PlatformWindowGeneric::FlushBatchRun(int32& lastCursorX, int32 lastBatchX, int32 row)
{
    //LOG_INFO(std::to_string(m_BatchStart) + " " + std::to_string(lastBatchX) + " '" + m_BatchBuffer + "'");  
    if (lastCursorX != m_BatchStart - 1)
    {
        PlatformUtils::Get().MoveCursor(m_BatchStart + 1, row + 1);
    }
    lastCursorX = lastBatchX;

        Cell* lastBatchProps = m_BatchStart > 0 ? &m_LastBatchProperties : nullptr;
        std::string escapeSequence = BuildEscapeSequence(&m_BatchProperties, lastBatchProps);
        escapeSequence += m_BatchBuffer;
        std::cout << escapeSequence;


    m_LastBatchProperties = m_BatchProperties;
}

void PlatformWindowGeneric::ResetBatchRun()
{
    m_BatchStart = -1;
    m_BatchBuffer.clear();
}

void PlatformWindowGeneric::StartNewBatch(int32 newStart, const Cell& newProps)
{
    m_BatchStart = newStart;
    m_BatchProperties = newProps;
    m_BatchBuffer.clear();
}

bool PlatformWindowGeneric::StartNewBatchIfNeeded(int32 newStart, const Cell& newProps)
{
    if (m_BatchStart < 0)
    {
        StartNewBatch(newStart, newProps);
        return true;
    }
    return false;
}

bool PlatformWindowGeneric::IsBatchRunning() const
{
    return m_BatchStart != -1;
}

void PlatformWindowGeneric::UpdateBatchBuffer(const Cell& cell)
{
    m_BatchBuffer += PlatformUtils::Get().MakeUtf8(cell.Char);
}
