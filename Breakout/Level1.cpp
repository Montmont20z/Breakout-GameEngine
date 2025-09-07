#include "Level1.h"
#include "Renderer.h"
#include "SpriteInstance.h"
#include <d3dx9.h>

static D3DXVECTOR3 makePos(float x, float y) { return D3DXVECTOR3(x, y, 0); }

bool Level1::OnEnter(const GameServices& services) {

    paddleTexId_ = services.renderer.LoadTexture("assets/singlePaddle.png");

    singlePaddle_.textureId = paddleTexId_;
    singlePaddle_.animationRows = 1;
    singlePaddle_.animationCols = 1;
    singlePaddle_.framesPerState = 1;
    singlePaddle_.position = { 500.f, 560.f, 0.f };
    singlePaddle_.scale    = { 1.f, 1.f, 1.f };
    singlePaddle_.color    = D3DCOLOR_XRGB(255,255,255);


    inited_ = true;
    return true;

}

void Level1::Update(float dt, InputManager&, PhysicsManager&, SoundManager&) {
    angle_ += dt;                        // spin ~1 rad/s
    center_.rotation = angle_;
}

void Level1::Render(Renderer& renderer) {
    if (!inited_) {
        // 1x1 white tex (we’ll tint with SpriteInstance.color)
        whiteTex_ = renderer.CreateSolidTexture(D3DCOLOR_ARGB(255, 255, 255, 255));

        const float W = 64, H = 64;      // draw size on screen (scaled from 1x1)

        // corners (verifies coordinate system & transforms)
        corner_[0].textureId = whiteTex_; corner_[0].position = makePos(50, 50);   corner_[0].scale = D3DXVECTOR3(W, H, 1); corner_[0].color = D3DCOLOR_ARGB(255, 255, 0, 0);   // red
        corner_[1].textureId = whiteTex_; corner_[1].position = makePos(950, 50);   corner_[1].scale = D3DXVECTOR3(W, H, 1); corner_[1].color = D3DCOLOR_ARGB(255, 0, 255, 0);   // green
        corner_[2].textureId = whiteTex_; corner_[2].position = makePos(50, 550);   corner_[2].scale = D3DXVECTOR3(W, H, 1); corner_[2].color = D3DCOLOR_ARGB(255, 0, 0, 255);   // blue
        corner_[3].textureId = whiteTex_; corner_[3].position = makePos(950, 550);  corner_[3].scale = D3DXVECTOR3(W, H, 1); corner_[3].color = D3DCOLOR_ARGB(255, 255, 255, 0); // yellow

        // center spinning square (verifies pivot/origin + rotation)
        center_.textureId = whiteTex_;
        center_.position = makePos(500, 300);
        center_.scale = D3DXVECTOR3(96, 96, 1);   // big enough to see
        center_.color = D3DCOLOR_ARGB(255, 255, 255, 255); // white

        // layering + alpha: A under B (B is translucent)
        overA_.textureId = whiteTex_; overA_.position = makePos(600, 300);
        overA_.scale = D3DXVECTOR3(120, 120, 1);
        overA_.color = D3DCOLOR_ARGB(255, 255, 0, 255);   // magenta, opaque

        overB_.textureId = whiteTex_; overB_.position = makePos(630, 330);
        overB_.scale = D3DXVECTOR3(120, 120, 1);
        overB_.color = D3DCOLOR_ARGB(128, 0, 0, 0);       // 50% black

        inited_ = true;
    }

    // Draw order is the z-order in immediate mode:
    for (auto& s : corner_) renderer.DrawSprite(s); // 4 corners
    renderer.DrawSprite(center_);                   // spinning center
    renderer.DrawSprite(overA_);                    // bottom magenta
    renderer.DrawSprite(overB_);                    // top semi-transparent black
    renderer.DrawSprite(singlePaddle_);
}

