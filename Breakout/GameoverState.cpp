#include "GameoverState.h"
#include "GameServices.h"
#include "Renderer.h"
#include "SoundManager.h"
#include "Game.h"
#include "Level1.h"
#include "MenuState.h"
#include <dinput.h>
#include <algorithm>
#include <memory>

extern Game* g_game;

bool GameoverState::OnEnter(const GameServices& services) {
    // Title: “GAME OVER”
    // Use your own image file; fallback to any existing title image if needed
    m_titleText.textureHandle = services.renderer.LoadTexture("assets/gameover_title.jpg");
    m_titleText.position = { g_game->GetScreenWidth()*0.5f-100.0f, g_game->GetScreenHeight()*0.3333f, 0.f};
    m_titleText.scale    = { 0.7f, 0.7f, 0.0f };

    m_playedSfx  = false;
    m_isInitialized = true;
    return true;

}

void GameoverState::Update(float dt, InputManager& input, PhysicsManager&, SoundManager& soundManager)
{
     if (!m_isInitialized) return;

    // Play once on entry 
    if (!m_playedSfx) {
        soundManager.Play("gameover");
        m_playedSfx = true;
    }
    soundManager.Update();

    // Quick shortcuts
	extern Game* g_game;
    if (input.IsKeyPressed(DIK_R)) { // Restart
        g_game->RestartCurrentLevel();
        return;
    }
    if (input.IsKeyPressed(DIK_ESCAPE)) { // Quit to Menu
        g_game->ChangeState(std::make_unique<MenuState>());
        return;
    }
}

void GameoverState::Render(Renderer& renderer) {
     if (!m_isInitialized) return;

    renderer.DrawSprite(m_titleText);
    renderer.DrawTextString(L"Press R to Restart • Esc to Menu", 350, 450, D3DCOLOR_XRGB(255,255,255));

}
