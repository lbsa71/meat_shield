# Meat Shield

A fast first-person fortress assault in which instant respawns and persistent bodies turn each death into a construction move. Reach a useful position, leave cover for the next life, and use the route you built to breach the fortress.

**Repository status:** first playable turret approach, 2 November 2026, alongside the full-game planning baseline. Meat Shield is the working name; the commercial title is unresolved.

Run the Windows prototype with `Run-MeatShield.cmd` after packaging. It includes first-person movement, crouch (hold Ctrl or C), sprint/jump, an automatic turret, cover, damage, persistent physical ragdolls, a short death-camera pull-back/return, procedural animation, and synthesized effects. A batch of 42 physics bodies drops together to form a central pile. The low turret allows crouching behind a small stack. Bullet impacts twist active ragdolls at the hit location. Moving and settled meat stop bullets at their actual surfaces. Bodies finish collapsing and settling before their collision locks; every corpse stays across lives and R retries in the loaded level. See [run/build instructions](docs/engineering/running-the-prototype.md). The full fortress, constrained placement and save systems remain in development.

## Start here

1. Read the [developer handoff](docs/handoff.md) for the first implementation tasks and milestone gates.
2. Read the [project brief](docs/brief.md), [game design](docs/design/game-design.md), and [technical design](docs/engineering/technical-design.md).
3. Use the [production plan](docs/production/production-plan.md) and [implementation backlog](docs/production/backlog.md) to assign work.
4. Review [source decisions and assumptions](docs/source-and-decisions.md) before changing scope.

The [full document index](docs/README.md) covers level design, art and audio, UX and accessibility, engineering contracts, QA, playtesting, risks, resourcing, and release preparation.

Development follows [AGENTS.md](AGENTS.md): specification-driven and test-driven implementation, clean modular boundaries, environment abstractions, and procedural Unreal production with Blender for source assets. Development is AI-led under the product owner's direction.

The [game project](game/README.md) contains the playable slice and editor automation setup. Follow the [toolchain record](docs/engineering/toolchain-setup.md) for installation and automation status.

## Planning baseline

The proposed first release is an offline Windows PC game containing one fortress with three connected sectors, one player weapon, and two fixed defensive threat variants. The editor bootstrap pins Unreal Engine 5.8.4; gameplay suitability and packaged delivery still require the M0 spike. The original 24-week human-team capacity model is retained as a reference scenario. It is not the current AI-led delivery forecast or a staffing, funding, or launch commitment.

The first funded gate should prove a single 1–3 minute greybox assault before the team builds the full fortress. The key test is whether players deliberately place and then exploit bodies, rather than winning through repetitive accumulation.

## Maintain the handoff

Structured requirements, tasks, milestones, assets, and example contracts live in [planning](planning/README.md). The backlog table is generated from JSON.

```powershell
python tools/validate_planning.py
python tools/validate_planning.py --write-backlog
```

Python 3.10 or newer is sufficient; no packages are required. The validator checks planning consistency and local document links. It does not test a game build.
