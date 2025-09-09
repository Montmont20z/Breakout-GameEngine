#pragma once
#include "SpriteInstance.h"
#include "IGameState.h"
#include "GameServices.h"
#include "PhysicsBody.h"

class Level1 : public IGameState
{
public:
    bool OnEnter(const GameServices& services) override;
	void OnExit() override {}
	void Update(float dt, InputManager&, PhysicsManager&, SoundManager&) override;
	void Render(Renderer& renderer) override;
private:
    bool m_isInitialized = false;
    int  m_whiteTex = 0;
    //SpriteInstance corner_[4];
    //SpriteInstance center_;
    //SpriteInstance overA_, overB_; // for layering & alpha
    //float angle_ = 0.0f;
    int life = 5;
    const D3DXVECTOR3 BALL_INITIAL_VELOCITY = D3DXVECTOR3(150.f, -150.f, 0);

    // Sprites in level
    SpriteInstance m_bricksList[40];
    SpriteInstance m_redBrick;
    SpriteInstance m_blueBrick;
    SpriteInstance m_singlePaddle;
    SpriteInstance m_background;
    SpriteInstance m_ball;

    // Physics Body
    PhysicsBody m_ballBody{};
    PhysicsBody m_paddleBody{};

    //D3DXVECTOR3  m_ballVelocity { 220.f, -220.f, 0.f }; // px/sec

    // default value, set value at OnEnter()
	D3DXVECTOR2  m_ballHalf     { 20.f, 20.f }; 
	D3DXVECTOR2  m_paddleHalf   { 96.f, 8.f  };         
};

