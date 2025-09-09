#include "headers/Game.h"
#include "headers/InputManager.h"
#include "headers/MyWindow.h"
#include "headers/Level1.h"
#include "headers/MenuState.h"
#include <iostream>
//#include <chrono>
#include <memory>
#include <GameServices.h>

using namespace std;

Game* g_game = nullptr;

Game::Game(HINSTANCE hInstance, int width, int height, int nCmdShow)
    : hInstance(hInstance)
    , m_screenWidth(width)
    , m_screenHeight(height)
    , m_nCmdShow(nCmdShow)
    , m_window(hInstance, width, height, L"Breakout Game")
{}

bool Game::Initialize() {
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
    //m_soundManager.LoadSounds();
    m_soundManager.Load("hit", "assets/glass_hit.wav");
    m_soundManager.Load("gameover", "assets/gameover_sound.wav");
    m_soundManager.Load("damage", "assets/damage_sound.wav");
    m_soundManager.Load("troll_bgm", "assets/never_gonna_give_you_up.mp3", /*stream*/true, /*loop*/true);
    m_soundManager.Load("bgm", "assets/ctr_title.mp3");
    //m_soundManager.Play("bgm");



    ChangeState(std::make_unique<MenuState>());

    return true;
}

void Game::Run()
{

    using clock = std::chrono::steady_clock;
    auto prev = clock::now();

    //m_soundManager.LoadSounds();
    //m_soundManager.PlaySound1();
    //m_soundManager.PlaySoundTrack();


    while (m_window.ProcessMessages()) {
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
    if (m_gameState) m_gameState->OnExit();
    m_gameState = std::move(next);
    if (m_gameState) {
        GameServices svc{ m_renderer, m_inputManager, m_physicsManager, m_soundManager };
        m_gameState->OnEnter(svc);
    }
}

int Game::GetScreenWidth() {
    return m_screenWidth;
}
int Game::GetScreenHeight() {
    return m_screenHeight;
}
