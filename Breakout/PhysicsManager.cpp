#include "PhysicsManager.h"
#include <algorithm>
#include <cmath>

// Simple circle collision (very small, easy to understand)
void PhysicsManager::SimpleResolveCircleCollision(SpriteInstance& A_spr, PhysicsBody& A_body,
                                  SpriteInstance& B_spr, PhysicsBody& B_body,
                                  float restitution) // 0..1, 1 = perfectly elastic
{
    // centers (assumes sprite.position is the center)
    float ax = A_spr.position.x;
    float ay = A_spr.position.y;
    float bx = B_spr.position.x;
    float by = B_spr.position.y;

    float dx = bx - ax;
    float dy = by - ay;
    float dist2 = dx*dx + dy*dy;
    float rSum = A_body.radius + B_body.radius;
    if (dist2 >= rSum * rSum) return; // no collision

    float dist = sqrtf(dist2);
    // avoid division by zero
    D3DXVECTOR3 normal;
    if (dist > 1e-6f) {
        normal = D3DXVECTOR3(dx / dist, dy / dist, 0.0f); // this get unit vector of normal
    } else {
        // perfectly overlapping; choose arbitrary normal
        normal = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
        dist = 0.0f;
    }

    // penetration and simple positional correction (split by mass)
    float penetration = rSum - dist;
    float invMassA = (A_body.mass > 0.0f) ? 1.0f / A_body.mass : 0.0f;
    float invMassB = (B_body.mass > 0.0f) ? 1.0f / B_body.mass : 0.0f;
    float invSum = invMassA + invMassB;
    if (invSum > 0.0f) {
        // move them out so they no longer overlap
        D3DXVECTOR3 correction = D3DXVECTOR3(normal.x * (penetration / invSum),
                                             normal.y * (penetration / invSum),
                                             0.0f);
        // heavier object moves less
        A_spr.position.x -= correction.x * invMassA;
        A_spr.position.y -= correction.y * invMassA;
        B_spr.position.x += correction.x * invMassB;
        B_spr.position.y += correction.y * invMassB;
    }

    // relative velocity
    D3DXVECTOR3 relVel = D3DXVECTOR3(B_body.velocity.x - A_body.velocity.x,
                                     B_body.velocity.y - A_body.velocity.y, 0.0f);
    // dot product to project vector onto the normal
    float velAlongNormal = relVel.x * normal.x + relVel.y * normal.y; // dot product between vector and normal,  v . n

    // if moving apart already, no impulse needed
    if (velAlongNormal > 0.0f) return;

    // compute impulse scalar (very simple)
    float e = restitution;
    float j = 0.0f;
    if (invSum > 0.0f) {
        j = -(1.0f + e) * velAlongNormal / invSum;
    }

    // apply impulse
    D3DXVECTOR3 impulse = D3DXVECTOR3(normal.x * j, normal.y * j, 0.0f);
    if (A_body.mass > 0.0f) {
        A_body.velocity.x -= impulse.x * invMassA;
        A_body.velocity.y -= impulse.y * invMassA;
    }
    if (B_body.mass > 0.0f) {
        B_body.velocity.x += impulse.x * invMassB;
        B_body.velocity.y += impulse.y * invMassB;
    }
}

void PhysicsManager::ResolveAABB(SpriteInstance& moving, PhysicsBody& bodyA, const D3DXVECTOR2& halfA,
                                 const SpriteInstance& solid,  const D3DXVECTOR2& halfB,
                                 float restitution)
{
    // centers
    const float ax = moving.position.x;
    const float ay = moving.position.y;
    const float bx = solid.position.x;
    const float by = solid.position.y;

    // delta center
    const float dx = ax - bx;
    const float dy = ay - by;

    // overlap amounts (positive => intersecting)
    const float overlapX = (halfA.x + halfB.x) - fabsf(dx);
    const float overlapY = (halfA.y + halfB.y) - fabsf(dy);

    if (overlapX <= 0.0f || overlapY <= 0.0f) return; // no intersection

    // clamp restitution
    float e = restitution;
    if (e < 0.0f) e = 0.0f;
    if (e > 1.0f) e = 1.0f;

    // Resolve along least penetration axis
    if (overlapX < overlapY) {
        // push along X
        if (dx > 0.0f) moving.position.x += overlapX;
        else            moving.position.x -= overlapX;

        // reflect X velocity (moving vs static)
        bodyA.velocity.x = -bodyA.velocity.x * e;
    } else {
        // push along Y
        if (dy > 0.0f) moving.position.y += overlapY;
        else            moving.position.y -= overlapY;

        // reflect Y velocity
        bodyA.velocity.y = -bodyA.velocity.y * e;
    }
}

bool PhysicsManager::OverlapAABB(const SpriteInstance& A, const D3DXVECTOR2 halfA, const SpriteInstance& B, const D3DXVECTOR2 halfB) const
{
    const float dx = fabsf(A.position.x - B.position.x);
    const float dy = fabsf(A.position.y - B.position.y);
    return (dx <= (halfA.x + halfB.x)) && (dy <= (halfA.y - halfB.y));
}


bool PhysicsManager::SweepAABB(const SpriteInstance& A, const D3DXVECTOR2& halfA,
                               const D3DXVECTOR3& d, // displacement over this frame
                               const SpriteInstance& B, const D3DXVECTOR2& halfB,
                               float& toi, D3DXVECTOR3& n) const
{
    // Build edge coords from centers
    const float leftA   = A.position.x - halfA.x;
    const float rightA  = A.position.x + halfA.x;
    const float topA    = A.position.y - halfA.y;
    const float bottomA = A.position.y + halfA.y;

    const float leftB   = B.position.x - halfB.x;
    const float rightB  = B.position.x + halfB.x;
    const float topB    = B.position.y - halfB.y;
    const float bottomB = B.position.y + halfB.y;

    // Early-out if already overlapping at t=0 (treat as no-sweep hit)
    const bool overlapNow =
        (rightA  > leftB)  && (leftA   < rightB) &&
        (bottomA > topB)   && (topA    < bottomB);
    if (overlapNow) { toi = 0.0f; n = D3DXVECTOR3(0,0,0); return true; }

    // For zero movement, no swept hit
    const float dx = d.x, dy = d.y;
    if (dx == 0.0f && dy == 0.0f) return false;

    // Compute entry/exit distances along each axis
    float xEntryDist, xExitDist;
    if (dx > 0.0f) {
        xEntryDist = leftB  - rightA;
        xExitDist  = rightB - leftA;
    } else {
        xEntryDist = rightB - leftA;
        xExitDist  = leftB  - rightA;
    }

    float yEntryDist, yExitDist;
    if (dy > 0.0f) {
        yEntryDist = topB    - bottomA;
        yExitDist  = bottomB - topA;
    } else {
        yEntryDist = bottomB - topA;
        yExitDist  = topB    - bottomA;
    }

    // Convert to times in [0..1] (normalize by displacement)
    const float INF = 1e30f;
    float xEntry = (dx == 0.0f) ? -INF : (xEntryDist / dx);
    float xExit  = (dx == 0.0f) ?  INF : (xExitDist  / dx);
    float yEntry = (dy == 0.0f) ? -INF : (yEntryDist / dy);
    float yExit  = (dy == 0.0f) ?  INF : (yExitDist  / dy);

    // Overall entry is max of per-axis entries; overall exit is min of exits
    float tEntry = (xEntry > yEntry) ? xEntry : yEntry;
    float tExit  = (xExit  < yExit ) ? xExit  : yExit;

    // Valid collision if intervals overlap, and entry is within the frame
    if (tEntry > tExit)            return false;
    if (tEntry < 0.0f || tEntry > 1.0f) return false;

    // Determine hit normal from axis of entry
    if (xEntry > yEntry) {
        n = D3DXVECTOR3((dx > 0.0f) ? -1.0f : 1.0f, 0.0f, 0.0f); // hit on X side
    } else {
        n = D3DXVECTOR3(0.0f, (dy > 0.0f) ? -1.0f : 1.0f, 0.0f); // hit on Y side
    }

    toi = tEntry;
    return true;
}


