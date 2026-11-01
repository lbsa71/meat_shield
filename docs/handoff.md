# Meat Shield developer handoff

Start by proving one useful death. The repository contains a proposed product baseline, implementation contracts, production work breakdown, verification plan, and an Unreal editor automation bootstrap. Blender background Python, the Unreal Python commandlet, and live editor MCP control have been exercised; see the [toolchain evidence](quality/toolchain-validation.md) for results and open issues. Gameplay, C++ compilation, packaged builds, and release acceptance remain unverified.

## Read and assign

Read [AGENTS.md](../AGENTS.md), the [brief](brief.md), [source decisions](source-and-decisions.md), [game design](design/game-design.md), and [technical design](engineering/technical-design.md). The [full index](README.md) routes each responsibility to its artifacts. Development is AI-led under the product owner's direction. Planning roles identify responsibilities across design engineering art QA and delivery; they do not require the originally modeled human staffing. Assign any human playtest account release or specialist responsibilities that cannot be completed through the development environment, and reflect them in the forecast.

## First ten working days

These activities are the intended kickoff order, not a promise that every milestone is complete in ten days. The producer adapts them to actual availability and the accepted engine.

| Days | Engineering | Design and art | Production and evidence |
| --- | --- | --- | --- |
| 1–2 | Pin candidate engine/toolchain, nominate benchmark, package minimal project | Annotate courtyard route, import body/wall proxy and scale fixture | Assign owners, review defaults, establish binary workflow; MS-001 through MS-004 |
| 3–5 | Begin pawn replacement and movement; test stable simple proxy against player and fire | Refine damaged wall and height opportunity; review stable silhouette | Record M0 decision and packaged environment evidence; book QA and audio windows |
| 6–8 | Implement bounded supported placement and persistent weapon/threat state incrementally | Integrate first useful placement and capture control in greybox | Inspect time-to-control and placement traces; keep scope to one encounter |
| 9–10 | Integrate fixed spawn capture and continuous siege; expose placement reason events | Observe first internal naive/planned routes and newest-body feedback | Update estimate and risks; schedule the M1 eight-player test when the build is ready |

M1 continues through weeks 2–4. Finish fixture verification and observed tests before accepting the prototype gate. Do not treat the day-ten build as an automatic go for content production.

## Implementation contract highlights

- One death commits at most one body and creates one new pawn. The world stays loaded.
- Stable proxies determine support, bullets, and sight. Cosmetic ragdolls cannot change useful terrain.
- Tile capacity and pooled visuals bound cost without deleting existing useful cover.
- Same-update lethal damage cancels an unfinished capture; successful capture applies gate threat checkpoint and save request atomically.
- Magazine reserve reload and threat timers persist through death. Resupply remains available at the current assault point.
- Saves use committed world snapshots with previous-good recovery. Resume and sector reset retain distinct run classifications.
- Time-to-control, placement repeatability, and route usefulness need recorded evidence before production expansion.

## Concrete work queue

Use [backlog.md](production/backlog.md) for human review and [backlog.json](../planning/backlog.json) for import into the chosen tracker. The JSON is tracker-neutral and does not claim to match a vendor's import API. Preserve IDs, dependencies, requirements, estimates, deliverables, and acceptance when importing. Split multi-day tasks into smaller implementation issues only after their interfaces and owners are known.

The 55-task baseline retains planned and unassigned status; the initial toolchain work provides partial MS-001 evidence without completing its packaged FPS and binary workflow conditions. No remote issues or messages to the development team have been sent. The [templates](../templates/README.md) are ready for implementation reviews, defects, playtests, decisions, and gate records.

## Kickoff decisions

Confirm or amend platform, commercial scope, team availability, engine patch, benchmark hardware, binary workflow, and funding for the next gate. Naming, store, pricing, and territories can remain open until their recorded deadlines. Changes to movement or construction require a route-fixture review; changes to staffing require a new forecast.

Run `python tools/validate_planning.py` before sharing the repository. Then package the repository or share its existing access with the receiving developers through your normal process. The handoff itself does not grant repository access, commit changes, or publish a release.
