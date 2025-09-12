#include "YouWinState.h"
#include "GameServices.h"
#include "Renderer.h"
#include "SoundManager.h"
#include "Game.h"
#include "Level1.h"
#include "Level2.h"
#include "MenuState.h"
#include <dinput.h>
#include <algorithm>
#include <memory>

extern Game* g_game;

bool YouWinState::OnEnter(const GameServices& services) {
    // Use your own image file; fallback to any existing title image if needed
    m_titleText.textureHandle = services.renderer.LoadTexture("assets/you_win.jpg");
    m_titleText.position = { g_game->GetScreenWidth()*0.5f-100.0f, g_game->GetScreenHeight()*0.3333f, 0.f};
    m_titleText.scale    = { 0.7f, 0.7f, 0.0f };

    m_playedSfx  = false;
    m_isInitialized = true;
    return true;

}

void YouWinState::Update(float dt, InputManager& input, PhysicsManager&, SoundManager& soundManager)
{
     if (!m_isInitialized) return;

    // Play once on entry 
    if (!m_playedSfx) {
        soundManager.Play("you_win");
        m_playedSfx = true;
    }
    soundManager.Update();

    // Quick shortcuts
	extern Game* g_game;
    if (input.IsKeyPressed(DIK_N)) {
		g_game->LoadNextLevel();  // <<< reusable
		return;
	}
	if (input.IsKeyPressed(DIK_R)) {
		g_game->RestartCurrentLevel(); // <<< reusable
		return;
	}
	if (input.IsKeyPressed(DIK_ESCAPE)) {
		g_game->RequestState(std::make_unique<MenuState>());
		return;
	}

}

void YouWinState::Render(Renderer& renderer) {
     if (!m_isInitialized) return;

    renderer.DrawSprite(m_titleText);
    renderer.DrawTextString(L"Press N for Next Level • R to Restart • Esc to Menu", 350, 450, D3DCOLOR_XRGB(255,255,255));

}

