# Meat Shield turret approach

`MeatShield.uproject` contains the runnable C++ slice for Unreal 5.8.4. A first-person character approaches an automatic turret through a procedural yard with cover, health/hit feedback, animated movement/firing and synthesized audio. Death now creates an articulated Chaos ragdoll, pulls the camera back to show its fall, then returns to the protected starting line. Every older corpse remains in the loaded scene with a frozen visible pose and separate stable collision. The full siege, constrained placement and saves remain future work.

Use [run/build instructions](../docs/engineering/running-the-prototype.md) for the standalone Windows executable. Runtime code separates engine-independent rules, gameplay adapters, procedural scene assembly, presentation/audio and opt-in diagnostics under `Source/MeatShield`. Scene and audio recipes are source code; this slice adds no downloaded assets or generated binary content to source control.

Open the project with the verified Unreal installation. The project enables Epic's official MCP and toolsets for Editor targets, together with Python and the procedural authoring foundation. Enable the local MCP server explicitly using `-ModelContextProtocolStartServer` at editor startup or the documented editor console command. The normal endpoint is `http://127.0.0.1:8000/mcp`.

The bootstrap records Enhanced Input's standard classes without gameplay actions or generated device/locale defaults. Android File Server is disabled for the Windows setup, preventing generated device-network credentials in shared configuration.

Use `MEAT_SHIELD_UNREAL_ROOT` or an explicit local path when invoking the engine. Keep engine installation paths, credentials, caches, and generated user settings out of shared configuration. Exact verified commands and evidence belong in the [toolchain setup record](../docs/engineering/toolchain-setup.md).

From the repository root, with `MEAT_SHIELD_UNREAL_ROOT` set for the process:

```powershell
python -m unittest discover -s tools/tests -p test_unreal.py
python tools/unreal.py smoke --dry-run
python tools/unreal.py smoke
python tools/unreal.py headless
```

Use `editor` in place of `headless` to keep the normal graphics pipeline and interactive editor. The launcher also accepts `--engine-root` and `--cache-root`. It validates the exact engine pin and keeps the default cache under the checkout's ignored `.local` directory. `headless` and `editor` report that a process started; MCP readiness requires a separate connection check. Smoke mode requires both successful exit and the running engine's explicit version marker.

Editor scripting, headless checks, and MCP interaction validate the toolchain. Run `tools/test_rules.ps1` for C++ rules, `tools/build.ps1` to build/package and `python tools/verify_package.py` for the rendered packaged scenario. Gameplay changes follow [AGENTS.md](../AGENTS.md), including specifications, meaningful failing tests, implementation, and relevant integrated verification.
