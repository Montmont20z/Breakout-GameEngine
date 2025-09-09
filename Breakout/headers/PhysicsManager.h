#pragma once
#include "PhysicsBody.h"
#include "SpriteInstance.h"
class PhysicsManager
{
public:
    void SimpleResolveCircleCollision(SpriteInstance& A_spr, PhysicsBody& A_body,
        SpriteInstance& B_spr, PhysicsBody& B_body,
        float restitution = 0.6f);

    void ResolveAABB(SpriteInstance& moving,PhysicsBody& bodyA, const D3DXVECTOR2& halfA, const SpriteInstance& solid, const D3DXVECTOR2& halfB, float restitution = 0.9f);
};

