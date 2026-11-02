# Run the first turret approach

REQ-025, REQ-026, REQ-027 / TC-029–031. The C++ game/editor build, Windows BuildCookRun package and rendered collision/damage scenario have run successfully. Current results are in the [batch pile and low turret validation record](../quality/physics-pile-validation.md), following the [first-slice validation](../quality/first-playable-validation.md). This is a development prototype; the full corpse-construction siege is not implemented in this slice.

On this development checkout, double-click `Run-MeatShield.cmd` at the repository root after a successful package. It launches `.artifacts/TurretApproach/Windows/MeatShield.exe` in a 1600Ã—900 window. Keep the Windows folder together when copying the build. The standalone package runs without Unreal Editor or Blender installed. Another PC may need the included Unreal prerequisites installer under `Engine/Extras/Redist/en-us`; install it if the executable reports missing runtime dependencies.

Controls: WASD move, mouse look, Shift sprint, hold Left Ctrl or C to crouch, Space jump, R return to protected spawn, M mute/unmute effects, Escape quit. Move beyond the green line to enter turret range. Bodies now pour from above the middle of the yard and physically form a pile. All 42 drop together from a separated aerial grid and collide with one another. You can play during the short physical settling period. The muzzle is about 95 cm above ground, allowing two prone bodies to block shots at a crouched torso. Lethal shots transfer their impact point into the ragdoll; further hits push and twist active limbs, while committed cover stays fixed. Concrete and meat block fire at their actual silhouettes. Crouching lowers your capsule, camera and turret target point, slows movement, and respects ceiling clearance on release. The tower warns before a burst, flashes and recoils when firing, and pauses between bursts. Hits reduce health, flash the screen edge and play an impact sound. Lethal damage leaves a physical ragdoll and begins a 1.05 s camera pull-back/observation/return to the start. Bodies continue physically collapsing after camera return and lock only after a supported, quiet pose. Every corpse remains across later deaths and R retries, with its settled pose and solid collision preserved. R is ignored during the short death transition. Quitting/relaunching starts a fresh demo; saves are not implemented. Procedural arm swing and footsteps accompany movement.

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

## MetaHuman avatar experiment

Both repository launchers now select the dressed human profile. Developers can launch the executable without `-MSRealism` for the primitive comparison profile. The player now has a full human body: look down to inspect the uniform. The same mesh, life seed, outfit and evaluated stance become the ragdoll at death. Initial movement is procedural FK; polished locomotion and foot planting remain open. Three camouflage palettes, seeded pattern placement and an optional vest provide prototype variation on one source human identity. These fitted shells are an initial wardrobe, not authored tactical uniforms or loose cloth simulation.

After packaging, `python tools/verify_package.py --offscreen --realism` runs the real collision/death scenario and requires current avatar evidence. `-MSRealism -MSArmProbe` exercises independent arms in a pinned-torso gravity fixture. `-MSRealism -MSRealismGallery` captures facial profiles. Add `-MSWardrobePreview` for full-body outfit captures. These development fixtures exit automatically; ordinary play does not.

The character source lives under `/Game/MeatShield/SourceCharacters`; Creator assembly outputs `/Game/MeatShield/Humans`. `tools/create_realism_assets.py` bakes the ground/materials/PCG recipe. `tools/create_uniform_assets.py` generates fitted skeletal wardrobe assets from the owned body. The wardrobe commandlet requires `-AllowCommandletRendering -RenderOffscreen` and must not use `-nullrhi`: render-data extraction without initialized skin-weight resources produces rigid, root-bound clothing. The recipe rejects that condition. Baked `.uasset` files use Git LFS; install LFS and run `git lfs pull` after cloning. Creator/cloud access is used for authoring, not packaged play.
