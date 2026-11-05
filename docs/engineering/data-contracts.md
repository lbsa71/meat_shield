# Meat Shield data contracts

The JSON files under [planning](../../planning/README.md) are reviewable contracts and examples, not files already loaded by a game. Developers should map tuning into engine data assets and validate it during content import. Changing a field requires updating its specification, fixtures, and version.

## Shared conventions

- Use meters, seconds, degrees, and integer ammunition in design data. Convert meters to the chosen engine's units at one boundary.
- Example transforms use a right-handed design frame: X forward, Y left, Z up. Rotation is yaw about Z in degrees. The Unreal adapter must explicitly map Y to Unreal's right axis and invert yaw as needed; do not copy transforms as raw engine values.
- Persistent IDs are stable strings scoped by run, sector, or content definition. Display labels are localized string IDs, never identity keys.
- Durations are nonnegative finite values. Timestamps in gameplay state are active simulation durations or remaining durations, never system-clock deadlines.
- Schema version describes serialized structure; rules version describes gameplay tuning; content version describes geometry and authored IDs. Save compatibility checks all three.

## Tuning contract

[tuning.example.json](../../planning/tuning.example.json) groups movement, construction, respawn, weapon, threat, timer, and performance targets. `status` is `prototype_hypothesis`. Each number is an initial value to measure, not an engine default. The validator checks relationships such as tile count times per-tile primitives equaling the sector cap, rather than validating tuning as fun or technically achievable.

Expose approved settings to designers through one authoritative data asset per rules version. Avoid duplicated numbers in Blueprint graphs, UI, and code. A changed rules version creates a distinct local-record category. Gameplay and presentation settings must be separate; reducing gore cannot change a collision dimension.

## Active run snapshot

[save.example.json](../../planning/save.example.json) demonstrates one committed construction body, a captured gate, a current checkpoint, relative threat timers, weapon state, and run classification. It is a human-readable fixture; the shipped binary format can differ if it preserves the contract.

| Field | Invariant |
| --- | --- |
| `schema_version` | Supported migration or explicit incompatible-save response |
| `generation` | Increases only for a new complete snapshot generation |
| `run_id`, versions, classification | Stable for a run; resumed and assisted classifications never revert to uninterrupted |
| `active_time_s`, `deployments` | Nonnegative; sector reset cannot decrease all-run values |
| `checkpoint_id`, `captured_objective_ids` | Refer to authored content in the saved content version |
| `sector_entry_snapshot_id` | Resolves to the current sector's reset baseline |
| `weapon` | Magazine and reserve in range; remaining reload time survives respawn |
| `threats` | Known IDs and states, remaining timers valid; disabled threats cannot fire |
| `construction` | Unique body IDs, supported pose, bounded finite transform, known tiles, and shape records |
| `supports` | World anchor IDs or earlier body IDs; no dangling or cyclic body references |
| `tile_occupancy` | Recomputed from shapes on load and compared to stored totals |

The checksum and final commit marker belong to the storage envelope, not the illustrative gameplay payload. Sector-entry snapshots use the same payload shape and are stored with the active run. Prune only generations no longer needed for active resume, previous-good recovery, or sector reset.

## Event contract

[telemetry.schema.json](../../planning/telemetry.schema.json) is a JSON Schema for local development and consented playtest event files. This handoff's validator checks its sample event and required references; it does not implement a general JSON Schema engine. A runtime implementation must validate all emitted event types against the schema.

Required envelope fields are schema version, event ID, event name, build ID, run ID, life ID or null, active timestamp, rules version, and payload. No account identifier or remote collection endpoint is required. Default output is a local JSON Lines file. Event IDs support deduplication; include the committed transaction ID where relevant.

| Event | Required payload | Purpose |
| --- | --- | --- |
| `life_started` | checkpoint ID, deployment index | Separate lives and verify fixed respawn |
| `player_died` | death reason, position, life duration | Review life rhythm and location |
| `body_committed` | body ID, tile IDs, pose ID, position, placement reason | Link a death to actual terrain |
| `body_rejected` | position, rejection reason | Detect invalid or saturated routes |
| `body_used` | body ID, use kind, life ID | Observe later traversal or intercepted fire; debounced per body and life |
| `threat_changed` | threat ID, previous state, next state, remaining duration | Verify continuous threats |
| `checkpoint_captured` | objective ID, checkpoint ID, transaction ID | Measure progress |
| `respawn_completed` | previous life ID, elapsed milliseconds | Measure the respawn budget |
| `run_finished` | completion time, deployments, classification | Compare matched rulesets |
| `save_result` | generation, success, elapsed milliseconds | Diagnose recovery and stalls |

`body_used` is supporting evidence, not a good-death score or proof of intent. A player can accidentally walk on a body, and an intercepted bullet need not have saved them. Combine events with observation and the participant's explanation.

## Planning file contract

Requirements contain an ID, statement, provenance, acceptance, source document, test IDs, and target gate. Tasks contain an ID, epic, role, priority, milestone, dependencies, requirement IDs, estimate range and likely days, deliverable, and acceptance list. Milestones contain sequence, relative week range, gates, and role capacity. Assets contain quantities, units, target gate, owner role, and acceptance. The generated backlog groups tasks by milestone and preserves these IDs.

IDs are durable across reordering. Dependencies form an acyclic graph and cannot point to a later milestone. Every release requirement has implementation work and a named test. Estimates are person-days, not elapsed days. A task may use several contributors through reviews, but its estimate and capacity charge belong to the listed owner role; cross-discipline production work should be separate tasks.

## Puzzle recipe 1 (REQ-030)

`game/Content/MeatShield/Data/puzzle-01.json` stores `version`, `seed`, and 18 ordered `bodies`. Each has an Unreal `FTransform` string for actor and body mesh world transforms, plus an ordered `bones` array of names and local `FTransform` strings. Units are centimetres, Unreal axes and quaternion-derived transform serialization. Slot IDs are 2^63 + index + 1. The physics bake is an authoring operation; the committed data is the deterministic gameplay input. Filesystem reads use Unreal project content paths and UFS staging. Invalid version/seed/count/transforms fail with an actionable error. A recipe or source skeleton change requires rebaking and replay/collision validation.

Runtime terrain and stance values are authoritative in the pure `PuzzleRules.h`; `tuning.example.json` documents these in metres. Terrain geometry is an exact triangle-grid sampling of that function, not an independent cosmetic surface. Gameplay collision and render geometry share vertices. New death bodies are not part of this initial recipe and are not saved across process exit yet.
