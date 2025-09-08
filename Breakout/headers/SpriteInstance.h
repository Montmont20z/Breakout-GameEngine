#pragma once
#include <string>
#include <d3dx9.h>
#include <atomic>

struct SpriteInstance {
	int textureHandle = 0;

	std::string texturePath;
	
	// Variable for transformation
	D3DXVECTOR3 position = D3DXVECTOR3(0,0,0);
	D3DXVECTOR3 scale = D3DXVECTOR3(1, 1, 1);
	float rotation = 0.0f;
	D3DCOLOR color = D3DCOLOR_XRGB(255,255,255);
	int renderOrder = 0; // Lower values render first (background), higher value render last (foreground)

	// animation layout
	int animationRows = 1; // states
	int animationCols = 1; 
	int framesPerState = 1;    // convenience: usually equals animationCols

	// runtime animation state
	int state = 0, currentFrame = 0;
	float frameDuration = 0.1f; // seconds per frame
	float elapsedSinceFrame = 0.0f; // accumulated seconds
	bool looping = true;
	bool playing = false;
	bool visible = true;

private:
	int m_textureId = 0; // unique ID to allow sprite to easily used by other class
	static inline std::atomic<int> s_nextUniqueId{ 1 };

public:


	 SpriteInstance()
		 : m_textureId(s_nextUniqueId.fetch_add(1, std::memory_order_relaxed)) {}

	 SpriteInstance CloneWithNewId() const {
		 SpriteInstance c = *this;
		 c.m_textureId = s_nextUniqueId.fetch_add(1, std::memory_order_relaxed);
		 return c;
	 }

	 int id() const { return m_textureId; }
	
	SpriteInstance(const std::string& path, const D3DXVECTOR3& pos,
		int order = 0,
		int animationRows_ = 1, int animationCols_ = 1,
		int framesPerState_ = 0, // if 0, default to animationCols_
		float frameDuration_ = 0.1f, bool looping_ = true, bool playing_ = false)
		: m_textureId(s_nextUniqueId.fetch_add(1, std::memory_order_relaxed))
		, texturePath(path)
		, position(pos)
		, renderOrder(order)
		, animationRows(animationRows_)
		, animationCols(animationCols_)
		, frameDuration(frameDuration_)
		, looping(looping_)
		, playing(playing_)
	{
		if (animationRows < 1) animationRows = 1;
		if (animationCols < 1) animationCols = 1;

		if (framesPerState_ > 0) framesPerState = framesPerState_;
		else framesPerState = animationCols;

		// sanity clamp
		if (framesPerState > animationCols) framesPerState = animationCols;
		currentFrame = 0;
		state = 0;
	}

	 RECT GetSourceRect(int textureWidth, int textureHeight) const {
        RECT rect = {0,0,textureWidth,textureHeight};
		if (animationCols <= 0 || animationRows <= 0 || framesPerState <= 0) {
			return rect;
		}
		// Size of one cell/frame
        int frameW = textureWidth / animationCols;
        int frameH = textureHeight / animationRows;

		// clamp state and frame
		int row = state;
		if (row < 0) row = 0;
		if (row >= animationRows) row = animationRows - 1;

		int col = currentFrame % framesPerState;
		if (col < 0) col = 0;
		if (col >= animationCols) col = animationCols - 1;

        rect.left = col * frameW;
        rect.top = row * frameH;
        rect.right = rect.left + frameW;
        rect.bottom = rect.top + frameH;
        return rect;
    }

	// Change animation state (row). newState is 0-based (0..animationRows-1)
	void SetState(int newState, bool resetFrame = true) {
		if (newState < 0) newState = 0;
		if (newState >= animationRows) newState = animationRows - 1;
		state = newState;
		if (resetFrame) currentFrame = 0;
		elapsedSinceFrame = 0.0f;
	}

	void UpdateAnimation(float dt) {
		if (!visible) return;
		if (framesPerState <= 1) return;   // no animation
		if (!playing) return;

		elapsedSinceFrame += dt;
		while (elapsedSinceFrame >= frameDuration && frameDuration > 0.0f) {
			elapsedSinceFrame -= frameDuration;
			++currentFrame;
			if (currentFrame >= framesPerState) {
				if (looping) {
					currentFrame = 0;
				}
				else {
					currentFrame = framesPerState - 1;
					playing = false; // stop at last frame
					break;
				}
			}
		}
	}
};
