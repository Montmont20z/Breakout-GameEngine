#include "PhysicsManager.h"
#include <algorithm>
#include <cmath>

// Simple circle collision (very small, easy to understand)
bool PhysicsManager::SimpleResolveCircleCollision(
    D3DXVECTOR3& pA, D3DXVECTOR3& vA, float rA, float mA,
    D3DXVECTOR3& pB, D3DXVECTOR3& vB, float rB, float mB,
    float e
){
    D3DXVECTOR3 delta = pB - pA;
    float dist2 = delta.x*delta.x + delta.y*delta.y;
    float r = rA + rB;

    if (dist2 >= r*r) return false;

    float dist = (std::max)(0.0001f, sqrtf(dist2));
    D3DXVECTOR3 n = delta / dist;           // collision normal from A->B
    float penetration = r - dist;

    // Positional correction (prevent sinking)
    const float k_slop = 0.001f;
    const float percent = 0.8f; // 80% correction
    D3DXVECTOR3 correction = n * (percent * (penetration - k_slop));
    float invA = (mA > 0.f) ? 1.0f / mA : 0.0f;
    float invB = (mB > 0.f) ? 1.0f / mB : 0.0f;
    float invSum = invA + invB;
    if (invSum > 0.f) {
        pA -= correction * (invA / invSum);
        pB += correction * (invB / invSum);
    } else {
        // both infinite mass? push apart evenly
        pA -= correction * 0.5f;
        pB += correction * 0.5f;
    }

    // Relative velocity along normal
    D3DXVECTOR3 rv = vB - vA;
    float velAlongNormal = rv.x*n.x + rv.y*n.y;
    if (velAlongNormal > 0.f) return true; // moving apart after correction

    // Compute impulse scalar
    float j = -(1.f + e) * velAlongNormal;
    float denom = invA + invB;
    if (denom <= 0.f) return true;
    j /= denom;

    D3DXVECTOR3 impulse = n * j;
    vA -= impulse * invA;
    vB += impulse * invB;

    return true;
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
    return (dx <= (halfA.x + halfB.x)) && (dy <= (halfA.y + halfB.y));
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
    if (overlapNow) {
         // Pick normal from least penetration axis
        const float dxL = rightA - leftB;   // A penetrates B from left
        const float dxR = rightB - leftA;   // A penetrates B from right
        const float dyT = bottomA - topB;   // A penetrates B from top
        const float dyB = bottomB - topA;   // A penetrates B from bottom

        // Smallest positive overlap wins:
        float minPen = 1e30f;
        D3DXVECTOR3 bestN(0,0,0);
        if (dxL > 0 && dxL < minPen) { minPen = dxL; bestN = D3DXVECTOR3(-1, 0, 0); }
        if (dxR > 0 && dxR < minPen) { minPen = dxR; bestN = D3DXVECTOR3( 1, 0, 0); }
        if (dyT > 0 && dyT < minPen) { minPen = dyT; bestN = D3DXVECTOR3( 0,-1, 0); }
        if (dyB > 0 && dyB < minPen) { minPen = dyB; bestN = D3DXVECTOR3( 0, 1, 0); }

        n = bestN;
        toi = 0.0f;
        return true;
    }

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


