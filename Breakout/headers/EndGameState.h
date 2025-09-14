#pragma once
#include "IGameState.h"
#include "SpriteInstance.h"
#include <vector>

class EndGameState : public IGameState
{
public:
    bool OnEnter(const GameServices& services) override;
    void OnExit(const GameServices& services) override;
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
    D3DXVECTOR2 m_bottomTextPos = { 280.f, 560.f };

     // --- Rick FX ---
    float m_rickBaseScale = 0.75f;  // target base scale
    float m_rickPulseAmp  = 0.08f;  // +/- 8% pulse
    float m_flipTimer     = 0.0f;
    float m_nextFlipTime  = 1.5f;   // randomized later
    int   m_rickDir       = 1;      // 1 or -1; negative flips X
    float m_flipCooldown = 0.0f;     // timer until next flip allowed
	bool  m_isFlipped = false;       // current flip state
	float m_scaleCooldown = 0.0f;    // delay before scale down


      // --- Confetti ---
    struct Confetti {
        SpriteInstance s;
        D3DXVECTOR3 v;    // velocity
        float        life; // time to live
        float        rot;  // rotation (radians)
        float        rotSpeed;
    };
    std::vector<Confetti> m_confetti;
    int m_confettiTex = 0;

    // Helper to draw changing/flicker text
	void SpawnConfettiBurst(int count, float xMin, float xMax, float yMin, float yMax, float speedMin, float speedMax);
    void ReburstSmall(); // gentle periodic respawn
    D3DCOLOR FlickerWhite() const;
};
