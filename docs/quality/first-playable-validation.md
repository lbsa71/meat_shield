# First playable validation

2 November 2026. REQ-025 / TC-029; partial MS-001, MS-005 and MS-008. Tested on the setup workstation recorded in [toolchain validation](toolchain-validation.md), with Unreal 5.8.4 / changelist 58902052. The scope is the [turret approach](../design/first-playable.md), not acceptance of the full siege.

This records the original instant-reset build. The subsequent [persistent-death validation](persistent-deaths-validation.md) covers the owner's ragdoll/camera continuation and supersedes that reset behavior.

## Executed checks

| Check | Result |
| --- | --- |
| Engine-independent C++ rules | 31 checks pass using the real MSVC compiler; initial incomplete implementation failed 19 checks before implementation |
| Python launch/evidence tooling | 30 tests pass; the ten new evidence checks first failed against an incomplete implementation |
| C++ game and editor targets | Both compile and link with MSVC 14.44 / Windows SDK 10.0.26100.0 |
| Windows package | BuildCookRun build/cook/stage/archive completed with exit 0 |
| Rendered packaged integration | 12 checks pass; 7 exposed hits cause exactly one lethal reset; 25 actors after retries; no package errors or handled ensures |
| Controls | Simulated engine key events verify movement, sprint/release, jump, retry, mouse look and mute; Escape exits through `UGameEngine::HandleExitCommand` with exit 0 |
| Collision | Protected spawn takes no fire; exposed player takes damage; concrete cover prevents further hits |
| Visual output | Fresh 1600×900 captures reviewed for readable yard/tower, cover, HUD, warning, muzzle/tracer and damage overlay; proxy arms and turret animate procedurally |
| Game audio output | Fresh stereo 48 kHz 16-bit mixer capture; RMS approximately 0.028, peak 0.34677 of full scale; non-silent without clipping |
| Windows root launcher | The packaged root `MeatShield.exe` forwards arguments, runs all checks and exits through Escape successfully |
| Clean checkout | Detached checkout of source revision `98b324f` started clean, passed all fast checks, compiled game/editor and packaged; its rendered 12-check scenario and audio verification passed, and source status stayed clean |
| Editor commandlet regression | Repeated pinned Python version smoke after adding the C++ module; exit 0 with matching evidence |

The package verifier requires a current process marker, matching fresh JSON, fresh PNG captures and non-silent/non-clipped WAV output. It rejects a passing status if the process logged errors. It does not certify visual quality by checking a PNG signature; the rendered captures were also inspected. Engine input events exercise the actual mapping and controller pipeline, but a physical keyboard/mouse and listening-device session remains a separate user playtest.

## Resolved findings

- The editor build lacked the .NET Framework SDK even though modern .NET and MSVC were installed. Verified Microsoft's signed SDK payload and catalog SHA-256, extracted it locally and registered its real include/lib directory for the current user. The actual editor build then passed. This was an environment blocker, not a red behavior test.
- The first package reached gameplay but logged a handled ensure while loading `GameFeatureData`. Its scan rule belonged to editor toolsets. Marked the scan type editor-only and repeated the package; the error is absent.
- The first background audio captures were silent. The opt-in diagnostic fixture now enables background audio for its own mixer capture. Normal game launches retain normal focus behavior. The later rendered package capture contains the intended effect signal.
- Visual review reduced tracer width and exposed the six-barrel assembly, and replaced block arms with rounded proxy geometry. These are prototype presentation changes, not final art acceptance.

## Reproduction and delivery

Use [run/build instructions](../engineering/running-the-prototype.md). `tools/build.ps1 -Target Package` writes the Windows folder under `.artifacts/TurretApproach`; `Run-MeatShield.cmd` launches it. `python tools/verify_package.py` reruns the rendered scenario and reports measured audio energy. Raw logs, screenshots, WAV and binary/debug outputs remain ignored local artifacts. The delivered executable hash and source revision belong in the accompanying sanitized build manifest.

The [build manifest](builds/turret-approach-2026-11-02.json) records the delivered binary hash and exact source revision. Clean checkout verification reused the installed engine/SDKs on this machine; it does not establish deployment on a separate clean machine.

The full first-person weapon, corpse construction, checkpoint capture, saves, skeletal character animation, final realistic assets, controller/rebinding flow, formal performance targets, binary asset locking and clean-machine deployment matrix remain open. Baseline tasks remain active with partial evidence; no full-game milestone is marked accepted. The earlier engine-owned `Core.Logging.StructuredLogFormat` issue remains recorded separately and did not appear as a packaged-game startup error.
