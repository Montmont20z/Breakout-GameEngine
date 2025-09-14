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

    // Rick slides in & scales up a bit
    m_rick.position.y += 60.0f * dt;
    if (m_rick.position.y > 140.0f) m_rick.position.y = 140.0f;
    float targetScale = 0.55f;
    m_rick.scale.x += (targetScale - m_rick.scale.x) * (1.5f * dt);
    m_rick.scale.y = m_rick.scale.x;

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

    // Draw title and Rick
    renderer.DrawSprite(shakenTitle);
    renderer.DrawSprite(m_rick);

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
