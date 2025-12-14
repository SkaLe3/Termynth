#include "Render/Console2D/Console2D.h"

#include "Core/Globals.h"
#include "Core/Logger.h"

void Console2D::SetCamera(const Camera2D& cam)
{
    m_Camera = cam;
}
void Console2D::DrawSprite(const Vector &pos, std::shared_ptr<Sprite> sprite)
{
    Vector screenpos;
    screenpos.x = pos.x - static_cast<float>(m_Camera.Position.x);
    screenpos.y = pos.y - static_cast<float>(m_Camera.Position.y);
    screenpos.z = pos.z;

    screenpos = screenpos - Vector(Vector2(sprite->GetOrigin()), 0.f);
    screenpos = screenpos - Vector(Vector2(sprite->GetRectStart()), 0.f);

    //LOG_INFO("x: " + std::to_string(screenpos.x) + " |y: " + std::to_string(screenpos.y));
    screenpos.x = screenpos.x + GlobalParameters::g_ViewportWidth / 2.f;
    screenpos.y = screenpos.y + GlobalParameters::g_ViewportHeight / 2.f;

    //screenpos.x = std::round(screenpos.x);
    //screenpos.y = std::round(screenpos.y);



    Renderer::GetConsole2D().DrawQuad(iVector(screenpos), sprite->GetRectStart(),sprite->GetRectSize(), sprite->GetTexture());
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
