#pragma once
class Renderer; class InputManager; class PhysicsManager; class SoundManager;

struct GameServices {
    Renderer&       renderer;
    InputManager&   inputManager;
    PhysicsManager& physicsManager;
    SoundManager&   soundManager;
};

