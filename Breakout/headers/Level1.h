#pragma once
#include "SpriteInstance.h"
#include "IGameState.h"
#include "GameServices.h"
#include "PhysicsBody.h"
#include <vector>

struct BlueBall {
    SpriteInstance sprite;
    PhysicsBody body;
    D3DXVECTOR2 half;
};

class Level1 : public IGameState
{
public:
    bool OnEnter(const GameServices& services) override;
    void OnExit(const GameServices& services) override;
	void Update(float dt, InputManager&, PhysicsManager&, SoundManager&) override;
	void Render(Renderer& renderer) override;
private:
    bool m_isInitialized = false;
    int  m_whiteTex = 0;

    int life = 3;
    const D3DXVECTOR3 BALL_INITIAL_VELOCITY = D3DXVECTOR3(150.f, -150.f, 0);
	static constexpr int MAX_BRICKS = 40;

    // Sprites in level
    SpriteInstance m_bricksList[MAX_BRICKS];
    SpriteInstance m_redBrick;
    SpriteInstance m_singlePaddle;
    SpriteInstance m_ball;
    SpriteInstance m_background;
    int m_brickCount   = 0;  // how many were actually created this level 
    int m_aliveBricks  = 0;  // how many are still visible

    // Blue Bricks
	SpriteInstance m_blueBrick;
    bool m_isBlue[MAX_BRICKS] = { false };
    std::vector<BlueBall> m_blueBalls;

    // Physics Body
    PhysicsBody m_ballBody{};
    PhysicsBody m_paddleBody{};

    // default value, set value at OnEnter()
	D3DXVECTOR2  m_ballHalf     { 20.f, 20.f }; 
	D3DXVECTOR2  m_paddleHalf   { 96.f, 8.f  };         

    void SpawnBlueBallAt(const D3DXVECTOR3& pos);
};

