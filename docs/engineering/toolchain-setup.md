# Meat Shield Unreal and Blender setup

Use the full Unreal Engine installation with its command-line and editor automation facilities. The intended first engine series is Unreal Engine 5.8 because it includes Epic's official Unreal MCP; pin the installed release patch after the first build and automation checks. Do not choose a Preview build for the project. [Epic installation guide](https://dev.epicgames.com/documentation/en-us/unreal-engine/install-unreal-engine).

## Verified starting environment

Checked 1 November 2026:

| Component | Observed result |
| --- | --- |
| Blender | 5.2.2 LTS; background factory-startup Python invocation passed |
| Visual Studio Build Tools | 2022 version 17.14.30 installed |
| MSVC toolset | 14.44.35207 installed; Unreal acceptance still requires a real build |
| Windows SDK | 10.0.26100.0 installed |
| .NET SDK | 10.0.203 installed |
| Unreal Engine | Not yet installed or verified |
| Epic Games Launcher | Official installer started; waiting for Windows administrator approval |
| Unreal MCP | Not yet running or connected |

Update this status when setup completes. Installed prerequisites are not proof that a C++ Unreal project builds. Check the pinned engine's requirements and actual build diagnostics before adding or replacing toolchains. [Epic C++ environment guide](https://dev.epicgames.com/documentation/en-us/unreal-engine/setting-up-visual-studio-development-environment-for-cplusplus-projects-in-unreal-engine).

## Installation route

The regular Epic Games Launcher is the default binary installation route. Engine download requires an Epic account and acceptance of Epic's engine agreements. The product owner handles sign-in, Windows security approvals, and agreement acceptance when required. Review the engine installation Options dialog for actual selected disk usage before starting the download. [Epic installation guide](https://dev.epicgames.com/documentation/en-us/unreal-engine/install-unreal-engine).

Select Windows development components first. Omit unrelated mobile and console targets and optional large symbol packages until needed. Preserve the full editor and relevant procedural plugins. Leave additional space for derived data, compilation, cooked builds, and Blender exports; do not consume the drive's remaining space with the engine alone.

Epic also documents a silent offline installer, but access requires an eligible organization; it is not assumed available to this account. A source build is another route, but should be chosen for an actual source requirement rather than assumed to save space or setup effort. [Epic offline installation guide](https://dev.epicgames.com/documentation/en-us/unreal-engine/offline-installer-of-unreal-engine).

## Automation layers

| Layer | Intended use |
| --- | --- |
| C++ and engine build tools | Tested gameplay rules, engine adapters, builds and packaging |
| Unreal commandlets and editor Python | Repeatable asset import, scene generation, validation and baking |
| Official Unreal MCP | Inspect and edit a live editor, discover tools and run supported automation |
| Blender background Python | Source asset generation, preparation, export and validation |
| Full editor and rendered builds | Visual QA, animation, lighting, camera, material and performance review |

Unreal supports headless Python commandlets through `UnrealEditor-Cmd.exe -run=pythonscript -script=...`; enable the Python Editor Script plugin in the project first. Python authoring automation is separate from packaged gameplay code. [Epic editor Python guide](https://dev.epicgames.com/documentation/en-us/unreal-engine/scripting-the-unreal-editor-using-python).

Build and packaging recipes will use the installed engine's `Engine/Build/BatchFiles` tools. `RunUAT.bat BuildCookRun` performs the build/cook/stage/package pipeline. Capture actual working arguments from the pinned project's successful build instead of treating an untested example as a build recipe. [Epic build operations guide](https://dev.epicgames.com/documentation/en-us/unreal-engine/build-operations-cooking-packaging-deploying-and-running-projects-in-unreal-engine).

## Official MCP setup

Enable Unreal MCP (`ModelContextProtocol`) and the required toolsets, initially through `AllToolsets`. Start locally with `ModelContextProtocol.StartServer` or `-ModelContextProtocolStartServer`. The default endpoint is `http://127.0.0.1:8000/mcp`. Keep its default loopback binding and serialize editor tool calls. Verify discovery and one reversible editor operation before relying on it. The plugin is Experimental; CLI and tests remain independent delivery paths. [Epic Unreal MCP guide](https://dev.epicgames.com/documentation/en-us/unreal-engine/unreal-mcp-in-unreal-editor).

Client configuration and actual connection verification follow installation. Do not overwrite an existing client configuration or claim this chat has engine tools before they are available and tested.

## Reproducible project setup

Use an explicit local setting such as `MEAT_SHIELD_UNREAL_ROOT` for engine discovery and `MEAT_SHIELD_BLENDER_EXE` for Blender. Keep workstation paths out of reusable rules and shared recipes. Setup tooling must validate versions, executables, plugin availability, disk space, and subprocess results through environment adapters with deterministic tests.

Before M0 acceptance, record the exact engine patch, plugin list, toolchain, benchmark machine, binary asset workflow, clean-checkout build command, test command, packaging command, and generation recipe. Re-run Blender smoke, Unreal commandlet smoke, MCP discovery and reversible edit, C++ build, and packaged launch. Rendering and performance checks must use the real graphics pipeline rather than a no-render logic-test path.
