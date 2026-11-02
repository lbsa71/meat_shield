# Crouch, meat cover and settling validation

REQ-001, REQ-006, REQ-026, REQ-027 / TC-031; partial MS-005, MS-007 and MS-008. Tested 2 November 2026 on the setup workstation with Unreal 5.8.4 CL58902052. Source revision `aa9fcc065e1bbd3a4939ada66040c0e8efc76e82`, rules `prototype-0.3`, [ADR 0007](../engineering/adr/0007-crouch-and-settling.md).

| Check | Observed result |
| --- | --- |
| Meaningful rule red/green | Settle stub first failed 3 checks; the implemented rules pass all 60 native checks, including early age, renewed motion, lost support, upright pose, angular motion, invalid data, hitch and once-only commitment |
| Missing-behavior integration red | Native engine fixture failed 7 crouch, clearance, active-meat, commitment and damage-interception checks against the earlier game |
| Rig regression | Crouched rig initially gained energy and failed settle/persistence checks; corrected joint freedom and contact filtering allow it to collapse and settle |
| Evidence verifier red/green | Two new tooling tests first failed because the older 25-check build and missing close render were accepted; the verifier now rejects both; all 34 Python tests pass |
| Cold-start fixture regression | First clean rendered run failed movement/sprint because a long first tick skipped the input hold. Fixture scheduling now caps its step and one-shot capture requests cannot miss a narrow time window; gameplay and measured camera clocks still use real world time |
| Data and planning | Planning consistency, regenerated backlog, telemetry schema/examples and diff whitespace checks pass |
| Build | Editor/game compile/link and Windows BuildCookRun build/cook/stage/archive pass |
| Rendered package | 35 checks pass with two independent deaths, two retained corpses and 28 actors; native crouch/release and overhead clearance work |
| Bullet contract | Active meat blocks Visibility rays; fixed meat intercepts the production damage adapter without reducing player health; an uncovered ray still reduces health by 15 |
| Collapse and commitment | Standing and crouched deaths settle on the floor before fixed proxies are created; initial rendered run committed at 4.26 s and 4.61 s, torso center about 19 cm above floor and absolute vertical-axis component 0.10 |
| Clean source reproduction | Detached revision `aa9fcc0` passed 60 native checks, 34 tooling tests, planning validation, full editor/game build and packaging, then all 35 rendered checks; tracked source stayed clean. That exact verified binary was copied to the normal launch location and its SHA-256 compared |
| Continuity | Camera/control return remains within 1.2 s while physics continues; older body transforms remain unchanged across the next death and R |
| Render/audio | Fresh 1600×900 fall, two-body and close settled-body captures reviewed; both torsos lie on the ground; master output RMS 0.0279, peak 0.3468, without silence or clipping |

Hold Left Ctrl or C to crouch. Native clearance prevents standing through a ceiling. Falling and fixed body geometry blocks shots at the first actual surface; a single flat body does not cover the entire crouched player. Final fixed collision matches the settled pose, rather than a larger invisible shield.

The fall uses a procedural articulated mannequin. Active PhysicsBody rigs ignore themselves and one another, but collide with ground, authored cover and older fixed WorldDynamic proxies. Their shot queries still block. Supported collapse plus 0.35 s of low motion after at least 1.2 s permits commitment; there is no age-only maximum that locks a moving body.

The [build manifest](builds/crouch-and-settling-2026-11-02.json) identifies the delivered binary. Raw logs and PNG/WAV captures remain in ignored local artifacts, including `.local/crouch-clean` and the package's `MeatShield/Saved` directory. Clean-source verification reused the installed engine and SDKs. This is loaded-session persistence with prototype art. Save restoration, final skeletal art, many-body performance/soak, obstacle-edge settling and a separate clean-machine deployment remain unaccepted. Simulated engine keys exercise the input pipeline; a human physical-device playtest remains separate.
