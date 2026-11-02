# Meat Shield technical design

The stack uses Unreal Engine 5 with C++ for state, construction, persistence, and measurement, and Blueprints or data assets where useful for encounter content and presentation. The initial project pins 5.8.4 and contains the [turret approach slice](../design/first-playable.md). Full construction and persistence designs below remain implementation contracts for later work; the slice does not establish their acceptance.

## System ownership

| System | Responsibility | Must not own |
| --- | --- | --- |
| Siege coordinator | Run lifecycle, active clock, transaction ordering, checkpoints, victory | Cosmetic effects |
| Player controller and pawn | Input, movement, camera, health; pawn replacement | Persistent world data |
| Persistent weapon state | Magazine, reserve, reload remaining, fire cadence | Per-life pawn lifetime |
| Construction subsystem | Placement validation, stable proxies, tile quotas, support graph | Random visual ragdoll results |
| Corpse presentation pool | Impact, frozen visuals, LOD and cosmetic pooling | Gameplay collision authority |
| Threat controller | Detection, telegraph, fire, suppression, permanent disable | Global reload on player death |
| Objective controller | Capture progress and transaction request | Partial application of gate or spawn state |
| Save service | Versioned immutable snapshots, async persistence, backup recovery | Direct serialization of actor pointers |
| Measurement service | Local events, build metadata, performance and playtest exports | Gameplay decisions |

Proposed boundaries map naturally to engine subsystems, components, and data assets; class names and inheritance remain implementation details. Keep authoritative run state outside the transient pawn. A corpse presentation actor may be pooled only after its persistent construction record and collision are owned elsewhere.

## Frame ordering and idempotency

Each life, body, objective, checkpoint, and run receives a stable identifier. A death request carries its life ID; duplicate hits for the same life cannot produce additional bodies or pawns.

Process an update in this order:

1. Advance simulation time and collect damage and interaction requests using that update's start state.
2. Resolve damage, cover occlusion, and lethal states. Lethal damage cancels unfinished capture requests from that pawn in the same update.
3. Commit completed interactions only for surviving pawns; atomically update objective, threats, gate, and checkpoint.
4. For each newly lethal life, compute construction, commit its record and collision, increment deployment count, and replace the pawn at the current captured spawn.
5. Publish committed events, refresh UI, and schedule a save snapshot from the consistent end state.

A previous-update capture remains valid if the player dies now. Victory is terminal and emits once. Apply explicit event precedence in automated scenarios; do not rely on incidental actor tick order or asynchronous callbacks for gameplay decisions.

## Placement algorithm proposal

The mechanical result is a stable authored proxy chosen from a small pose library, initially prone and compact wall-collapse. A proxy uses at most two simple convex shapes. Visual impact may briefly simulate, but it does not determine the saved or walkable result. Unreal distinguishes simple and complex collision and restricts simulation with complex-as-simple geometry. [Epic collision reference](https://dev.epicgames.com/documentation/en-us/unreal-engine/simple-versus-complex-collision-in-unreal-engine).

```text
on lethal damage for an unconsumed life:
  sample final pawn transform, velocity, stance, and movement action
  sweep a bounded path from the sample toward the movement direction
  find the nearest valid supported anchor in covered construction tiles
  choose a pose and clamp its orientation against world geometry
  reject proxies intersecting protected zones, walls, or unsupported spans
  check per-tile primitive capacity and maximum construction height
  if valid, commit immutable proxy records and collision in this update
  otherwise emit a nonconstructive impact with a reason code
  replace pawn; cosmetic settling can finish independently
```

The initial sweep limit is 1 m, maximum cosmetic settling time 350 ms, and repeated-placement positional tolerance 0.15 m. Use meters in design data and convert once at the engine boundary. A candidate that crosses a wall is invalid even if its endpoint is supported. Never use a nearest-free-point search that silently teleports the body far from the death.

Support is a reference to immutable world geometry or earlier construction records. New placement requires support under each proxy's configured footprint samples, with no gap exceeding the tuned tolerance. Construction is additive: baseline geometry has no removal or displacement. Track dependencies for QA and snapshot validation even when there is no runtime destruction.

If no candidate is valid, do not create invisible cover. Record `out_of_bounds`, `no_support`, `protected_zone`, `occupied`, `height_limit`, or `tile_full`. Level review must establish that intended deaths rarely fail placement; otherwise the premise becomes unpredictable.

## Bounded geometry and memory

Use authored construction tiles covering reachable assault ground. The starting budget is 128 tiles per sector, eight simple primitives per tile, and at most 1,024 construction primitives per sector. A body can consume slots in more than one tile; count every intersected tile before committing, reserve atomically, and reject the entire placement if any tile would overflow. The initial height limit is 2 m above each tile's authored base. Final volume dimensions and budgets are verified at M1 and M2.

These quotas are permanent construction capacity, not a rolling corpse deletion threshold. Saturating one tile cannot consume another tile's reserved capacity. Saturated placements add no geometry, support, damage resistance, or height. Display only a short effect that conforms to existing material so a decorative body cannot promise new support. Levels must offer a viable route within every tile's quota.

Keep at most four cosmetic ragdolls active globally, returning the oldest to a frozen or batched presentation when another arrives. Keep at most 128 individually rendered corpse instances per sector before batching or simplifying visuals. Retain all committed gameplay proxies and their records up to the bounded tile budgets. Batching may reduce rendering overhead; it is not assumed to reduce physics shape count. No approximation or merge may change support or bullet occlusion without the regression fixtures passing.

The three-sector release is bounded to 3,072 construction primitives plus ordinary level collision. The nominal visual and physics budgets are hypotheses; this count is not a performance guarantee. Unlimited lives are supported by finite local capacity and pooled cosmetics, not an unlimited stream of persistent actors.

## Collision and traversal contract

| Pair or query | Baseline response |
| --- | --- |
| Player capsule against stable construction | Block and support movement |
| Weapon or emplacement ray against stable construction | Block at first hit |
| Threat visibility ray against stable construction | Occlude |
| New construction against stable construction | Support test; reject penetrating placement |
| Cosmetic ragdoll against player | Ignore to avoid launches and snagging |
| Cosmetic debris against gameplay queries | Ignore |
| Capture trigger against player | Overlap; stable bodies do not capture |
| Spawn safety zone against construction candidate | Reject placement |

Use the same authoritative proxy set for traversal, bullet occlusion, and threat sight, with explicit channel responses. A visible gap is not a shield unless a proxy occupies it. Test crouched eye, muzzle, and capsule positions separately. Sweep vault destinations and reject clearance failures rather than moving the player through a ceiling.

## Threat and navigation scope

Fixed emplacements query the player through construction-aware sight rays. Their state machine and timers live across pawn replacement. Death does not rewind fire cadence or suppression. Controls disable the related threat IDs permanently. No runtime navigation mesh rebuild is required by this baseline.

If moving enemies are later approved, prototype their navigation separately. Unreal offers tiles, modifiers, and navigation links, but that alone does not prove arbitrary corpse piles are cheaply walkable by AI. [Epic navigation reference](https://dev.epicgames.com/documentation/en-us/unreal-engine/overview-of-how-to-modify-the-navigation-mesh-in-unreal-engine).

## Saving and recovery

Maintain separate settings, local record, and active-run slots. An active-run snapshot stores stable IDs and transforms, shape and tile occupancy, support references, objective and checkpoint states, relative threat timers, weapon resources and reload remaining, active time, deployments, classification, and content/rules versions. Do not save transient actor pointers, live ragdoll state, wall-clock deadlines, or the pawn's airborne position. See [data contracts](data-contracts.md).

Snapshot at checkpoint capture, explicit save and quit, and at most once per five seconds while dirty. On quit, show completion or failure before closing. If the game exits abruptly, up to the last interval of dirty progress can be lost; the checkpoint snapshot remains the durable milestone. Async writes avoid blocking the respawn path; Epic documents custom SaveGame objects and asynchronous saving. [Epic save reference](https://dev.epicgames.com/documentation/en-us/unreal-engine/saving-and-loading-your-game-in-unreal-engine).

Copy state on the game thread at a committed frame boundary. Serialize from an immutable copy. Coalesce dirty requests and allow one writer per run slot. Use two alternating versioned slots with monotonically increasing generation and checksums; mark a generation committed only after successful write and verification. On load, pick the newest valid complete generation, falling back to the previous one. A failure never replaces the last valid snapshot. This is a project recovery protocol to implement and test, not a guarantee supplied by a single SaveGame call.

Restore geometry and objectives before enabling threats or spawning the pawn. Validate IDs, supported poses, finite transforms, bounds, tile totals, and acyclic support references. Restore relative timer durations against the resumed simulation clock. Migrate supported older schema versions explicitly; unknown newer versions offer a safe error and fresh run without overwriting the original. All resumed runs retain a resumed classification. Sector reset restores its entry snapshot and marks assisted without subtracting already-spent time or deployments.

## Performance and observability

Nominate the benchmark CPU, GPU, memory, storage, drivers, resolution, quality preset, and build configuration at M0. Proposed M2 targets are p95 frame time at or below 16.7 ms at 1080p, p99 at or below 25 ms in a scripted 10-minute stress run, and p95 respawn at or below 150 ms. Profile CPU and GPU separately; these budgets are targets, not measured results.

Run stress cases at empty, half-full, and full construction budgets, with bursts and saves active. Include a 1,000-death soak. Memory must plateau after visual pools and construction tiles fill; compare the final two 10-minute windows with a 5% growth threshold for investigation. Profile allocation churn, collision query count, active ragdolls, draw submissions, save duration, and time-to-control. Keep raw traces tied to the build and benchmark manifest.

If targets fail, reduce cosmetic count and cost, improve static collision ownership, or reduce sector construction coverage with a designer-validated route. Do not silently delete useful cover. Rendering feature choices and minimum published specifications follow measured evidence at M4.
