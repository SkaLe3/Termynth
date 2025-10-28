#include "Render/Renderer.h"
#include "Platform/PlatformUtils.h"
#include "Platform/Window.h"
#include "Render/Cell.h"

#include <algorithm>




void ConsoleRenderer2D::Init(IPlatformWindow* window)
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
void ConsoleRenderer2D::SetRenderBuffer(FrameBuffer* renderBuffer)
{
   m_Framebuffer = renderBuffer;
}
FrameBuffer* ConsoleRenderer2D::GetFrameBuffer()
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
      m_Framebuffer->GetHandle() + m_Framebuffer->Width * m_Framebuffer->Height * sizeof(Cell),
      [this, clearAttributes](Cell &c)
      {
         std::memcpy(c.Char, m_ClearChar, 2);
         c.Attributes = clearAttributes;
         c.FgR = m_FgClearColor.x;
         c.FgG = m_FgClearColor.y;
         c.FgB = m_FgClearColor.z;
         c.BgR = m_BgClearColor.x;
         c.BgG = m_BgClearColor.y;
         c.BgB = m_BgClearColor.z;
      }
   );
   PlatformUtils::Get().MoveCursorTopLeft();
}
void ConsoleRenderer2D::SetFgClearColor(const Vector& color)
{
   m_FgClearColor = color;
}

void ConsoleRenderer2D::SetBgClearColor(const Vector& color)
{
   m_BgClearColor = color;
}
void ConsoleRenderer2D::SetClearChar(uint8* clearChar)
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