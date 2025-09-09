#pragma once
#include <chrono>
#include "MyWindow.h"
#include "Renderer.h"
#include "InputManager.h"
#include "PhysicsManager.h"
#include "SoundManager.h"
#include "IGameState.h"

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

public:
	Game(HINSTANCE hInstance, int width, int height, int nCmdShow); 
	bool Initialize(); 
	void Run();
	void CleanUp();
	void ChangeState(std::unique_ptr<IGameState> next);
	int GetScreenWidth();
	int GetScreenHeight();
};

extern Game* g_game;

