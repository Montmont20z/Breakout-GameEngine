#pragma once
#include "IGameState.h"
#include "SpriteInstance.h"
#include "GameServices.h"
#include "PhysicsBody.h"

class MenuState : public IGameState {
public:
    bool OnEnter(const GameServices& services) override;
    void OnExit(const GameServices& services) override;
    void Update(float dt, InputManager& input, PhysicsManager&, SoundManager&) override;
    void Render(Renderer& renderer) override;


private:
    SpriteInstance m_background;
    SpriteInstance m_titleText;
    SpriteInstance m_playButton;
    D3DXVECTOR2    m_playHalf{ 70.f, 20.f };
    bool m_isInitialized = false;

    // ball
	SpriteInstance m_ball;
    const D3DXVECTOR3 BALL_INITIAL_VELOCITY = D3DXVECTOR3(350.f, -350.f, 0);
    PhysicsBody m_ballBody{};
	D3DXVECTOR2  m_ballHalf     { 20.f, 20.f }; 

};