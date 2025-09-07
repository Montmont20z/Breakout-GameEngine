#pragma once
#include "PhysicsBody.h"
#include "SpriteInstance.h"
class PhysicsManager
{
    void SimpleResolveCircleCollision(SpriteInstance& A_spr, PhysicsBody& A_body,
        SpriteInstance& B_spr, PhysicsBody& B_body,
        float restitution = 0.6f);
};

