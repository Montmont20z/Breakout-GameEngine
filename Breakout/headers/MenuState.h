#pragma once
#include "IGameState.h"
#include "SpriteInstance.h"
#include "GameServices.h"

class MenuState : public IGameState {
public:
    bool OnEnter(const GameServices& services) override;
    void OnExit(const GameServices& services) override;
    void Update(float dt, InputManager& input, PhysicsManager&, SoundManager&) override;
    void Render(Renderer& renderer) override;

private:
    SpriteInstance m_background;
    SpriteInstance m_titleText;
    SpriteInstance m_startText;
    bool m_isInitialized = false;
};