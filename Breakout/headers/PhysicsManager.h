#pragma once
#include "PhysicsBody.h"
#include "SpriteInstance.h"
class PhysicsManager
{
public:
    bool SimpleResolveCircleCollision(
		D3DXVECTOR3& posA, D3DXVECTOR3& velA, float rA, float mA,
		D3DXVECTOR3& posB, D3DXVECTOR3& velB, float rB, float mB,
		float restitution = 0.1f
	);

    void ResolveAABB(SpriteInstance& moving,PhysicsBody& bodyA, const D3DXVECTOR2& halfA, const SpriteInstance& solid, const D3DXVECTOR2& halfB, float restitution = 0.9f);

    bool OverlapAABB(const SpriteInstance& A, const D3DXVECTOR2 halfA, const SpriteInstance& B, const D3DXVECTOR2 halfB) const;
    // + in public:
	bool SweepAABB(const SpriteInstance& moving, const D3DXVECTOR2& halfA,
               const D3DXVECTOR3& delta01, // displacement for this frame (v * dt)
               const SpriteInstance& solid, const D3DXVECTOR2& halfB,
               float& toi01,               // [0..1] earliest time of impact
               D3DXVECTOR3& hitNormal) const;

};

