# Engine selection proposal

Status: Proposed. Owner: Technical lead. Decision gate: M0.

## Context

The game needs photorealistic first-person presentation, responsive movement, stable construction collision, quick pawn replacement, and packaged PC builds. The team and its engine expertise are not yet known.

## Proposed decision

Use Unreal Engine 5, with an exact patch pinned after a clean-checkout build and a corpse placement fixture succeed. Keep state and construction systems in C++; expose encounter and presentation data to content authors. Rendering features are selected against measured budgets, not enabled by assumption.

This is a suitability proposal based on project needs and documented collision and persistence capabilities, not an engine benchmark. [Epic collision documentation](https://dev.epicgames.com/documentation/en-us/unreal-engine/simple-versus-complex-collision-in-unreal-engine), [Epic save documentation](https://dev.epicgames.com/documentation/en-us/unreal-engine/saving-and-loading-your-game-in-unreal-engine).

## Alternatives and consequences

A team-proven Unity, Godot, or custom stack may reduce onboarding cost; assess the same packaged fixture and content workflow before replacing the baseline. No cross-engine performance comparison has been performed. Unreal entails large binary content and a specific build toolchain, so binary locking and a reproducible Windows build are acceptance conditions.

Acceptance requires a named engine/toolchain manifest, packaged first-person scene, stable proxy that blocks player and fire, measured pawn replacement, binary round trip, and an updated production forecast. Record the accepted or superseded decision in the milestone review.
