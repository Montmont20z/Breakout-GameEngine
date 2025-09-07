#include "headers/Game.h"
#include "headers/InputManager.h"
#include "headers/MyWindow.h"
#include <iostream>
#include <chrono>

using namespace std;

Game::Game(HINSTANCE hInstance, int width, int height, int nCmdShow)
    : hInstance(hInstance)
    , m_screenWidth(width)
    , m_screenHeight(height)
    , m_nCmdShow(nCmdShow)
    , m_window(hInstance, width, height, L"Breakout Game")
{
 


}

bool Game::Initialize() {
    if (!m_window.Initialize()) {
        MessageBoxW(nullptr, L"Failed to create window!", L"Error", MB_ICONERROR);
        return false;
    }
    m_window.Show(m_nCmdShow);
    if (!m_inputManager.Initialize(m_window.GetHWND())) {
        MessageBoxW(nullptr, L"Failed to initialize input manager!", L"Error", MB_ICONERROR);
        return false;
    }
    return true;
}

void Game::Run()
{

    using clock = std::chrono::steady_clock;
    auto prev = clock::now();


    while (m_window.ProcessMessages()) {
        // update dt
        auto now = clock::now();
        float dt = std::chrono::duration<float>(now - prev).count();
        prev = now;

        m_inputManager.Update();

        //m_gameState->Update(dt, m_inputManager, m_physicsManager, m_soundManager);

    }
}

void Game::CleanUp()
{
}
