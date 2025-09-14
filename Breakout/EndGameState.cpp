#include "EndGameState.h"
#include "GameServices.h"
#include "Renderer.h"
#include "SoundManager.h"
#include "Game.h"
#include "Level1.h"
#include "MenuState.h"
#include <dinput.h>
#include <algorithm>
#include <memory>
#include <cstdlib>

extern Game* g_game;

static float frand(float a, float b) { return a + (b - a) * (rand() / (float)RAND_MAX); }

bool EndGameState::OnEnter(const GameServices& services) {
    srand((unsigned)time(nullptr));

    // Stop everything from the previous state, then troll BGM
    services.soundManager.StopAll();
    services.soundManager.Play("troll_bgm");

    // Title
    m_titleText.textureHandle = services.renderer.LoadTexture("assets/gameover_title.jpg");
    m_titleText.position = { g_game->GetScreenWidth() * 0.5f, g_game->GetScreenHeight() * 0.33f, 0.f };
    m_titleText.scale = { 0.7f, 0.7f, 0.0f };

    // Rick (starts off-screen slightly, slides down & grows)
    m_rick.textureHandle = services.renderer.LoadTexture("assets/rick.png");
    m_rick.position = { g_game->GetScreenWidth() * 0.5f, -100.0f, 0.f };
    m_rick.scale = { 0.4f, 0.4f, 0.0f };
    m_rick.visible = true;

    // Confetti texture: tiny white pixel we can tint/scale
    m_confettiTex = services.renderer.CreateSolidTexture(D3DCOLOR_XRGB(255,255,255));

    // Big initial burst across the top third of screen
    SpawnConfettiBurst(220, 0.f, (float)g_game->GetScreenWidth(), 0.f, (float)g_game->GetScreenHeight()*0.4f, 120.f, 260.f);

    // Rick FX timers
    m_flipTimer = 0.0f;
    m_nextFlipTime = frand(1.0f, 2.2f); // flip every ~1–2.2s at random
    m_rickDir = 1;

    m_time = 0.0f;
    m_shakeTime = 0.0f;
    m_denialsR = 0;
    m_denialsEsc = 0;

    m_playedSfx = true;   // BGM is already started
    m_isInitialized = true;
    return true;
}

void EndGameState::Update(float dt, InputManager& input, PhysicsManager&, SoundManager& sound) {
    if (!m_isInitialized) return;

    m_time += dt;
    if (m_shakeTime > 0.f) m_shakeTime -= dt;
    sound.Update();

    // Title wobble + subtle hue flicker
    const float wobbleX = sinf(m_time * 2.2f) * 10.0f;
    const float wobbleY = cosf(m_time * 1.6f) * 6.0f;
    m_titleText.position.x = g_game->GetScreenWidth() * 0.5f + wobbleX;
    m_titleText.position.y = g_game->GetScreenHeight() * 0.33f + wobbleY;

    // Update bottom text
    m_bottomTextPos.x += wobbleX*0.5;

    // Rick slides down, then “breathes” + flips occasionally
    // --- Rick slide & base scaling ---
	m_rick.position.y += 60.0f * dt;
	if (m_rick.position.y > 140.0f) m_rick.position.y = 140.0f;

	// Base "pulse" scale (up & down a little)
	float pulse = 0.05f * sinf(m_time * 2.0f);
	float baseScale = 0.75f + pulse;

	// --- Flip logic ---
	if (m_flipCooldown > 0.f) {
		m_flipCooldown -= dt;
	}
	else {
		// Trigger a flip every ~3–5 seconds
		if (rand() % 200 == 0) { // ~1/200 chance each frame
			m_isFlipped = !m_isFlipped;
			m_flipCooldown = 1.0f;   // lock out flips for 1 sec
			m_scaleCooldown = 1.0f;  // wait 1 sec before scaling down
		}
	}

	// --- Scale down delay after flip ---
	if (m_scaleCooldown > 0.f) {
		m_scaleCooldown -= dt;
		m_rick.scale.x = baseScale * (m_isFlipped ? -1.0f : 1.0f); // flip only
		m_rick.scale.y = baseScale;
	} else {
		// After waiting, allow scale to shrink
		float shrink = 0.9f + 0.1f * cosf(m_time * 1.3f);
		m_rick.scale.x = baseScale * shrink * (m_isFlipped ? -1.0f : 1.0f);
		m_rick.scale.y = baseScale * shrink;
	}

    // Keyboard trolling
    if (input.IsKeyPressed(DIK_R)) {
        if (m_denialsR < kMaxDenials) {
            ++m_denialsR;
            m_shakeTime = 0.25f;
            sound.Play("hit");         // lil “nope” tick
        }
        else {
            // Finally allow restart (restart the current level)
            g_game->RestartCurrentLevel();
            return;
        }
    }
    if (input.IsKeyPressed(DIK_ESCAPE)) {
        if (m_denialsEsc < kMaxDenials) {
            ++m_denialsEsc;
            m_shakeTime = 0.25f;
            sound.Play("hit");
        }
        else {
            // Finally allow quit to menu
            g_game->ChangeState(std::make_unique<MenuState>());
            return;
        }
    }

    // --- Confetti simulation ---
    // Simple gravity + air-drift + spin; recycle when life <= 0 or off bottom
    const float gravity = 380.f;
    for (auto& c : m_confetti) {
        c.v.y += gravity * dt;
        c.s.position += c.v * dt;
        c.rot += c.rotSpeed * dt;
        c.life -= dt;

        // tiny color shimmer
        if (((int)(m_time * 60) & 7) == 0) {
            // random slight bright flicker
            int r = (rand() % 40);
            c.s.color ^= D3DCOLOR_XRGB(r, 0, 0); // quick playful twinkle
        }

        // fade out near end
        if (c.life < 0.8f) {
            float a = (std::max)(0.f, c.life / 0.8f);
            // keep RGB, scale alpha
            DWORD rgb = (c.s.color & 0x00FFFFFF);
            DWORD a8  = (DWORD)(a * 255.f) << 24;
            c.s.color = a8 | rgb;
        }
    }

    // Remove dead / off-screen pieces
    m_confetti.erase(
        std::remove_if(m_confetti.begin(), m_confetti.end(), [&](const Confetti& c){
            return (c.life <= 0.f) || (c.s.position.y > g_game->GetScreenHeight() + 40.f);
        }),
        m_confetti.end()
    );

    // Gentle periodic re-bursts to keep party going
    if (fmodf(m_time, 1.1f) < dt) { ReburstSmall(); }
}

void EndGameState::OnExit(const GameServices& services) {
	services.soundManager.StopAll();
}

void EndGameState::Render(Renderer& renderer) {
    if (!m_isInitialized) return;

    // Tiny “screen shake” for trolling feedback (applies to title only)
    float shakeX = 0.f, shakeY = 0.f;
    if (m_shakeTime > 0.f) {
        shakeX = frand(-5.f, 5.f);
        shakeY = frand(-4.f, 4.f);
    }
    SpriteInstance shakenTitle = m_titleText;
    shakenTitle.position.x += shakeX;
    shakenTitle.position.y += shakeY;
    shakenTitle.position.z = 1.f;

    // Draw title and Rick
    renderer.DrawSprite(shakenTitle);
    renderer.DrawSprite(m_rick);

     // Confetti (tiny colored rectangles)
    for (auto& c : m_confetti) {
        // Encode rotation into scale skew if your Renderer doesn’t support rotation.
        // If DrawSprite respects scale only, we fake a skinny slanted piece by skewing X vs Y.
        // Otherwise, if your Renderer has a rotated draw, use that and pass c.rot.
        //c.rot(10);
        renderer.DrawSprite(c.s);
    }

    // Main instructions (randomly “misleading” line swaps)
    const bool swap = (sin(m_time * 3.0f) > 0.6f); // flicker between lines
    if (!swap) {
        renderer.DrawTextString(L"Press R to Restart • Esc to Menu",
            280, 460, FlickerWhite());
    }
    else {
        renderer.DrawTextString(L"Press Alt+F4 to Win • Just kidding… maybe",
            260, 460, FlickerWhite());
    }

    renderer.DrawTextString(L"Thanks For Playing!!!!! Never gonna let you downn!", m_bottomTextPos.x, m_bottomTextPos.y, FlickerWhite());
    // Denial counters to tease the user
    if (m_denialsR > 0 && m_denialsR <= kMaxDenials) {
        std::wstring msg = (m_denialsR < kMaxDenials)
            ? L"Not yet. Try R again…"
            : L"Okay okay—one more R will actually restart.";
        renderer.DrawTextString(msg, 360, 500, D3DCOLOR_XRGB(255, 200, 200));
    }
    if (m_denialsEsc > 0 && m_denialsEsc <= kMaxDenials) {
        std::wstring msg = (m_denialsEsc < kMaxDenials)
            ? L"Nah. Are you sure you want to quit?"
            : L"Fine. Esc one more time to quit.";
        renderer.DrawTextString(msg, 360, 520, D3DCOLOR_XRGB(200, 255, 200));
    }

    // Sprinkle lyric bits for fun
    if (fmodf(m_time, 2.5f) < 1.25f) {
        renderer.DrawTextString(L"Never gonna give you up",
            120, 90 + (int)(sinf(m_time * 2.f) * 8), D3DCOLOR_XRGB(255, 255, 255));
    }
    if (fmodf(m_time + 1.25f, 2.5f) < 1.25f) {
        renderer.DrawTextString(L"Never gonna let you down",
            640, 120 + (int)(cosf(m_time * 2.1f) * 8), D3DCOLOR_XRGB(255, 255, 255));
    }
}

D3DCOLOR EndGameState::FlickerWhite() const {
    // Subtle brightness flicker
    int v = (int)(230 + 25 * (0.5f + 0.5f * sinf(m_time * 8.0f)));
    v = (std::max)(0, (std::min)(255, v));
    return D3DCOLOR_XRGB(v, v, v);
}

void EndGameState::SpawnConfettiBurst(int count, float xMin, float xMax, float yMin, float yMax, float speedMin, float speedMax) {
    m_confetti.reserve(m_confetti.size() + count);
    for (int i = 0; i < count; ++i) {
        Confetti c{};
        c.s.textureHandle = m_confettiTex;
        c.s.position = { frand(xMin, xMax), frand(yMin, yMax), 0.f };
        // random tiny rectangle size
        float w = frand(4.f, 8.f);
        float h = frand(6.f, 14.f);
        c.s.scale = { w, h, 0.f };
        // random bright color
        int r = (int)frand(120, 255);
        int g = (int)frand(120, 255);
        int b = (int)frand(120, 255);
        c.s.color = D3DCOLOR_XRGB(r, g, b);
        c.s.visible = true;

        float sp = frand(speedMin, speedMax);
        float ang = frand(-3.14f, 3.14f);
        c.v = { sp * cosf(ang)*0.35f, -fabsf(sp), 0.f }; // initial toss upward-ish
        c.life = frand(1.6f, 2.6f);
        c.rot = frand(0.f, 6.28f);
        c.rotSpeed = frand(-8.f, 8.f);

        m_confetti.push_back(c);
    }
}

void EndGameState::ReburstSmall() {
    // small periodic sprinkle near top center
    float cx = g_game->GetScreenWidth() * 0.5f;
    SpawnConfettiBurst(30, cx - 120.f, cx + 120.f, 0.f, 80.f, 140.f, 260.f);
}

