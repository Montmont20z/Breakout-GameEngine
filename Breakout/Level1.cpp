#include "Level1.h"
#include "Renderer.h"
#include "SpriteInstance.h"
#include <d3dx9.h>
#include <iostream>

using namespace std;

static D3DXVECTOR3 makePos(float x, float y) { return D3DXVECTOR3(x, y, 0); }

bool Level1::OnEnter(const GameServices& services) {
    m_whiteTex = services.renderer.CreateSolidTexture(D3DCOLOR_ARGB(255, 255, 255, 255));
    const float brickWidth = 64.f, brickHeight = 32.f;
    m_redBrick.textureHandle = m_whiteTex;
    m_redBrick.scale = D3DXVECTOR3(brickWidth, brickHeight, 1);
    m_redBrick.color = D3DCOLOR_ARGB(255, 255, 0, 0); // red
    m_redBrick.visible = true;
	m_redBrick.position = makePos(100, 100);
	
    cout << "redBrick Id: " << m_redBrick.id() << endl;

    float posX = 0, posY = 50;
    for (int i = 0; i < 10; i++) {
        posX += 70;
        m_bricksList[i] = m_redBrick.CloneWithNewId();
        m_bricksList[i].position = makePos(posX, posY);
        m_bricksList[i].visible = true;
    }

    m_background.textureHandle = services.renderer.LoadTexture("assets/bg2.png");
    m_background.position = { 500.0f, 300.0f, 0.f };
    
    cout << "BG Id: " << m_background.id() << endl;
    

    m_singlePaddle.textureHandle = services.renderer.LoadTexture("assets/singlePaddle.png");
    m_singlePaddle.animationRows = 1;
    m_singlePaddle.animationCols = 1;
    m_singlePaddle.framesPerState = 1;
    m_singlePaddle.position = { 500.f, 500.f, 0.f };
    m_singlePaddle.scale    = { 3.f, 1.f, 1.f };
    m_singlePaddle.color    = D3DCOLOR_XRGB(255,255,255);

    cout << "Paddle Id: " << m_singlePaddle.id() << endl;
	cout << "redBrick spriteId=" << m_redBrick.id()
     << " texHandle=" << m_redBrick.textureHandle << endl;

	cout << "Paddle spriteId=" << m_singlePaddle.id()
     << " texHandle=" << m_singlePaddle.textureHandle << endl;
 

    m_isInitialized = true;
    return true;

}

void Level1::Update(float dt, InputManager&, PhysicsManager&, SoundManager&) {
    angle_ += dt;                        // spin ~1 rad/s
    center_.rotation = angle_;
}

void Level1::Render(Renderer& renderer) {
    //if (!m_isInitialized) {
    //    // 1x1 white tex (we’ll tint with SpriteInstance.color)
    //    m_whiteTex = renderer.CreateSolidTexture(D3DCOLOR_ARGB(255, 255, 255, 255));

    //    const float W = 64, H = 64;      // draw size on screen (scaled from 1x1)

    //    // corners (verifies coordinate system & transforms)
    //    corner_[0].id() = m_whiteTex; corner_[0].position = makePos(50, 50);   corner_[0].scale = D3DXVECTOR3(W, H, 1); corner_[0].color = D3DCOLOR_ARGB(255, 255, 0, 0);   // red
    //    corner_[1].id() = m_whiteTex; corner_[1].position = makePos(950, 50);   corner_[1].scale = D3DXVECTOR3(W, H, 1); corner_[1].color = D3DCOLOR_ARGB(255, 0, 255, 0);   // green
    //    corner_[2].id() = m_whiteTex; corner_[2].position = makePos(50, 550);   corner_[2].scale = D3DXVECTOR3(W, H, 1); corner_[2].color = D3DCOLOR_ARGB(255, 0, 0, 255);   // blue
    //    corner_[3].id() = m_whiteTex; corner_[3].position = makePos(950, 550);  corner_[3].scale = D3DXVECTOR3(W, H, 1); corner_[3].color = D3DCOLOR_ARGB(255, 255, 255, 0); // yellow

    //    // center spinning square (verifies pivot/origin + rotation)
    //    center_.id() = m_whiteTex;
    //    center_.position = makePos(500, 300);
    //    center_.scale = D3DXVECTOR3(96, 96, 1);   // big enough to see
    //    center_.color = D3DCOLOR_ARGB(255, 255, 255, 255); // white

    //    // layering + alpha: A under B (B is translucent)
    //    overA_.id() = m_whiteTex; overA_.position = makePos(600, 300);
    //    overA_.scale = D3DXVECTOR3(120, 120, 1);
    //    overA_.color = D3DCOLOR_ARGB(255, 255, 0, 255);   // magenta, opaque

    //    overB_.id() = m_whiteTex; overB_.position = makePos(630, 330);
    //    overB_.scale = D3DXVECTOR3(120, 120, 1);
    //    overB_.color = D3DCOLOR_ARGB(128, 0, 0, 0);       // 50% black

    //    m_isInitialized = true;
    //}

    // Draw order is the z-order in immediate mode:
    //for (auto& s : corner_) renderer.DrawSprite(s); // 4 corners
    //renderer.DrawSprite(center_);                   // spinning center
    //renderer.DrawSprite(overA_);                    // bottom magenta
    //renderer.DrawSprite(overB_);                    // top semi-transparent black
	renderer.DrawSprite(m_background);
	renderer.DrawSprite(m_singlePaddle);
	//renderer.DrawSprite(m_redBrick);
    for (auto& brick : m_bricksList) renderer.DrawSprite(brick);





}

