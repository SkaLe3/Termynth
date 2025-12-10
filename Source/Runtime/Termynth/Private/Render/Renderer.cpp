#include "Render/Renderer.h"
#include "Platform/PlatformUtils.h"
#include "Platform/Window.h"
#include "Render/Cell.h"
#include "Render/Texture.h"

#include <algorithm>
#include <cstring>
#include "Core/Logger.h"

void ConsoleRenderer2D::Init(IPlatformWindow *window)
{
   m_Window = window;
   SetRenderBuffer(m_Window->GetWindowBuffer());
   m_FgClearColor = Vector(192, 192, 192);
   m_BgClearColor = Vector(0, 0, 0);
   m_bUseDefaultFgColor = true;
   m_bUseDefaultBgColor = true;
   char16_t clearChar = ' ';
   std::memcpy(m_ClearChar, &clearChar, 2);
}
void ConsoleRenderer2D::Shutdown()
{
}
void ConsoleRenderer2D::BeginFrame()
{
   Clear();
}
void ConsoleRenderer2D::EndFrame()
{
}
void ConsoleRenderer2D::SetRenderBuffer(FrameBuffer *renderBuffer)
{
   m_Framebuffer = renderBuffer;
}
FrameBuffer *ConsoleRenderer2D::GetFrameBuffer()
{
   return m_Framebuffer;
}
void ConsoleRenderer2D::Clear()
{
   ECellAttribute clearAttributes = ECellAttribute::None;
   if (m_bUseDefaultFgColor)
      clearAttributes |= ECellAttribute::DefaultFg;
   if (m_bUseDefaultBgColor)
      clearAttributes |= ECellAttribute::DefaultBg;

   std::for_each(
       m_Framebuffer->GetHandle(),
       m_Framebuffer->GetHandle() + m_Framebuffer->Width * m_Framebuffer->Height,
       [this, clearAttributes](Cell &c)
       {
          std::memcpy(c.Char, m_ClearChar, 2);
          c.Attributes = clearAttributes;
          c.bTransparent = false;
          c.FgR = m_FgClearColor.x;
          c.FgG = m_FgClearColor.y;
          c.FgB = m_FgClearColor.z;
          c.BgR = m_BgClearColor.x;
          c.BgG = m_BgClearColor.y;
          c.BgB = m_BgClearColor.z;
       });
   std::for_each(m_Framebuffer->GetDepth(0, 0), m_Framebuffer->GetDepth(0, 0) + m_Framebuffer->Width * m_Framebuffer->Height,
       [](uint8& depth)
       {
           depth = 0;
       });
   PlatformUtils::Get().MoveCursorTopLeft();
}
void ConsoleRenderer2D::SetFgClearColor(const Vector &color)
{
   m_FgClearColor = color;
}

void ConsoleRenderer2D::SetBgClearColor(const Vector &color)
{
   m_BgClearColor = color;
}
void ConsoleRenderer2D::SetClearChar(uint8 *clearChar)
{
   std::memcpy(m_ClearChar, clearChar, 2);
}

void ConsoleRenderer2D::SetUseDefaultFgColor(bool bUse)
{
   m_bUseDefaultFgColor = bUse;
}
void ConsoleRenderer2D::SetUseDefaultBgColor(bool bUse)
{
   m_bUseDefaultBgColor = bUse;
}
void ConsoleRenderer2D::DrawQuad(const iVector &pos, const iVector2& regionStart, const iVector2& regionSize, Texture *texture)
{
   if (!m_Framebuffer)
      return;

   Cell *start = m_Framebuffer->GetHandle();
   const uint8 *data = texture->GetData();

   int32 bytesInCell = texture->GetBytesInCell();

   for (uint32 x = regionStart.x; x < regionStart.x + regionSize.x; x++)
   {
      for (uint32 y = regionStart.y; y < regionStart.y + regionSize.y; y++)
      {
         int32_t framebufferX = pos.x + x;
         int32_t framebufferY = pos.y + y;
         if (framebufferX >= m_Framebuffer->Width 
               || framebufferY >= m_Framebuffer->Height
               || framebufferX < 0
               || framebufferY < 0)
         {
            continue;   
         } 
         if (*(m_Framebuffer->GetDepth(framebufferX, framebufferY)) > pos.z)
         {
             continue;
         }
         
         int32_t framebufferIndex = framebufferX + framebufferY * m_Framebuffer->Width;
         int32_t textureIndex = (x + y * texture->GetWidth()) * bytesInCell;
         
         uint8* bytePtr = reinterpret_cast<uint8_t*>(start + framebufferIndex);

         uint8 bTransparent = 0;
         std::memcpy(&bTransparent, data + textureIndex + 2, 1);
         if (bTransparent)
         {
            continue;
         }
         *m_Framebuffer->GetDepth(framebufferX, framebufferY) = pos.z;

         switch (bytesInCell)
         {
         case 3:
         case 4:
         case 10:
            std::memcpy(bytePtr, data + textureIndex, bytesInCell);
            break;
         case 9:
            std::memcpy(bytePtr, data + textureIndex, 3);
            std::memcpy(bytePtr + 4, data + textureIndex + 3, 6);
            break;
         }

         // If no color data -> use defaultFg and defaultBg flags
         if (bytesInCell == 3)
         {
            uint8 attributes = 0;
            attributes |= static_cast<uint8>(ECellAttribute::DefaultFg);
            attributes |= static_cast<uint8>(ECellAttribute::DefaultBg);
            std::memcpy(bytePtr + 3, &attributes, 1);
         }
         else if (bytesInCell == 4)
         {
            uint8 attributes = 0;
            std::memcpy(&attributes, bytePtr + 3, 1);
            attributes |= static_cast<uint8>(ECellAttribute::DefaultFg);
            attributes |= static_cast<uint8>(ECellAttribute::DefaultBg);
            std::memcpy(bytePtr + 3, &attributes, 1);
         }
      }
   }
}