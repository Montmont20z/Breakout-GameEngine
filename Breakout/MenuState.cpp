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

    m_titleText.textureHandle = services.renderer.LoadTexture("assets/title.png", 130, 40);
    m_titleText.position = { 500.f, 150.f, 0.f };

    m_startText.textureHandle = services.renderer.LoadTexture("assets/start_text.png", 140, 40);
    m_startText.position = { 500.f, 400.f, 0.f };

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
        g_game->ChangeState(std::make_unique<Level1>());
        return;
    }
}

void MenuState::Render(Renderer& renderer) {
    if (!m_isInitialized) return;
    renderer.DrawSprite(m_background);
    renderer.DrawSprite(m_titleText);
    renderer.DrawSprite(m_startText);
}