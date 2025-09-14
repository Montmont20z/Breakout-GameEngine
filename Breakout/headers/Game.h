#pragma once
#include <chrono>
#include "MyWindow.h"
#include "Renderer.h"
#include "InputManager.h"
#include "PhysicsManager.h"
#include "SoundManager.h"
#include "IGameState.h"
#include <functional>
#include <unordered_map>

class IGameState; class Level1;

class Game
{
private:
	std::unique_ptr<IGameState> m_gameState;
	MyWindow m_window;
	Renderer m_renderer;
	InputManager m_inputManager;
	SoundManager m_soundManager;
	PhysicsManager m_physicsManager;
	HINSTANCE hInstance;
	int m_screenWidth;
	int m_screenHeight;
	int m_nCmdShow;

	std::unordered_map<int, std::function<std::unique_ptr<IGameState>()>> m_levels;
    int m_currentLevel = -1;
    int m_maxLevelId   = -1;

    std::unique_ptr<IGameState> m_pending; // queued next state

public:
	Game(HINSTANCE hInstance, int width, int height, int nCmdShow); 
	bool Initialize(); 
	void Run();
	void CleanUp();
	void ChangeState(std::unique_ptr<IGameState> next);
	int GetScreenWidth();
	int GetScreenHeight();
	 // Register and load levels by integer id
    void RegisterLevel(int id, std::function<std::unique_ptr<IGameState>()> factory);
    void LoadLevel(int id);         // queue switch to specific level
    void LoadNextLevel();           // queue switch to next existing level
    void RestartCurrentLevel();     // queue restart of current level
    int  CurrentLevel() const { return m_currentLevel; }

    // Safer queued switch (use this instead of direct ChangeState inside Update)
    void RequestState(std::unique_ptr<IGameState> next);

    // Call this once per frame at a safe point (before Update/Render):
    void PumpPendingState();
};

extern Game* g_game;

