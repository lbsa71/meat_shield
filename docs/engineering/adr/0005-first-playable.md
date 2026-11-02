# First runnable turret approach

Accepted 2 November 2026. REQ-025; DEC-21. The owner requested a runnable first-person approach to an automatic tower with hit feedback, animations and sound, before implementing the complete game.

Use a C++ game module with Unreal's Character movement and First Person template patterns for camera/input. Generate a deterministic yard, turret, proxy arms, procedural animation and original synthesized audio in runtime modules. This gives the packaged game explicit reproducible inputs without copying the entire Arena Shooter or adding binary asset dependencies before the asset workflow is ready. Use engine primitives as a temporary art kit. Keep pure threat and health rules in a small engine-independent library tested with the installed C++ compiler.

The [slice specification](../../design/first-playable.md) bounds this delivery. Core body construction, player weapon, checkpoints, persistence, full skeletal animation and final realistic art remain future work. Preserve MCP/editor configuration, but authoring plugins must stay out of packaged targets. Gameplay integration uses an explicit scenario mode for packaging checks; normal launches always accept live input. Never represent scripted movement alone as a manual playtest.
