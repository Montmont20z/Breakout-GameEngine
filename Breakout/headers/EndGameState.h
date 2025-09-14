#pragma once
#include "IGameState.h"
#include "SpriteInstance.h"

class EndGameState : public IGameState
{
public:
    bool OnEnter(const GameServices& services) override;
    void OnExit(const GameServices& services) override {};
    void Update(float dt, InputManager& input, PhysicsManager&, SoundManager& sound) override;
    void Render(Renderer& renderer) override;

private:
    SpriteInstance m_titleText{};
    SpriteInstance m_rick{};
    bool   m_isInitialized = false;
    bool   m_playedSfx = false;

    // Troll state
    float  m_time = 0.0f;   // animation clock
    float  m_shakeTime = 0.0f;   // short shakes when we “deny”
    int    m_denialsR = 0;      // times we denied R
    int    m_denialsEsc = 0;      // times we denied Esc
    static constexpr int kMaxDenials = 2; // after this, allow the action

    // Helper to draw changing/flicker text
    D3DCOLOR FlickerWhite() const;
};
