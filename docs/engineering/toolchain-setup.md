# Meat Shield Unreal and Blender setup

Use the full Unreal Engine installation with its command-line and editor automation facilities. The editor bootstrap pins Unreal Engine 5.8.4, changelist 58902052, which includes Epic's official Unreal MCP. Packaged-game suitability remains subject to M0 checks. Do not choose a Preview build for the project. [Epic installation guide](https://dev.epicgames.com/documentation/en-us/unreal-engine/install-unreal-engine).

## Verified starting environment

Starting components checked 1 November 2026; game/editor compilation and packaging checked 2 November:

| Component | Observed result |
| --- | --- |
| Blender | 5.2.2 LTS; background factory-startup Python invocation passed |
| Visual Studio Build Tools | 2022 version 17.14.30 installed |
| MSVC toolset | 14.44.35207 installed; first-slice game/editor compile, link and package passed |
| Windows SDK | 10.0.26100.0 installed |
| .NET SDK | 10.0.203 installed |
| Unreal Engine | 5.8.4 installed and launcher verification complete; runtime and Python commandlet validated |
| Epic Games Launcher | Official installer and first-run update completed; Epic sign-in completed |
| Unreal MCP | Official localhost server connected through SDK and native chat tools; 52 toolsets discovered and selection/restore passed |

See the [validation record](../quality/toolchain-validation.md) for commands, results, and an open engine smoke-test failure. Installed prerequisites are not proof that a C++ Unreal project builds. Check the pinned engine's requirements and actual build diagnostics before adding or replacing toolchains. [Epic C++ environment guide](https://dev.epicgames.com/documentation/en-us/unreal-engine/setting-up-visual-studio-development-environment-for-cplusplus-projects-in-unreal-engine).

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

The project client configuration and live connection have been checked. Do not overwrite an existing client configuration when adapting the setup on another machine; inspect it first and merge the local endpoint deliberately.

## Reproducible project setup

Use an explicit local setting such as `MEAT_SHIELD_UNREAL_ROOT` for engine discovery and `MEAT_SHIELD_BLENDER_EXE` for Blender. Keep workstation paths out of reusable rules and shared recipes. Setup tooling must validate versions, executables, plugin availability, disk space, and subprocess results through environment adapters with deterministic tests.

The [editor bootstrap project](../../game/MeatShield.uproject) hosts setup validation before gameplay modules are implemented. It enables the installed official MCP and toolsets, Python, PCG, and Geometry Script. MCP and authoring-only plugins are restricted to Editor targets. Bootstrap acceptance requires the installed 5.8.4 editor to load this descriptor, a headless Python smoke to report the engine version, and a local MCP client to discover tools and perform an editor operation. These are environment checks; they do not complete the packaged FPS, binary collaboration, or gameplay acceptance conditions in MS-001.

All Toolsets enables Game Features through its dependencies. The bootstrap must register the `GameFeatureData` primary asset type with the `AlwaysCook` rule, matching the installed engine editor's own corrective action. The first live commandlet exposed the missing rule and returned a failure despite running the Python probe; successful bootstrap requires a clean commandlet exit as well as its marker. The editor starts in the stock Entry map while authored gameplay levels remain unimplemented.

The first runtime slice marks that scan type editor-only, matching the toolset dependency boundary. The first packaged test exposed a handled ensure when the runtime attempted to load the editor-only Game Features class; the editor-only scan flag removes that dependency from normal game startup.

The Windows bootstrap targets D3D12 and Shader Model 6, matching the installed engine's blank-project template. SM6 makes modern rendering features available; enabling and budgeting those features remains part of the representative scene work. [Epic rendering requirements](https://dev.epicgames.com/documentation/en-us/unreal-engine/hardware-and-software-specifications-for-unreal-engine).

The project-scoped [MCP client configuration](../../.codex/config.toml) points to the local editor endpoint. Start the editor before connecting a client; a configured URL alone does not prove the server is running. Project-scoped Codex configuration applies to trusted projects. [Official MCP configuration documentation](https://learn.chatgpt.com/docs/extend/mcp?surface=cli).

The project launcher must accept an explicit engine root or `MEAT_SHIELD_UNREAL_ROOT`, with the explicit argument taking precedence. Before launching, it must reject a missing project, missing executable, or engine version/changelist differing from [toolchain.json](../../planning/toolchain.json). Smoke mode runs the Python commandlet and exits. Headless mode starts the editor with a null renderer and local MCP; normal editor mode starts MCP while retaining graphics. Every mode uses a process-local derived-data cache override under `.local` by default, preserving the caller's environment. A dry run must launch nothing and print only the command and relevant paths, never the full environment. These contracts trace to REQ-019, REQ-021, and MS-001.

Before M0 acceptance, record the exact engine patch, plugin list, toolchain, benchmark machine, binary asset workflow, clean-checkout build command, test command, packaging command, and generation recipe. Re-run Blender smoke, Unreal commandlet smoke, MCP discovery and reversible edit, C++ build, and packaged launch. Rendering and performance checks must use the real graphics pipeline rather than a no-render logic-test path.
