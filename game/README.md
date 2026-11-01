# Meat Shield editor bootstrap

`MeatShield.uproject` is a minimal content project for validating Unreal 5.8.4 editor automation. It contains no implemented siege, player controller, authored gameplay level, or C++ gameplay module yet. C++ systems remain the proposed implementation direction.

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

Editor scripting, headless checks, and MCP interaction validate the toolchain. Gameplay behavior changes must follow [AGENTS.md](../AGENTS.md), including specifications, meaningful failing tests, implementation, and relevant integrated verification.
