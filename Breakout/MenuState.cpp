#include "MenuState.h"
#include "Renderer.h"
#include "InputManager.h"
#include "SoundManager.h"
#include "Level1.h"
#include "Game.h"
#include <dinput.h>
#include <iostream>

bool MenuState::OnEnter(const GameServices& services) {
    m_background.textureHandle = services.renderer.LoadTexture("assets/menu_bg.jpg");
    m_background.position = { 500.f, 300.f, 0.f };

    m_titleText.textureHandle = services.renderer.LoadTexture("assets/MenuTitle.png", 1024, 1024);
    m_titleText.position = { 500.f, 150.f, 0.f };
    m_titleText.scale = { 1.0f, 1.0f, 1.0f };

    m_playButton.textureHandle = services.renderer.LoadTexture("assets/start_text.png");
    m_playButton.position = { 500.f, 400.f, 0.f };
    m_playButton.scale = { 0.2f, 0.2f, 1.f };
    m_playButton.color = D3DCOLOR_XRGB(255, 255, 255);
    m_playHalf = { 140.f * m_playButton.scale.x * 0.5f,
                   40.f * m_playButton.scale.y * 0.5f };

    // Load Red Ball
    m_ball.textureHandle = services.renderer.LoadTexture("assets/red_ball_transparent.png", 1024,1024);
    m_ball.position = { 500.0f, 450.0f, 0.f };
    m_ball.scale    = { 0.5f, 0.5f, 0.f };
    m_ball.animationRows = 5;
    m_ball.animationCols = 5;
    m_ball.framesPerState = 5;
    m_ball.playing = true;
	// Physics Body setup
	m_ballBody.mass = 1.0f;
	m_ballBody.force = D3DXVECTOR3(100, 100, 0);        
	m_ballBody.velocity = BALL_INITIAL_VELOCITY; // initial launch speed

    services.soundManager.Play("bgm");


    m_isInitialized = true;
    return true;
}

void MenuState::OnExit(const GameServices& services) {
    services.soundManager.StopAll();
}

void MenuState::Update(float dt, InputManager& input, PhysicsManager&, SoundManager&) {
    if (input.IsKeyPressed(DIK_RETURN)) { 
        extern Game* g_game;               
        g_game->LoadLevel(1);
        return;
    }
    if (input.IsMousePressed(0)) {
        extern Game* g_game;
        g_game->LoadLevel(1);
        return;
    }

    m_ballBody.UpdatePhysics(dt);
    m_ball.position += m_ballBody.velocity * dt;
    // ball bounce
     // LEFT wall
    if (m_ball.position.x < m_ballHalf.x) {
        m_ball.position.x = m_ballHalf.x;
        if (m_ballBody.velocity.x < 0.0f) m_ballBody.velocity.x = -m_ballBody.velocity.x;
    }
    // RIGHT wall
    if (m_ball.position.x > g_game->GetScreenWidth()  - m_ballHalf.x) {
        m_ball.position.x = (g_game->GetScreenWidth()- m_ballHalf.x);
        if (m_ballBody.velocity.x > 0.0f) m_ballBody.velocity.x = -m_ballBody.velocity.x;
    }
    // TOP wall
    if (m_ball.position.y < m_ballHalf.y) {
        m_ball.position.y = m_ballHalf.y;
        if (m_ballBody.velocity.y < 0.0f) m_ballBody.velocity.y = -m_ballBody.velocity.y;
    }
    // BOTTOM wall
    if (m_ball.position.y > g_game->GetScreenHeight() - m_ballHalf.y) {
        m_ball.position.y = (g_game->GetScreenHeight()  - m_ballHalf.y);
        if (m_ballBody.velocity.y > 0.0f) m_ballBody.velocity.y = -m_ballBody.velocity.y;
    }
}

void MenuState::Render(Renderer& renderer) {
    if (!m_isInitialized) return;
    renderer.DrawSprite(m_background);
    renderer.DrawSprite(m_titleText);
    renderer.DrawSprite(m_playButton);
    renderer.DrawSprite(m_ball);
}