# Run the first turret approach

REQ-025 / TC-029. The C++ game/editor build, Windows BuildCookRun package and rendered collision/damage scenario have run successfully. Final evidence is recorded in the [first-slice validation record](../quality/first-playable-validation.md). This is a development prototype; the full corpse-construction siege is not implemented in this slice.

On this development checkout, double-click `Run-MeatShield.cmd` at the repository root after a successful package. It launches `.artifacts/TurretApproach/Windows/MeatShield.exe` in a 1600Ã—900 window. Keep the Windows folder together when copying the build. The standalone package runs without Unreal Editor or Blender installed. Another PC may need the included Unreal prerequisites installer under `Engine/Extras/Redist/en-us`; install it if the executable reports missing runtime dependencies.

Controls: WASD move, mouse look, Shift sprint, hold Left Ctrl or C to crouch, Space jump, R return to protected spawn, M mute/unmute effects, Escape quit. Move beyond the green line to enter turret range. Concrete and meat block fire at their actual silhouettes. Crouching lowers your capsule, camera and turret target point, slows movement, and respects ceiling clearance on release. The tower warns before a burst, flashes and recoils when firing, and pauses between bursts. Hits reduce health, flash the screen edge and play an impact sound. Lethal damage leaves a physical ragdoll and begins a 1.05 s camera pull-back/observation/return to the start. Bodies continue physically collapsing after camera return and lock only after a supported, quiet pose. Every corpse remains across later deaths and R retries, with its settled pose and solid collision preserved. R is ignored during the short death transition. Quitting/relaunching starts a fresh demo; saves are not implemented. Procedural arm swing and footsteps accompany movement.

To reproduce from source in PowerShell:

```powershell
# Configure the engine location for this shell only.
$env:MEAT_SHIELD_UNREAL_ROOT = '<your Unreal 5.8.4 installation>'
./tools/test_rules.ps1
python -m unittest discover -s tools/tests -p test_unreal.py
python -m unittest discover -s tools/tests -p test_package_evidence.py
python tools/validate_planning.py
./tools/build.ps1 -Target Package
python tools/verify_package.py
./Run-MeatShield.cmd
```

Build Tools 2022 with MSVC, Windows SDK and .NET Framework 4.8 SDK are required to compile the editor module. The ordinary .NET SDK alone does not provide the last component. `tools/build.ps1` validates the exact engine pin, builds the editor module, then runs BuildCookRun with build/cook/stage/archive and prerequisites. It restores the caller's cache setting and keeps derived data, logs, binaries and packages in ignored local directories. A failed build or absent executable is an error, not acceptance.

For the integration scenario, launch the packaged executable with `-MSScenario -unattended -nosplash -windowed -ResX=1280 -ResY=720`. It checks protected spawn, exposed turret traces and damage, lethal reset, cover and bounded scene actors using the real world. Inspect `Saved/turret-scenario.json` and the log marker `MS_SCENARIO_RESULT`. This scenario teleports the player to fixtures and exits; normal launches never do so. Visual and audible output require separate rendered verification.

`python tools/verify_package.py` runs the current rendered scenario, requires a matching completion marker/report and fresh PNG/WAV files, and rejects silent/clipped output or logged errors. It exercises controls through Unreal's simulated key events; this proves the engine input pipeline, not an OS-level physical-device test. The scenario explicitly enables background audio for its capture and returns a failure if Escape does not exit. Normal launches use the usual focus/audio behavior.

The current ragdoll fixture checks two independent deaths, camera clearance and timing, duplicate lethal/R requests during death, real physics toppling, committed collision, unchanged older poses and corpse retention after R. The expanded crouch/settle fixture also checks native crouch clearance, active meat occlusion, standing and crouched deaths, and the actual first-hit damage adapter with/without meat in the ray. It writes fresh `ragdoll-fall.png`, `ragdoll-two.png` and close `meat-cover.png` captures. See the [ragdoll validation record](../quality/persistent-deaths-validation.md) for results and limits. Body art is a procedural articulated mannequin proxy, not final skeletal character art.

This machine's missing .NET Framework SDK was supplied from Microsoft's signed 4.8 SDK package, verified against the cached Visual Studio catalog's SHA-256 hashes, extracted under ignored `.local/netfx-sdk`, and registered for the current user. No system-level installer approval was needed for that route. After the owner approved the original installer, the system SDK headers/libraries were verified and the temporary per-user registration was removed. The editor build still passes with system discovery. Another machine should install the SDK through its normal Build Tools setup. Source and packaged runtime do not depend on this workstation's SDK path.
