# Meat Shield development workflow

Begin with the planning validator and M0 tasks. There is no runnable game, editor configuration, build script, CI runner, or binary storage configuration in this handoff. Establish them in a reviewed bootstrap change once the engine and storage decisions are made.

## M0 environment and repository setup

1. Record the selected Unreal patch, installation source, C++ toolchain, SDK, target platform, plugin versions, and benchmark PC in a tracked environment manifest.
2. Create a minimal first-person project and package a Windows build from a clean checkout. Confirm the license and redistribution requirements for selected engine and third-party content through their official terms before release.
3. Choose binary storage. For this existing Git repo, test Git LFS and locking for engine assets using a small real `.uasset` and `.umap` round trip. Record server support and clone instructions. If the team uses Perforce for content, document the boundary and revision pairing. Do not assume built-in editor Git support from the source control documentation. [Epic source control documentation](https://dev.epicgames.com/documentation/en-us/unreal-engine/source-control-in-unreal-engine).
4. Add engine-appropriate ignores for generated caches, intermediate builds, local saves, and IDE state. Add binary attributes only after LFS and lock behavior work on the team's host.
5. Create a licensed Windows build runner and define compile, data validation, packaged smoke, artifact retention, and failure ownership. Keep credentials outside the repository.

Proposed future game tree:

```text
game/MeatShield.uproject
game/Source/MeatShield/
game/Config/
game/Content/MeatShield/{Characters,Construction,Environments,Weapons,Threats,UI,Audio,Maps}/
game/Tests/Fixtures/
docs/
planning/
tools/
```

No empty engine directories or dummy binaries are required to begin planning review. Bootstrap should include a clean-checkout build recipe with actual commands for the pinned environment.

## Branches and review

Use short branches such as `codex/MS-011-corpse-placement` or the team's established naming convention. Reference task and requirement IDs in the PR. Include visible before-and-after evidence for movement, collision, visual, or UX changes. Protect engine binary assets with the chosen lock workflow and avoid simultaneous map edits until a level partitioning method is tested.

Use [the PR template](../../.github/PULL_REQUEST_TEMPLATE.md), [implementation task template](../../.github/ISSUE_TEMPLATE/implementation_task.md), and [bug report template](../../.github/ISSUE_TEMPLATE/bug_report.md). These are local templates; no remote issues have been created.

## Definition of ready

A task is ready when its dependency changes have landed, its requirement and gate are clear, an owner role has a named assignee, required content or fixtures are available, acceptance can be demonstrated, and its estimate fits capacity. A proposed engine or feature decision may be an explicit prerequisite for a spike rather than a blocker for unrelated work.

## Definition of done

A task is done when its deliverable is reviewed and integrated, its acceptance conditions are demonstrated on the intended packaged build where applicable, affected requirements and examples agree, relevant regression cases pass, and evidence is linked from the task or milestone review. No new critical crash, progress loss, unreliable cover, spawn loop, or invalid run classification remains. Evidence includes build revision and environment.

Spikes finish with a decision, traces or recordings, a reduced reproducible fixture, and estimate implications. They do not finish merely because a prototype branch exists. Art assets finish when imported, licensed or provenance-recorded, named, budgeted, and reviewed in the target scene.

## Build and change policy

Planned checks after bootstrap are compile, content and configuration validation, packaged launch and capture smoke, save round trip, movement and occlusion fixtures, and the latest representative performance scenario. Run large soak tests nightly or before gates; do not block every art edit on a full 1,000-death run unless the change affects that risk.

Record build ID as source revision plus content revision, rules version, engine patch, and configuration. Keep the latest green main build and gate candidates. Proposed retention is 14 daily builds plus all accepted milestone candidates; choose storage costs and access at M0.

After M2, new mechanics require a change record with player value, removed scope or extra capacity, engineering and QA costs, save compatibility, and gate impact. At M4, allow fixes and content correction only unless the producer and leads rebaseline the release.

## Current validation

Run `python tools/validate_planning.py` from the repo root. Use `--write-backlog` only when intentionally regenerating the backlog table after a data change. The tool checks document links, data shape, identifiers, task dependencies, requirement coverage, milestone capacities, asset references, and example invariants. It does not fetch external links, validate Unreal assets, or certify the game.
