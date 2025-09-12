#pragma once
#include "GameServices.h"
class InputManager; class PhysicsManager; class SoundManager; class Renderer;

class IGameState {
public:
	virtual ~IGameState() = default;
	virtual bool OnEnter(const GameServices& services) { (void)services; return true; }
	virtual void OnExit(const GameServices& services) {}
	virtual void Update(float dt,
						InputManager& inputManager,
						PhysicsManager& physicsManager,
						SoundManager& soundManager) = 0;
	virtual void Render(Renderer& renderer) = 0;


};
