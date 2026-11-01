# Meat Shield development instructions

These instructions apply throughout this repository. The product owner has directed that development be AI-led, specification-driven, test-driven, cleanly modular, and procedural wherever practical through Unreal. Roles in the planning files identify responsibilities, not a requirement to recruit the originally modeled human team.

## Specification driven development

- Treat `planning/requirements.json`, approved decisions, gameplay and technical designs, and versioned data contracts as the implementation contract. Read the affected specifications before changing behavior.
- Before implementing new or changed behavior, update the requirement and its observable acceptance criteria. Include relevant normal, failure, boundary, persistence, and performance cases. Keep specifications focused on the change.
- Trace implementation work and tests to requirement and task IDs. Resolve conflicting specifications explicitly; update affected design, tuning, contracts, test cases, and backlog together.
- Record consequential architecture and scope changes in a decision record. Separate user requirements, accepted decisions, experimental hypotheses, and deferred work.

## Test driven development

- For executable behavior changes and bug fixes, write the smallest meaningful automated test first. Run it and observe a failure caused by the missing or incorrect behavior before implementing the fix.
- Follow red, green, refactor: demonstrate the relevant failure, implement the minimum coherent behavior, then improve structure while keeping the tests green. A missing compiler, engine, or dependency is an environment blocker, not a successful red test.
- Test observable contracts and invariants rather than copying implementation logic or relying on mocks alone. Add regression coverage for fixed defects, especially cover stability, capture ordering, death idempotency, saves, and resource continuity.
- Prefer fast deterministic tests for rules and data; use Unreal automation, functional fixtures, and packaged integration tests for engine behavior. Supply clocks, random seeds, and fixture state explicitly.
- Run the relevant checks after changes and the required gate checks before acceptance. Report what actually ran, its result, and anything blocked. Never claim unexecuted tests passed.
- Documentation-only and cosmetic changes do not need artificial unit tests. Run applicable document, data, build, visual, or content validation instead. Record manual or visual verification when automation cannot establish the result.

## Modularization and separation of concerns

- Organize modules around clear responsibilities and stable contracts. Separate gameplay rules, application orchestration, Unreal integration, editor tooling, presentation, persistence, and diagnostics.
- Keep construction geometry authority separate from cosmetic bodies and effects. Keep run, world, weapon, and save state outside the transient player pawn. Do not let UI, audio, or visuals own gameplay decisions.
- Make dependency direction explicit. Reusable rules and data models should not depend on editor UI, platform paths, remote services, or concrete storage adapters. Avoid circular dependencies and global mutable state.
- Use small cohesive APIs and composition. Avoid catch-all managers, giant actors, unrelated utilities, duplicated rules, and speculative abstraction layers. Add interfaces where they isolate a real environment boundary or a tested alternative.
- Preserve engine conventions where they matter, including object lifecycle, reflection, serialization, and game-thread constraints. Module boundaries must support Unreal's build and runtime behavior.

## Environment abstractions

- Centralize environment configuration and validate it early. Discover engine, Blender, SDK, and tool paths through explicit configuration or documented discovery; never hardcode this workstation's paths into shared logic.
- Put filesystem and save access, clocks and scheduling, randomness, input devices, platform capabilities, process execution, and optional network access behind narrow replaceable boundaries where gameplay or tools depend on them.
- Keep core tests independent of installed tools and live services through in-memory adapters and deterministic fixtures. Also exercise real adapters in integration tests; mocks do not prove an engine or filesystem contract.
- Separate editor-only automation from packaged gameplay. Keep local configuration, secrets, caches, logs, generated binaries, and machine manifests out of shared source unless deliberately recorded as sanitized evidence.
- Use structured arguments for subprocesses, explicit units and coordinate conversion, actionable errors, bounded timeouts, and repeatable setup. Do not silently fall back to different rules or delete progress when an environment dependency fails.

## Unreal and Blender production pipeline

- Use the full Unreal Engine and Blender toolsets where they improve the result. Prefer procedural authoring and assembly in Unreal over manually maintaining repeated scene content.
- Evaluate Unreal PCG, Geometry Script, material graphs, Niagara, animation and rigging systems, and suitable rendering features against the game's needs and measured budgets. Do not enable every feature by default or make experimental editor tools a shipped runtime dependency without a decision and tests.
- Use Blender for source modeling, sculpting, UVs, rigging, animation, and asset preparation when those operations are better served there. Automate repeatable Blender work through versioned scripts and explicit export presets.
- Keep procedural recipes, graphs, source scripts, seeds, parameters, dependencies, and versions under source control. Generated output must be reproducible and traceable to its recipe; define which baked artifacts must also be tracked for packaging.
- Build procedural generation around authored encounter constraints. Preserve intentional routes, threat sightlines, placement opportunities, collision, checkpoint safety, and navigation. Generation must not replace the core placement design with uncontrolled randomness.
- Make generation idempotent or explicitly incremental. Use stable naming and IDs, avoid duplicating assets on reruns, and validate scale, coordinate systems, materials, collision, and provenance during import and baking.
- Prefer Epic's official editor MCP, commandlets, Python scripting, build tools, and automation facilities where available. Use project-local tooling with explicit configuration, preserve localhost binding, and keep editor calls serial where required by the engine.

## Delivery and evidence

- Use `python tools/validate_planning.py` for planning consistency; regenerate the backlog with `--write-backlog` after changing its source data. Keep human-readable and structured artifacts aligned.
- Keep changes reviewable and focused. Include specification IDs, relevant test evidence, and material risks in implementation reviews.
- Treat the original human staffing and 24-week capacity model as a reference scenario, not the current AI-led delivery commitment. Reforecast from actual implementation and validation results.
- Never count a plan, generated asset, successful editor action, or passing unit test alone as release evidence. Acceptance requires the applicable integrated, packaged, visual, performance, and recovery checks.
