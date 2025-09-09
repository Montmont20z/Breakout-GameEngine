#include "Level1.h"
#include "Renderer.h"
#include "SpriteInstance.h"
#include "inputManager.h"
#include "PhysicsManager.h"
#include <d3dx9.h>
#include <iostream>
#include <dinput.h>

using namespace std;

static D3DXVECTOR3 makePos(float x, float y) { return D3DXVECTOR3(x, y, 0); }

bool Level1::OnEnter(const GameServices& services) {
    // Bricks Layout
    m_whiteTex = services.renderer.CreateSolidTexture(D3DCOLOR_ARGB(255, 255, 255, 255));
    // Brick ordering variable
    const float brickWidth = 64.f, brickHeight = 32.f;
    const float columnCount = 10.f, rowCount = 3.f, gap = 10.f;
    // ========================
    m_redBrick.textureHandle = m_whiteTex;
    m_redBrick.scale = D3DXVECTOR3(brickWidth, brickHeight, 0);
    m_redBrick.color = D3DCOLOR_ARGB(255, 255, 0, 0); // red
    m_redBrick.visible = false;
	m_redBrick.position = makePos(100, 100);
    // Create the Brick layout
    float posX = 0, posY = 50;
    int totalBrick = 0;
    for (int j = 0; j < rowCount; j++) {
        posY += brickHeight + gap;
		for (int i = 0; i < columnCount; i++) {
			posX += brickWidth + gap;
			m_bricksList[totalBrick] = m_redBrick.CloneWithNewId();
			m_bricksList[totalBrick].position = makePos(posX, posY);
            //cout << "PosX: " << posX << " PosY: " << posY << endl;
			m_bricksList[totalBrick].visible = true;
            totalBrick++;
		}
        posX = 0;
    }

    // Load Bg
    m_background.textureHandle = services.renderer.LoadTexture("assets/bg2.png");
    m_background.position = { 500.0f, 300.0f, 0.f };
    
    // Load Ball
    m_ball.textureHandle = services.renderer.LoadTexture("assets/red_ball_transparent.png", 1024,1024);
    m_ball.position = { 500.0f, 450.0f, 0.f };
    m_ball.scale    = { 0.2f, 0.2f, 1.f };
    m_ball.animationRows = 5;
    m_ball.animationCols = 5;
    m_ball.framesPerState = 5;
    m_ball.playing = true;
    
    // Load paddle
    m_singlePaddle.textureHandle = services.renderer.LoadTexture("assets/singlePaddle.png");
    m_singlePaddle.animationRows = 1;
    m_singlePaddle.animationCols = 1;
    m_singlePaddle.framesPerState = 1;
    m_singlePaddle.position = { 500.f, 500.f, 0.f };
    m_singlePaddle.scale    = { 3.f, 1.f, 1.f };
    m_singlePaddle.color    = D3DCOLOR_XRGB(255,255,255);

    // Ball half-size from logical frame size (1024x1024 sheet, 5x5, scaled 0.2)
	const float frameW = (1024.0f / m_ball.animationCols) * m_ball.scale.x; // 1024 / 5 * 0.2 ≈ 40.96
	const float frameH = (1024.0f / m_ball.animationRows) * m_ball.scale.y;
	m_ballHalf = D3DXVECTOR2(frameW * 0.5f, frameH * 0.5f);

	// Paddle half-size (e.g., ~192x16 → {96,8})
	m_paddleHalf = D3DXVECTOR2(96.f, 8.f);

	// Physics Body setup
	m_ballBody.mass = 1.0f;
	m_ballBody.force = D3DXVECTOR3(0, 0, 0);             // no gravity for breakout
	m_ballBody.velocity = D3DXVECTOR3(220.f, -220.f, 0); // initial launch speed

	m_paddleBody.mass = 0.0f;     // immovable/kinematic for collision response
	m_paddleBody.velocity = D3DXVECTOR3(0, 0, 0);

    m_isInitialized = true;
    return true;

}

void Level1::Update(float dt, InputManager& inputManager, PhysicsManager& physicsManager, SoundManager&) {
    // Update Animation Sprite
    m_ball.UpdateAnimation(dt);

    const float screenW = 1000.f, screenH = 600.f;

    // --- 1) Paddle input (kinematic) ---
    const float paddleSpeed = 300.f;
    if (inputManager.IsKeyDown(DIK_LEFT))  m_singlePaddle.position.x -= paddleSpeed * dt;
    if (inputManager.IsKeyDown(DIK_RIGHT)) m_singlePaddle.position.x += paddleSpeed * dt;

    // Clamp paddle
    if (m_singlePaddle.position.x < m_paddleHalf.x) m_singlePaddle.position.x = m_paddleHalf.x;
    if (m_singlePaddle.position.x > screenW - m_paddleHalf.x) m_singlePaddle.position.x = screenW - m_paddleHalf.x;

    // Keep paddle body "kinematic"
    m_paddleBody.velocity = D3DXVECTOR3(0, 0, 0);
    m_paddleBody.acceleration = D3DXVECTOR3(0, 0, 0);
    m_paddleBody.force = D3DXVECTOR3(0, 0, 0);

    // --- 2) Ball physics integration (force -> accel -> vel -> pos) ---
    // (Apply forces here if you want, e.g., gravity: m_ballBody.ApplyForce({0, 980.f * m_ballBody.mass, 0});)
    m_ballBody.UpdatePhysics(dt);
    m_ball.position += m_ballBody.velocity * dt;

    // --- 3) Wall bounce using AABB (center vs half-size) ---
    const float eps = 0.5f; // small push-away

    // LEFT wall
    if (m_ball.position.x < m_ballHalf.x) {
        m_ball.position.x = m_ballHalf.x + eps;
        if (m_ballBody.velocity.x < 0.0f) m_ballBody.velocity.x = -m_ballBody.velocity.x;
    }
    // RIGHT wall
    if (m_ball.position.x > screenW - m_ballHalf.x) {
        m_ball.position.x = (screenW - m_ballHalf.x) - eps;
        if (m_ballBody.velocity.x > 0.0f) m_ballBody.velocity.x = -m_ballBody.velocity.x;
    }
    // TOP wall
    if (m_ball.position.y < m_ballHalf.y) {
        m_ball.position.y = m_ballHalf.y + eps;
        if (m_ballBody.velocity.y < 0.0f) m_ballBody.velocity.y = -m_ballBody.velocity.y;
    }
    // BOTTOM wall
    if (m_ball.position.y > screenH - m_ballHalf.y) {
        m_ball.position.y = (screenH - m_ballHalf.y) - eps;
        if (m_ballBody.velocity.y > 0.0f) m_ballBody.velocity.y = -m_ballBody.velocity.y;
    }

    // --- 4) Ball vs Paddle AABB resolve (moving body vs static solid) ---
    physicsManager.ResolveAABB(
        m_ball, m_ballBody, m_ballHalf,
        m_singlePaddle, m_paddleHalf,
        0.9f // restitution
    );
}

void Level1::Render(Renderer& renderer) {
    // Render order, first draw at back, last draw at front
	renderer.DrawSprite(m_background);
	renderer.DrawSprite(m_singlePaddle);
	//renderer.DrawSprite(m_redBrick);
    for (auto& brick : m_bricksList) renderer.DrawSprite(brick);
    renderer.DrawSprite(m_ball);


}

