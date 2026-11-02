# Persistent ragdolls and a return camera

Accepted by the product owner's 2 November 2026 direction. DEC-22 / REQ-026. The previous instant reset demonstration omitted the signature persistent body. The owner now requests a visible falling ragdoll, a camera pull-back/return, and retention of all old deaths.

Use procedural articulated Chaos primitive bodies in this art-light slice. Freeze the sampled pose before first-person control returns, with separate stable collision authority and inert visible geometry. Keep every corpse across lives/retries. Death timing is a deterministic engine-independent state machine; the Unreal camera, physics and input adapters consume it. No per-life world reset or corpse eviction is permitted.

The freeze-before-control timing above is superseded by [ADR 0007](0007-crouch-and-settling.md): wait for supported collapse and sustained quiet, even after the camera has returned. Active meat also blocks shots.

The initial 1.05 s presentation supersedes the earlier 150 ms death-to-control goal, 350 ms settling hypothesis and immediate authored pose proposal for this slice. Full skeletal art, save restoration, constrained placement and large-count performance acceptance remain open. Update their future designs from measured results; do not claim the old two-shape budget while generating an articulated proxy per body.
