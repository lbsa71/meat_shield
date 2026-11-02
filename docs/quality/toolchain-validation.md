# Toolchain validation record

Observed 1 November 2026. Scope: REQ-019, REQ-021, TC-019, partial MS-001. This record validates editor automation; M0 and all gameplay/release gates remain open.

This is the historical setup record. The [2 November first-playable validation](first-playable-validation.md) records subsequent C++ compilation, clean-checkout packaging, rendered gameplay and audio checks.

## Environment and results

| Check | Actual result |
| --- | --- |
| Unreal runtime | 5.8.4, changelist 58902052; runtime Python version matches the central pin |
| Epic installation | File construction, relocation and final verification completed; installed-item metadata reports Installed |
| Blender | 5.2.2 LTS; background factory-startup Python probe exited 0 |
| Build prerequisites | VS Build Tools 2022 17.14.30, MSVC 14.44.35207, Windows SDK 10.0.26100.0 and .NET SDK 10.0.203 observed; C++ compatibility not yet proven |
| Workstation | Windows; i9-14900HX, about 32 GiB RAM, RTX 4090 Laptop GPU; this is an observed setup machine, not an accepted minimum-spec benchmark |
| Launcher regression suite | 20 tests passed using system Python 3.12.10 |
| Unreal Python commandlet | Exit 0 and explicit `MEAT_SHIELD_ENGINE_SMOKE` evidence from the running engine |
| Normal editor | Initialized with D3D12/SM6 on the NVIDIA adapter; Entry map check reported 0 errors and 0 warnings |
| Official MCP | Loopback listener owned by the editor; SDK negotiated protocol 2025-11-25; 52 toolsets discovered |
| Editor control | Both SDK and native chat MCP selected the existing Entry PlayerStart, verified it, then restored the original empty selection; no level assets saved |
| Codex client config | Project `.codex/config.toml` parsed and recognized by `codex mcp get unreal`; native Unreal tools subsequently connected in this chat |
| Engine diagnostic smoke run | 330 reported tests: 326 Success, 1 Fail, 3 Not Run; details below. These are engine tests, not game coverage |

The launcher tests were developed with failing tests before implementation, then rerun through green and refactoring. They cover path arguments containing spaces, explicit/environment precedence, engine patch and changelist mismatch, missing inputs, cache isolation, dry-run behavior, secret-free output, and real child-process marker/exit handling. A zero exit without engine evidence fails. Missing tools are setup failures, not acceptable red-test evidence.

## Repeat the bootstrap checks

From the repository root, set `MEAT_SHIELD_UNREAL_ROOT` locally to the installation directory, or supply `--engine-root`. Do not commit a workstation path.

```powershell
python -m unittest discover -s tools/tests -p test_unreal.py
python tools/validate_planning.py
python tools/unreal.py smoke --dry-run
python tools/unreal.py smoke
python tools/unreal.py editor
codex mcp get unreal
```

Use one editor instance for the project. `headless` starts persistent MCP with a null renderer; its launch-plan contract is tested, but a persistent headless MCP session has not yet been integration-tested. The successful headless runtime probe above used the separate `smoke` commandlet. Normal editor startup and live MCP were exercised together. Headless checks do not validate lighting, materials, animation, GPU performance, or visual quality.

For the reversible MCP check, discover toolsets, describe SceneTools and EditorAppToolset, and read the current level and selection. In `/Engine/Maps/Entry`, find the existing PlayerStart, select it, verify selection, and restore the original selection in a finally block. Stop if the fixture differs. Do not save or alter the stock engine map. Calls remain serial.

The Blender check uses the configured Blender executable with `--background --factory-startup --python-expr "import bpy; print(bpy.app.version_string)"`. Check exit status and actual version output.

## Findings and open work

**Resolved bootstrap issue:** All Toolsets enables Game Features. The first commandlet returned 1 because `GameFeatureData` lacked its required asset-manager rule, even though the version probe ran. Added the installed engine editor's own `AlwaysCook` corrective rule; the next commandlet returned 0 with matching evidence. The launcher requires both conditions.

**ENV-001 — open engine test failure:** Normal startup logs two `LogAutomationTest: Error: Condition failed` messages. A diagnostic run through the official MCP Automation Test Toolset identifies `Core.Logging.StructuredLogFormat` as failing with those two messages. The engine owns this test; no gameplay code exists yet. The same run reports `System.Core.GenericPlatform`, `System.Engine.FileSystem.JSON`, and `System.Engine.Projects.Loading.RapidJsonPluginLoading` as Not Run. The 14 UnifiedError tests passed independently. Do not describe engine automation as wholly green. Preserve this reproduction and investigate before accepting the engine/build environment at M0; no installed engine source was patched.

**Installer verification completed:** Epic's installed-item metadata transitioned from Incomplete to Installed after final verification, and its installation helper exited. Storage has adequate headroom after the product owner's cleanup; no user files were deleted by the development agent.

**Remaining acceptance:** A clean-checkout C++ build, packaged FPS launch, binary asset round trip and locking, build CI, representative rendering/performance scene, procedural asset reproduction, and gameplay fixtures remain unexecuted. None is implied by the editor or MCP results.

Raw logs, local MCP responses, process IDs, installation locations and derived data remain in ignored local directories. Shared records contain sanitized observations only.
