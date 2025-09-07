#pragma once
#include <chrono>
#include "Renderer.h"
#include "InputManager.h"
#include "MyWindow.h"
#include "SpriteInstance.h"
#include "IGameState.h"
#include "PhysicsManager.h"
#include "SoundManager.h"

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

	//// Timing
	//using clock = std::chrono::high_resolution_clock;
	//clock::time_point lastTick{};

	//// Game objects
	//SpriteInstance  background{ "assets/bg3.png", D3DXVECTOR3(0,0,0), 0 };
	//SpriteInstance  paddle{ "assets/singlePaddle.png", D3DXVECTOR3(400, 500, 0), 1 };
	//SpriteInstance  ball{ "assets/ball.png", D3DXVECTOR3(500, 300, 0), 2 };

	//// Game state
	//D3DXVECTOR3     ballSpeed = D3DXVECTOR3(-5.0f, -5.0f, 0.0f);

	//// Helpers
	////virtual void update(float dt);
	//void render();

public:
	Game(HINSTANCE hInstance, int width, int height, int nCmdShow); 
	bool Initialize(); 
	void Run();
	void CleanUp();
};

