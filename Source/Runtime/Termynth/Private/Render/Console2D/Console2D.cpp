#include "Render/Console2D/Console2D.h"

#include "Core/Globals.h"

void Console2D::SetCamera(const Camera2D& cam)
{
    m_Camera = cam;
}
void Console2D::DrawSprite(const Vector &pos, std::shared_ptr<Sprite> sprite)
{
    iVector screenpos;
    screenpos.x = pos.x - m_Camera.Position.x;
    screenpos.y = pos.y - m_Camera.Position.y;

    screenpos.x = screenpos.x + GlobalParameters::g_ViewportWidth / 2;
    screenpos.y = screenpos.y + GlobalParameters::g_ViewportHeight / 2;

    Renderer::GetConsole2D().DrawQuad(screenpos, sprite->GetRectStart(),sprite->GetRectSize(), sprite->GetTexture());
}
void Console2D::DrawRect(const Vector &pos, const iVector2 &size, const iVector2 &origin)
{
}

void Console2D::DrawRect(const Vector &pos, const iVector2 &size, const Vector &color, const iVector2 &origin)
{
}

void Console2D::DrawText(const Vector &pos, const std::string &text)
{
}

void Console2D::DrawText(const Vector &pos, const std::string &text, const Vector &color)
{
}
