#pragma once
#include "SpriteInstance.h"
#include "IGameState.h"
#include "GameServices.h"

class Level1 : public IGameState
{
public:
    bool OnEnter(const GameServices& services) override;
	void OnExit() override {}
	void Update(float dt, InputManager&, PhysicsManager&, SoundManager&) override;
	void Render(Renderer& renderer) override;
private:
    bool inited_ = false;
    int  whiteTex_ = 0;
    SpriteInstance corner_[4];
    SpriteInstance center_;
    SpriteInstance overA_, overB_; // for layering & alpha
    float angle_ = 0.0f;

	int  paddleTexId_ = 0;
    SpriteInstance singlePaddle_;
};

