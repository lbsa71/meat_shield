# Crouch, meat cover and settling

Accepted 2 November 2026 from the product owner's playtest. REQ-001, REQ-006, REQ-026, REQ-027 / TC-031. Supersedes the fixed 0.75 s ragdoll lock assumption.

Hold Left Ctrl or C to crouch; release to stand when overhead clearance permits. Use Unreal's native crouch capsule/clearance handling, a 1 m crouched height, lower eye height and slower movement. Sprint cannot override crouch speed. The turret targets a stance-aware torso point, and visible/mechanical collision must agree.

All meat blocks nonpenetrating shots at the first surface, during the fall and after commitment. Use the same first-hit ballistic adapter for gameplay and integration tests; tracer length and damage derive from that hit. Body cover works where its actual silhouette intersects the shot. Preserve older committed shapes throughout.

Replace the hard-age freeze with explicit settle rules: minimum 1.2 s simulation, supported collapsed torso, and at least 0.35 s of low linear/angular motion across the body. Any renewed motion, lost support or noncollapsed posture resets the quiet interval. Invalid/nonfinite inputs cannot advance it. Never freeze a still-moving body merely because a maximum timer elapsed. The 1.05 s camera/control return remains independent, so a corpse can continue falling after the next life begins.

The procedural rig must be free to topple and fold; give it a bounded physical knockdown impulse rather than an animated pose snap. Fit final fixed primitives to the settled pose. Tests cover early-age and interrupted settling, a moving/suspended/upright body, native input and ceiling clearance, stance-aware targeting, actual first-hit interception by both moving and settled meat, unchanged health behind intercepted shots, and persistence after later deaths/retries. Inspect a rendered settled-body capture, not just a mid-fall screenshot.

Active rigs use PhysicsBody and ignore mutual/self PhysicsBody contacts, while retaining world/older fixed-body contacts and Visibility blocking. Only settled WorldDynamic proxies become support authority. This prevents folded limbs fighting their own constraints; two simultaneously falling cosmetic rigs may overlap. Their later fixed poses remain immutable. The prototype has no other PhysicsBody props; revisit this filter when introducing them.

The next playtest refinement adds location-based bullet impulse to active bodies and transfers the lethal hit into the new rig. An off-centre torso impact must create lateral/twist rotation instead of a uniform backward fall. Fixed bodies remain immutable. A seeded field of at least 40 pre-downed bodies uses authored prone poses with fixed collision, avoids the safe start, concrete cover and central lane, and is built once per level. This is initial encounter content, separate from player deployment counts.

Playtest correction: a torque call alone is insufficient evidence. In the open-lane fall, the torso lateral axis must tilt at least 0.8 out of horizontal at the observation capture, proving pronounced sideways rotation rather than only backward pitch. The location-based impact implementation passes this stronger native fixture.
