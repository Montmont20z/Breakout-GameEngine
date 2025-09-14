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

    m_playButton.textureHandle = services.renderer.LoadTexture("assets/start_text.png",);
    m_playButton.position = { 500.f, 400.f, 0.f };
    m_playButton.scale = { 1.5f, 1.5f, 1.f };
    m_playButton.color = D3DCOLOR_XRGB(255, 255, 255);
    m_playHalf = { 140.f * m_playButton.scale.x * 0.5f,
                   40.f * m_playButton.scale.y * 0.5f };

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
        g_game->RequestState(std::make_unique<Level1>());
        return;
    }
    if (input.IsMousePressed(0)) {
        extern Game* g_game;
        g_game->ChangeState(std::make_unique<Level1>());
        return;
    }
}

void MenuState::Render(Renderer& renderer) {
    if (!m_isInitialized) return;
    renderer.DrawSprite(m_background);
    renderer.DrawSprite(m_titleText);
    renderer.DrawSprite(m_playButton);
}