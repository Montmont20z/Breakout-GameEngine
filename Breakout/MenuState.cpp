#include "MenuState.h"
#include "Renderer.h"
#include "InputManager.h"
#include "SoundManager.h"
#include "Level1.h"
#include "Game.h"
#include <dinput.h>
#include <iostream>
#include <Windows.h>

bool MenuState::OnEnter(const GameServices& services) {
    m_background.textureHandle = services.renderer.LoadTexture("assets/menu_bg3.png");
    m_background.position = { 500.f, 300.f, 0.f };
    m_background.scale = { 1.0f, 1.0f, 0.0f };

    m_titleText.textureHandle = services.renderer.LoadTexture("assets/MenuTitleTransparent.png", 1024, 1024);
    m_titleText.position = { 500.f, 150.f, 0.f };
    m_titleText.scale = { 1.0f, 1.0f, 0.0f };

    m_playButton.textureHandle = services.renderer.LoadTexture("assets/start_text.png");
    m_playButton.position = { 500.f, 350.f, 0.f };
    m_playButton.scale = { 0.2f, 0.08f, 0.f };
    m_playButton.color = D3DCOLOR_XRGB(255, 255, 255);
    //m_playHalf = { 140.f * m_playButton.scale.x * 0.5f,
                   //40.f * m_playButton.scale.y * 0.5f };
    const float playNativeW = 1024.f, playNativeH = 1024.f;
	m_playHalf = { playNativeW * m_playButton.scale.x * 0.5f, playNativeH * m_playButton.scale.y * 0.5f };

    m_quitButton.textureHandle = services.renderer.LoadTexture("assets/quit_text.png");
	m_quitButton.position      = { 500.f, 480.f, 0.f };
	m_quitButton.scale         = { 0.2f, 0.08f, 0.f };
	m_quitButton.color         = D3DCOLOR_XRGB(255, 255, 255);
    const float quitNativeW = 1024.f, quitNativeH = 1024.f;
	m_quitHalf = { quitNativeW * m_quitButton.scale.x * 0.5f, quitNativeH * m_quitButton.scale.y * 0.5f };

	// half-extents for simple AABB hit-test (match your play sizing approach)
	//m_quitHalf = { 140.f * m_quitButton.scale.x * 0.5f, 40.f  * m_quitButton.scale.y * 0.5f };

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
    
	  // Cursor
	ShowCursor(FALSE);            // hide native cursor
    g_game->cursor.textureHandle = services.renderer.LoadTexture("assets/cursor.png");
	//g_game->cursor.scale = {0.03125f, 0.03125f, 0.f}; // 512x512 to 16x16
	g_game->cursor.scale = {0.03125f, 0.03125f, 0.f}; // 512x512 to 16x16


    services.soundManager.Play("bgm");


    m_isInitialized = true;
    return true;
}

void MenuState::OnExit(const GameServices& services) {
    ShowCursor(TRUE);
    services.soundManager.StopAll();
}

void MenuState::Update(float dt, InputManager& input, PhysicsManager&, SoundManager& sound) {
 
    // --- keyboard quick start ---
    if (input.IsKeyPressed(DIK_RETURN)) {
        extern Game* g_game;
        g_game->LoadLevel(1);
        return;
    }

    // --- mouse hit-test helper ---
    auto hitRect = [](const D3DXVECTOR3& p, const D3DXVECTOR2& half, float mx, float my) {
        return (mx >= p.x - half.x) && (mx <= p.x + half.x) &&
               (my >= p.y - half.y) && (my <= p.y + half.y);
    };
    const float mx = static_cast<float>(input.GetMouseX());
	const float my = static_cast<float>(input.GetMouseY());

	// Move visual cursor sprite (optional)
	g_game->cursor.position = { mx, my, 0.f };

	// Your hit-test (unchanged) now uses mx/my that live in the same logical space as your sprites
	bool overPlay = hitRect(m_playButton.position, m_playHalf, mx, my);
	bool overQuit = hitRect(m_quitButton.position, m_quitHalf, mx, my);

    m_playButton.color = overPlay ? D3DCOLOR_XRGB(255, 230, 150) : D3DCOLOR_XRGB(255, 255, 255);
    m_quitButton.color = overQuit ? D3DCOLOR_XRGB(255, 230, 150) : D3DCOLOR_XRGB(255, 255, 255);

    // click to trigger
    if (input.IsMousePressed(0)) { // left click just pressed
        if (overPlay) {
            extern Game* g_game;
            g_game->LoadLevel(1);
            return;
        }
        if (overQuit) {
            // leave game cleanly
            PostQuitMessage(0);
            return;
        }
    }

      // Volume control
    if (input.IsKeyPressed(DIK_P)) {
        float v = sound.GetVolume(SoundManager::Bus::Master);
        sound.SetVolume(SoundManager::Bus::Master, v + 0.1f);
    }
    if (input.IsKeyPressed(DIK_O)) {
        float v = sound.GetVolume(SoundManager::Bus::Master);
        sound.SetVolume(SoundManager::Bus::Master, v - 0.1f);
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
    renderer.DrawSprite(m_quitButton);  
    renderer.DrawSprite(m_ball);
    renderer.DrawSprite(g_game->cursor);
}