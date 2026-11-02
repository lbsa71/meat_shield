# Persistent death-loop validation

REQ-002, REQ-003, REQ-026 / TC-030. Tested 2 November 2026 on the setup workstation with Unreal 5.8.4, changelist 58902052. This continues the [first playable](first-playable-validation.md) under [ADR 0006](../engineering/adr/0006-persistent-ragdolls.md).

## Evidence

| Check | Observed result |
| --- | --- |
| Core C++ rules | 44 checks passed; the new death timeline first failed 9 checks before implementation |
| Tooling tests | 32 Python tests passed; two new evidence cases first failed before the verifier enforced corpse/death-camera evidence |
| Missing-behavior integration red | Real engine fixture failed the camera and corpse-retention cases against the old instant-reset implementation |
| Visual regression red | Render review exposed the avatar capsule blocking the camera sweep; the added clearance test failed before the ignore rule was fixed |
| C++ builds and package | Editor/game compile/link and Windows BuildCookRun build/cook/stage/archive completed successfully |
| Rendered packaged scenario | 25 checks passed; two deaths, 14 real turret hits, two retained corpses, 28 actors including the diagnostic actor and one reused camera |
| Physics and authority | Articulated body movement/toppling observed, then 15 separate fixed collision proxies per corpse; older component transforms unchanged after the next death |
| Camera and controls | Independent death view, open-lane clearance over 2.2 m, first-person return within 1.2 s; duplicate hit and R during death do not restart or duplicate it; controls and Escape exit pass |
| Retention | Both visible corpse actors and their fixed proxies remain after later death and R; no age/count eviction implemented |
| Render/audio | Fresh 1600×900 fall, two-corpse and turret captures reviewed; master audio RMS about 0.029, peak 0.347 without clipping |

The camera fixture uncovered a real visual defect that passing persistence checks alone did not establish: the sweep began inside the dying avatar's capsule. It now explicitly ignores that avatar while retaining world collision checks. The return endpoint is applied before switching back to the player, avoiding a final-frame snap to the corpse.

Bodies are native Chaos primitive rigid bodies connected by joints. Visible meshes follow those bodies while falling. At 0.75 s the pose is frozen and independent hidden primitives own fixed movement/fire/sight collision. The camera sequence is 0.35 s pull-back, 0.35 s observation and 0.35 s return; the world is not paused. The old 150 ms/350 ms assumptions are superseded for this accepted slice.

Use [run/build instructions](../engineering/running-the-prototype.md). The sanitized build manifest will record the exact verified source and binary hash. Raw captures and logs stay in ignored local artifacts.

This is a procedural mannequin proxy and a loaded-session persistence implementation. No save/relaunch restoration, final skeletal art, supported placement solver, two-shape corpse budget, large-count performance acceptance or full-game milestone acceptance is claimed. A physical-device human playtest remains separate from the automated engine-key fixture.
