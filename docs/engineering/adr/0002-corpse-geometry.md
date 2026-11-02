# Corpse geometry proposal

Status: Proposed. Owners: Technical lead and designer. Decision gate: M1, representative performance recheck M2.

The [accepted persistent-ragdoll slice](0006-persistent-ragdolls.md) supersedes immediate authored pose placement for the current demo. This original proposal remains a future constrained-placement/performance option, not a description of the current ragdoll implementation.

## Context

Pure persistent ragdolls can vary under repeated impacts and require ongoing simulation. Players must trust a death placement, and unlimited attempts cannot create unlimited active simulation or draw cost.

## Proposed decision

Commit an authored simple collision proxy at death from a bounded movement-informed placement calculation. Let a short cosmetic ragdoll or impact effect settle toward its stable visual. Use local tile quotas and a construction height limit to bound geometry; pool or batch visuals independently. Existing useful collision remains for the whole siege.

The proposal deliberately separates the visual violence from terrain authority. Unreal's simple collision primitives support the intended proxy representation, but the feel, tolerances, and cost must be proven by this project's fixtures. [Epic collision documentation](https://dev.epicgames.com/documentation/en-us/unreal-engine/simple-versus-complex-collision-in-unreal-engine).

## Alternatives and consequences

Fully simulated bodies offer more emergent motion but risk unstable structures. Freezing arbitrary final ragdoll poses retains that variability and can leave complex shapes. Voxelized or merged piles can bound cost differently but introduce silhouette, support, and occlusion errors. Compare one representative alternative only if the proposed proxy fails placement or visual trust tests.

Acceptance requires repeated dive placements within 0.15 m tolerance, immediate reliable support, aligned visible cover, no unsupported construction, local saturation without disappearing cover, and stress traces at the planned shape budget. If that fails, revise the construction model before building additional sectors.
