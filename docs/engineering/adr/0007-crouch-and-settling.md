# Crouching and condition-based corpse commitment

Accepted 2 November 2026. DEC-23 / REQ-027. Playtesting found missing crouch and premature corpse lock. Keep the camera duration independent of physics settlement. Replace elapsed-age commitment with supported collapse plus a sustained low-motion interval; a fixed timeout cannot establish a valid final pose.

Use CharacterMovement's crouch/uncrouch clearance and a stance-aware turret torso target. Both active ragdoll shapes and committed proxies participate in the first-hit visibility/shot query. A shared ballistic adapter makes intercepted shots and tracer endpoints testable without duplicating the damage rule. Older fixed bodies remain immutable.

The folded native rig initially injected energy and failed the integration settle/persistence check. Keep swing cones below 90 degrees and twist free in this proxy rig. Ignore self/mutual PhysicsBody contacts during the cosmetic fall, while blocking world, older fixed bodies and shot queries. Concurrent falling rigs can overlap; committed WorldDynamic proxies alone own stable support. Revisit filtering before adding other PhysicsBody props. Final skeletal art and anatomical joint limits remain future production work.
