#pragma once
#include "IGameState.h"
#include "SpriteInstance.h"

class GameoverState : public IGameState
{
public:
    bool OnEnter(const GameServices& services) override;
    void OnExit() override {}
    void Update(float dt, InputManager& input, PhysicsManager&, SoundManager&) override;
    void Render(Renderer& renderer) override;

private:
    SpriteInstance m_background;
    SpriteInstance m_titleText;
    SpriteInstance m_restartText;
    SpriteInstance m_quitText;

    bool  m_isInitialized = false;
    bool  m_playedSfx = false;
};

