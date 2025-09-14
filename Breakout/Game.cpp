#include "headers/Game.h"
#include "InputManager.h"
#include "headers/MyWindow.h"
#include "headers/Level1.h"
#include "headers/MenuState.h"
#include "headers/YouWinState.h"
#include "headers/EndGameState.h"
#include <iostream>
#include <memory>
#include <GameServices.h>
#include <Level2.h>

using namespace std;

extern Game* g_game = nullptr;

Game::Game(HINSTANCE hInstance, int width, int height, int nCmdShow)
    : hInstance(hInstance)
    , m_screenWidth(width)
    , m_screenHeight(height)
    , m_nCmdShow(nCmdShow)
    , m_window(hInstance, width, height, L"Breakout Game")
{}

bool Game::Initialize() {
    g_game = this;
    if (!m_window.Initialize()) {
        MessageBoxW(nullptr, L"Failed to create window!", L"Error", MB_ICONERROR);
        return false;
    }
     if (!m_renderer.Initialize(m_window.GetHWND(), m_window.GetWidth(), m_window.GetHeight())) {
        MessageBoxW(nullptr, L"Failed to initialize renderer!", L"Error", MB_ICONERROR);
        return false;
    }
    m_window.Show(m_nCmdShow);
    if (!m_inputManager.Initialize(m_window.GetHWND())) {
        MessageBoxW(nullptr, L"Failed to initialize input manager!", L"Error", MB_ICONERROR);
        return false;
    }
    if (!m_soundManager.Initialize()){
        MessageBoxW(nullptr, L"Failed to initialize sound manager!", L"Error", MB_ICONERROR);
        return false;
    }
    m_soundManager.Load("hit", "assets/glass_hit.wav");
    m_soundManager.Load("hard_hit", "assets/hard_hit.wav");
    m_soundManager.Load("gameover", "assets/gameover_sound.wav");
    m_soundManager.Load("you_win", "assets/you_win.mp3");
    m_soundManager.Load("damage", "assets/damage_sound.wav");
    m_soundManager.Load("troll_bgm", "assets/never_gonna_give_you_up.mp3", /*stream*/true, /*loop*/true);
    m_soundManager.Load("bgm", "assets/ctr_title.mp3", true, true);
    m_soundManager.Load("ingame_bgm", "assets/ctr_ingame.mp3", true, true);
    //ChangeState(std::make_unique<EndGameState>());
  
    // set mouse logical size
    m_inputManager.SetLogicalSize(g_game->GetScreenWidth(), g_game->GetScreenHeight());
    g_game->LoadLevel(0);

    return true;
}

void Game::Run()
{

    using clock = std::chrono::steady_clock;
    auto prev = clock::now();


    while (m_window.ProcessMessages()) {
        // ensure correct state
		PumpPendingState();
        if (!m_gameState) {
            // fallback to a default state to avoid null deref
            ChangeState(std::make_unique<MenuState>());
            continue;
        }
        // update dt
        auto now = clock::now();
        float dt = std::chrono::duration<float>(now - prev).count();
        prev = now;

        m_inputManager.Update();

        m_renderer.BeginFrame();
		m_gameState->Update(dt, m_inputManager, m_physicsManager, m_soundManager);
        m_gameState->Render(m_renderer);
        m_renderer.EndFrame();

    }
}

void Game::CleanUp()
{
}

void Game::ChangeState(std::unique_ptr<IGameState> next)
{
	GameServices svc{ m_renderer, m_inputManager, m_physicsManager, m_soundManager };
    if (m_gameState) m_gameState->OnExit(svc);
    m_gameState = std::move(next);
    if (m_gameState) {
        m_gameState->OnEnter(svc);
    }
}

int Game::GetScreenWidth() {
    return m_screenWidth;
}
int Game::GetScreenHeight() {
    return m_screenHeight;
}

void Game::RegisterLevel(int id, std::function<std::unique_ptr<IGameState>()> factory) {
    m_levels[id] = std::move(factory);
    if (id > m_maxLevelId) m_maxLevelId = id;
}

void Game::RequestState(std::unique_ptr<IGameState> next) {
    m_pending = std::move(next);
}

void Game::PumpPendingState() {
    if (!m_pending) return;

    GameServices svc{ m_renderer, m_inputManager, m_physicsManager, m_soundManager };
    if (m_gameState) m_gameState->OnExit(svc);
    m_gameState = std::move(m_pending);
    if (m_gameState) m_gameState->OnEnter(svc);
}


void Game::LoadLevel(int id) {
    auto it = m_levels.find(id);
    if (it == m_levels.end()) return; // unknown level
    m_currentLevel = id;
    RequestState(it->second());        // queue creation
}

void Game::LoadNextLevel() {
    if (m_currentLevel < 0) return;             // not in a level yet
    for (int id = m_currentLevel + 1; id <= m_maxLevelId; ++id) {
        if (m_levels.count(id)) { LoadLevel(id); return; }
    }
    // no next level -> go to Menu (optional)
    RequestState(std::make_unique<MenuState>());
}

void Game::RestartCurrentLevel() {
    if (m_currentLevel < 0) return;
    LoadLevel(m_currentLevel);
}
