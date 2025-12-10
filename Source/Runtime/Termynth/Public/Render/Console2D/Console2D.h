#pragma once
#include "Render/Console2D/Camera2D.h"
#include "Core/Math/Math.h"

#include "Engine/SpriteAsset.h"

#include "Render/Renderer.h"
#include "Render/Sprite.h"



class Console2D
{
public:
    void SetCamera(const Camera2D& cam);

    void DrawSprite(const Vector& pos, std::shared_ptr<Sprite> sprite);
    void DrawRect(const Vector& pos, const iVector2& size, const iVector2& origin = {0, 0});
    void DrawRect(const Vector& pos, const iVector2& size, const Vector& color, const iVector2& origin = {0, 0});
    void DrawText(const Vector& pos, const std::string& text);
    void DrawText(const Vector& pos, const std::string& text, const Vector& color);
private:
    Camera2D m_Camera;
};
