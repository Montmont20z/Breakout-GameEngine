#include "Level1.h"
#include "Renderer.h"
#include "SpriteInstance.h"
#include "inputManager.h"
#include "PhysicsManager.h"
#include "SoundManager.h"
#include "Game.h"
#include <d3dx9.h>
#include <iostream>
#include <dinput.h>
#include <sstream>
#include "GameoverState.h"
#include "YouWinState.h"
#include <cstdlib>
#include <ctime>

using namespace std;
extern Game* g_game;

static D3DXVECTOR3 makePos(float x, float y) { return D3DXVECTOR3(x, y, 0); }

bool Level1::OnEnter(const GameServices& services) {
	services.soundManager.Play("ingame_bgm");

    // Bricks Layout
    m_whiteTex = services.renderer.CreateSolidTexture(D3DCOLOR_ARGB(255, 255, 255, 255));
    // Brick ordering variable
    const float brickWidth = 64.f, brickHeight = 32.f;
    const float columnCount = 12.f, rowCount = 3.f, gap = 10.f;
    // ========================
    // Create RedBrick Prototype
    m_redBrick.textureHandle = m_whiteTex;
    m_redBrick.scale = D3DXVECTOR3(brickWidth, brickHeight, 0);
    m_redBrick.color = D3DCOLOR_ARGB(255, 255, 0, 0); // red
    m_redBrick.visible = false;
	m_redBrick.position = makePos(100, 100);

    // Create Blue Brick Prototype
    m_blueBrick = m_redBrick; // copy base size & scale
    m_blueBrick.color = D3DCOLOR_ARGB(255, 0, 120, 255); // blue tint
    //m_blueBrick.textureHandle = services.renderer.Load
    m_blueBalls.clear();

    // Random assign some bricks to blue
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    const float blueChance = 0.20f; // 20% chance blue

    // Create the Brick layout
    float posX = 0, posY = 50;
	m_brickCount  = 0;
	m_aliveBricks = 0;
    for (int j = 0; j < rowCount; j++) {
        posY += brickHeight + gap;
		for (int i = 0; i < columnCount; i++) {
			posX += brickWidth + gap;

            // default: red
			m_bricksList[m_brickCount] = m_redBrick.CloneWithNewId();
			m_bricksList[m_brickCount].position = makePos(posX, posY);
            //cout << "PosX: " << posX << " PosY: " << posY << endl;
			m_bricksList[m_brickCount].visible = true;

            // Randomly flip to blue
            if ((std::rand() / (float)RAND_MAX) < blueChance) {
                m_bricksList[m_brickCount].color = m_blueBrick.color;
                m_isBlue[m_brickCount] = true;
            }
            else {
                m_isBlue[m_brickCount] = false;
            }

            m_brickCount++;
			m_aliveBricks++;
		}
        posX = 0;
    }

    // Load Bg
    m_background.textureHandle = services.renderer.LoadTexture("assets/bg2.png");
    m_background.position = { 500.0f, 300.0f, 0.f };
    
    // Load Red Ball
    m_ball.textureHandle = services.renderer.LoadTexture("assets/red_ball_transparent.png", 1024,1024);
    m_ball.position = { 500.0f, 450.0f, 0.f };
    m_ball.scale    = { 0.2f, 0.2f, 1.f };
    m_ball.animationRows = 5;
    m_ball.animationCols = 5;
    m_ball.framesPerState = 5;
    m_ball.playing = true;

    
    // Load paddle
    m_singlePaddle.textureHandle = services.renderer.LoadTexture("assets/singlePaddle.png");
    m_singlePaddle.animationRows = 1;
    m_singlePaddle.animationCols = 1;
    m_singlePaddle.framesPerState = 1;
    m_singlePaddle.position = { 500.f, 500.f, 0.f };
    m_singlePaddle.scale    = { 3.f, 1.f, 1.f };
    m_singlePaddle.color    = D3DCOLOR_XRGB(255,255,255);

    // Ball half-size from logical frame size (1024x1024 sheet, 5x5, scaled 0.2)
	const float frameW = (1024.0f / m_ball.animationCols) * m_ball.scale.x; // 1024 / 5 * 0.2 ≈ 40.96
	const float frameH = (1024.0f / m_ball.animationRows) * m_ball.scale.y;
	m_ballHalf = D3DXVECTOR2(frameW * 0.5f, frameH * 0.5f);

	// Paddle half-size (e.g., ~192x16 → {96,8})
	m_paddleHalf = D3DXVECTOR2(96.f, 8.f);

	// Physics Body setup
	m_ballBody.mass = 1.0f;
	m_ballBody.force = D3DXVECTOR3(0, 0, 0);             // no gravity for breakout
	m_ballBody.velocity = BALL_INITIAL_VELOCITY; // initial launch speed

	m_paddleBody.mass = 0.0f;     // immovable/kinematic for collision response
	m_paddleBody.velocity = D3DXVECTOR3(0, 0, 0);

    m_isInitialized = true;
    return true;

}

void Level1::OnExit(const GameServices& services) {
	services.soundManager.StopAll();
}

void Level1::Update(float dt, InputManager& inputManager, PhysicsManager& physicsManager, SoundManager& soundManager) {
    // Change Game State
	// Lose
    if (life <= 0) {
		g_game->RequestState(std::make_unique<GameoverState>());
		return;
    }
	// Win
	if (m_aliveBricks <= 0) {
		g_game->RequestState(std::make_unique<YouWinState>());
		return;
	}
    
    // Update Animation Sprite
    m_ball.UpdateAnimation(dt);
    soundManager.Update();

    const float screenW = g_game->GetScreenWidth(), screenH = g_game->GetScreenHeight();

    // --- 1) Paddle input (kinematic) ---
    const float paddleSpeed = 800.f;
    if (inputManager.IsKeyDown(DIK_LEFT))  m_singlePaddle.position.x -= paddleSpeed * dt;
    if (inputManager.IsKeyDown(DIK_RIGHT)) m_singlePaddle.position.x += paddleSpeed * dt;
	if (inputManager.IsKeyDown(DIK_R)) g_game->RestartCurrentLevel();

    // Clamp paddle
    if (m_singlePaddle.position.x < m_paddleHalf.x) m_singlePaddle.position.x = m_paddleHalf.x;
    if (m_singlePaddle.position.x > screenW - m_paddleHalf.x) m_singlePaddle.position.x = screenW - m_paddleHalf.x;

    // Keep paddle body "kinematic"
    m_paddleBody.velocity = D3DXVECTOR3(0, 0, 0);
    m_paddleBody.acceleration = D3DXVECTOR3(0, 0, 0);
    m_paddleBody.force = D3DXVECTOR3(0, 0, 0);

    // --- 2) Ball physics integration (force -> accel -> vel -> pos) ---
    // (Apply forces here if you want, e.g., gravity: m_ballBody.ApplyForce({0, 980.f * m_ballBody.mass, 0});)
    m_ballBody.UpdatePhysics(dt);
    m_ball.position += m_ballBody.velocity * dt;

    // --- 3) Wall bounce using AABB (center vs half-size) ---
    const float eps = 0.5f; // small push-away

    // LEFT wall
    if (m_ball.position.x < m_ballHalf.x) {
        m_ball.position.x = m_ballHalf.x + eps;
        if (m_ballBody.velocity.x < 0.0f) m_ballBody.velocity.x = -m_ballBody.velocity.x;
    }
    // RIGHT wall
    if (m_ball.position.x > screenW - m_ballHalf.x) {
        m_ball.position.x = (screenW - m_ballHalf.x) - eps;
        if (m_ballBody.velocity.x > 0.0f) m_ballBody.velocity.x = -m_ballBody.velocity.x;
    }
    // TOP wall
    if (m_ball.position.y < m_ballHalf.y) {
        m_ball.position.y = m_ballHalf.y + eps;
        if (m_ballBody.velocity.y < 0.0f) m_ballBody.velocity.y = -m_ballBody.velocity.y;
    }
    // BOTTOM wall
    if (m_ball.position.y > screenH - m_ballHalf.y) {
        //m_ball.position.y = (screenH - m_ballHalf.y) - eps;
        //if (m_ballBody.velocity.y > 0.0f) m_ballBody.velocity.y = -m_ballBody.velocity.y;
        // move the ball back to original location and minus 1 life
         m_ball.position = { 500.0f, 450.0f, 0.f };
         m_ballBody.velocity = BALL_INITIAL_VELOCITY;

         soundManager.Play("damage");
         life--;
    }

    // --- 4) Ball vs Paddle AABB resolve (moving body vs static solid) ---
    physicsManager.ResolveAABB(
        m_ball, m_ballBody, m_ballHalf,
        m_singlePaddle, m_paddleHalf,
        0.9f // restitution
    );

    // --- 5) Ball vs Bricks using swept AABB to prevent tunneling ---
	{
		float remaining = dt;
		const int maxHits = 3; // avoid infinite loops if trapped
		int hits = 0;

		while (remaining > 0.0f && hits < maxHits) {
			// Displacement for this sub-step
			D3DXVECTOR3 disp = m_ballBody.velocity * remaining;

			float bestToi = 1.1f; // >1 means "no hit"
			int   hitIndex = -1;
			D3DXVECTOR3 hitNormal(0,0,0);

			// Find earliest brick hit in this sub-step
			for (int i = 0; i < m_brickCount; ++i) {
				auto& brick = m_bricksList[i];
				if (!brick.visible) continue;
				const D3DXVECTOR2 brickHalf(brick.scale.x * 0.5f, brick.scale.y * 0.5f);

				float toi; D3DXVECTOR3 n;
				if (physicsManager.SweepAABB(m_ball, m_ballHalf, disp, brick, brickHalf, toi, n)) {
					if (toi >= 0.0f && toi <= 1.0f && toi < bestToi) {
						bestToi = toi;
						hitIndex = i;
						hitNormal = n;
					}
				}
			}

			if (hitIndex < 0) {
				// No brick hit in this sub-step: advance fully and finish
				m_ball.position += disp;
				break;
			}
            if (hitIndex >= 0) {
				// Move to contact (existing) ...
				const float eps = 0.001f;
				m_ball.position += disp * (bestToi - eps);

				// Reflect red ball
				if (hitNormal.x != 0.0f) m_ballBody.velocity.x = -m_ballBody.velocity.x;
				if (hitNormal.y != 0.0f) m_ballBody.velocity.y = -m_ballBody.velocity.y;

				// If this was a blue brick, spawn a blue ball at brick center
				if (m_isBlue[hitIndex]) {
					SpawnBlueBallAt(m_bricksList[hitIndex].position);
				}

				// Remove brick
				m_bricksList[hitIndex].visible = false;
				--m_aliveBricks;
				soundManager.Play("hit");

				// continue remaining time (existing)
				const float used = (bestToi > 0.0f) ? bestToi : 0.0f;
				remaining *= (1.0f - used);
				++hits;
				continue;
			}
		}
	}

    // Integrate blue balls
	for (auto& bb : m_blueBalls) {
		bb.body.UpdatePhysics(dt);
		bb.sprite.position += bb.body.velocity * dt;

		// Walls (same as red, but bottom removes ball instead of life--)
		if (bb.sprite.position.x < bb.half.x) {
			bb.sprite.position.x = bb.half.x + eps;
			if (bb.body.velocity.x < 0) bb.body.velocity.x = -bb.body.velocity.x;
		}
		if (bb.sprite.position.x > screenW - bb.half.x) {
			bb.sprite.position.x = (screenW - bb.half.x) - eps;
			if (bb.body.velocity.x > 0) bb.body.velocity.x = -bb.body.velocity.x;
		}
		if (bb.sprite.position.y < bb.half.y) {
			bb.sprite.position.y = bb.half.y + eps;
			if (bb.body.velocity.y < 0) bb.body.velocity.y = -bb.body.velocity.y;
		}
		// Bottom: blue ball disappears, no life penalty
		// (mark with NaN Y; we'll erase after loop)
		if (bb.sprite.position.y > screenH - bb.half.y) {
			bb.sprite.position.y = std::numeric_limits<float>::quiet_NaN();
		}

		// Paddle collide (reuse your AABB solver)
		physicsManager.ResolveAABB(
			bb.sprite, bb.body, bb.half,
			m_singlePaddle, m_paddleHalf,
			0.9f
		);
	}

	// Erase dead blue balls (NaN Y mark)
	m_blueBalls.erase(
		std::remove_if(m_blueBalls.begin(), m_blueBalls.end(),
			[](const BlueBall& b){ return !_finite(b.sprite.position.y); }),
		m_blueBalls.end()
	);

	// Blue ball vs bricks (simple overlap or swept, here: overlap to keep it short)
	for (auto& bb : m_blueBalls) {
		float remainingBB = dt;
		const int maxBBHits = 3;
		int bbHits = 0;
		while (remainingBB > 0.0f && bbHits < maxBBHits) {
			D3DXVECTOR3 disp = bb.body.velocity * remainingBB;

			float bestToi = 1.1f; int hitIdx = -1; D3DXVECTOR3 n(0,0,0);

			for (int i = 0; i < m_brickCount; ++i) {
				auto& brick = m_bricksList[i];
				if (!brick.visible) continue;
				const D3DXVECTOR2 brickHalf(brick.scale.x * 0.5f, brick.scale.y * 0.5f);

				float toi; D3DXVECTOR3 nn;
				if (physicsManager.SweepAABB(bb.sprite, bb.half, disp, brick, brickHalf, toi, nn)) {
					if (toi >= 0 && toi <= 1 && toi < bestToi) {
						bestToi = toi; hitIdx = i; n = nn;
					}
				}
			}

			if (hitIdx < 0) {
				bb.sprite.position += disp;
				break;
			}

			const float eps3 = 0.001f;
			bb.sprite.position += disp * (bestToi - eps3);

			if (n.x != 0) bb.body.velocity.x = -bb.body.velocity.x;
			if (n.y != 0) bb.body.velocity.y = -bb.body.velocity.y;

			if (m_isBlue[hitIdx]) {
				// chain spawn allowed? If yes:
				SpawnBlueBallAt(m_bricksList[hitIdx].position);
			}

			m_bricksList[hitIdx].visible = false;
			--m_aliveBricks;
			soundManager.Play("hit");

			const float used = (bestToi > 0.0f) ? bestToi : 0.0f;
			remainingBB *= (1.0f - used);
			++bbHits;
		}
	}


	// Ball to Ball collision
	// Red vs each blue
	for (auto& bb : m_blueBalls) {
		float rA = m_ballHalf.x;
		float rB = bb.half.x;
		physicsManager.SimpleResolveCircleCollision(
			m_ball.position, m_ballBody.velocity, rA, m_ballBody.mass,
			bb.sprite.position, bb.body.velocity, rB, bb.body.mass,
			0.9f // restitution
		);
	}

	// Blue vs blue
	for (size_t i = 0; i < m_blueBalls.size(); ++i) {
		for (size_t j = i + 1; j < m_blueBalls.size(); ++j) {
			auto& A = m_blueBalls[i];
			auto& B = m_blueBalls[j];
			physicsManager.SimpleResolveCircleCollision(
				A.sprite.position, A.body.velocity, A.half.x, A.body.mass,
				B.sprite.position, B.body.velocity, B.half.x, B.body.mass,
				0.9f
			);
		}
	}
}

void Level1::Render(Renderer& renderer) {
    // Render order, first draw at back, last draw at front
	renderer.DrawSprite(m_background);
	renderer.DrawSprite(m_singlePaddle);
    for (auto& brick : m_bricksList) renderer.DrawSprite(brick);
	for (auto& bb : m_blueBalls) renderer.DrawSprite(bb.sprite); // draw blue ball
    renderer.DrawSprite(m_ball);


    // Draw Hud Text (Life)
    std::wstringstream ss;
    ss << L"Lives: " << life;
    D3DCOLOR lifeColor = (life <= 1) ? D3DCOLOR_XRGB(255, 60, 60) : D3DCOLOR_XRGB(255, 255, 255);
    renderer.DrawTextString(ss.str(), 16, 12, lifeColor); // draw on top left 
}

void Level1::SpawnBlueBallAt(const D3DXVECTOR3& pos) {
    BlueBall bb{};

    // Visuals: reuse red ball sheet but tint blue (or point to a blue sheet)
    bb.sprite = m_ball; // copy sprite definition (same spritesheet & frames)
    bb.sprite.color = D3DCOLOR_XRGB(0, 100, 255); // blue tint
    bb.sprite.position = pos;
    bb.sprite.scale = { 0.17f, 0.17f, 1.f }; // a little smaller/lighter

    // Size (radius ≈ half.x)
    const float frameW = (1024.0f / bb.sprite.animationCols) * bb.sprite.scale.x;
    const float frameH = (1024.0f / bb.sprite.animationRows) * bb.sprite.scale.y;
    bb.half = D3DXVECTOR2(frameW * 0.5f, frameH * 0.5f);

    // Physics: lighter & faster than red
    bb.body.mass = 0.2f; // lighter
    // random initial direction upward
    D3DXVECTOR3 dir(
        (std::rand() / (float)RAND_MAX) * 2.f - 1.f,  // -1..1
        -1.0f,                                        // up
        0.0f
    );
    // normalize dir.xy
    float len = (std::max)(0.001f, sqrtf(dir.x * dir.x + dir.y * dir.y));
    dir.x /= len; dir.y /= len;

    const float speed = 220.f; // faster than red (red ~150)
    bb.body.velocity = dir * speed;
    bb.body.force = D3DXVECTOR3(0, 0, 0);

    m_blueBalls.push_back(std::move(bb));
}


