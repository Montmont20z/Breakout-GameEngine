#include "PhysicsManager.h"

// TO REMOVE --------------------------------------------------------------------
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
// --------------------------------------------------------------------------
