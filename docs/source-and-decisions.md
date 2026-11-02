# Meat Shield source decisions and assumptions

Prepared 1 November 2026 from [Game Concept Analysis](https://chatgpt.com/c/6ae73b03-82c0-83eb-a56f-1f302218bd97). The conversation contains an initial concept request, a design analysis, a naming question, and naming suggestions. The full prototype section was checked in the browser after the conversation tool clipped that response. This is a synthesis for developers, not a verbatim transcript.

## Source requirements

| Source | User intent | Treatment in this handoff |
| --- | --- | --- |
| Original concept request | Fast ultraviolent first-person assault on a fortress with ultrarealistic presentation | Core product direction |
| Original concept request | Spawn immediately after death | Core loop; a measurable engineering target is proposed separately |
| Original concept request | Die where the body becomes good cover for the next run | Signature mechanic and primary prototype question |
| Original concept request | Build walls and moats through earlier deaths | Accumulated terrain and existing ditch crossings; excavation deferred as a new system |
| Naming question | Whether Meat Cover is the best English title | Commercial name remains open |
| Current repository request | Create project and production planning artifacts for developer implementation | Local repository handoff |

## Analysis suggestions carried into the proposed baseline

The previous assistant suggested one to three useful placements, short experimental lives, fixed advancing assault points, temporary suppression with permanent shutdown at controls, stable corpse terrain after brief settling, time as the primary result, unlimited lives, and persistent world state. It proposed a small single-player prototype with one weapon, one threat, a breach objective, and an intended successful attempt of roughly one to three minutes. These are suggestions, not approvals by the user.

The references to Super Rude Bear Resurrection, Life Goes On, Die Again, ULTRAKILL, and Bodycam were creative comparisons in that analysis. No feature, sales claim, or production estimate in this handoff depends on those products having a particular current implementation.

## Decision register

All entries below are **proposed** unless explicitly labeled source requirement. Owners are responsibilities to assign at kickoff; they are not named or staffed people. A default allows prototype planning to continue and does not authorize spending.

| ID | Decision or assumption | Status | Owner | Resolve by | Consequence if changed |
| --- | --- | --- | --- | --- | --- |
| DEC-01 | Immediate retry and useful persistent bodies | Source requirement | Product owner | Preserved throughout | Changes the premise |
| DEC-02 | Meat Shield as working name, repo name unchanged | Proposed | Product owner | M2; commercial clearance before M5 | Rename UI and release assets |
| DEC-03 | Offline Windows PC, keyboard mouse and controller | Proposed | Product owner | M0 | Input packaging testing and budget change |
| DEC-04 | Unreal Engine 5 with C++ systems and Blueprint content | Proposed | Technical lead | M0, exact patch pinned | Re-estimate engineering and content pipeline |
| DEC-05 | Short bounded ragdoll followed by stable simple collision | Proposed | Technical lead and designer | M1 | Central feasibility risk; ADR 0002 |
| DEC-06 | Continuous siege and fixed captured checkpoints | Analysis suggestion adopted for planning | Designer | M1 | Rework save world and threat rules |
| DEC-07 | One fortress, three sectors, rifle, two fixed threats | Proposed | Product owner and producer | M2 | Reforecast content and QA |
| DEC-08 | 24 baseline weeks plus five contingency weeks | Estimate | Producer | Reforecast at every gate | No calendar commitment until staffing is known |
| DEC-09 | Original human-team staffing retained only as a reference scenario; development is now AI-led | Superseded by user direction | Product owner | M0 | Reforecast from actual implementation and validation throughput |
| DEC-10 | 1080p at 60 fps target on a nominated test PC | Proposed | Technical lead | M0 hardware; M2 budget | Quality settings or scope change |
| DEC-11 | Unlimited lives, active play timer, local records | Analysis suggestion adopted for planning | Designer | M1 | Rework scoring and resource rules |
| DEC-12 | Bodies are not destructible in baseline | Proposed | Designer | M1 | More physics, recovery, save and QA work |
| DEC-13 | Ammo replenishes at checkpoints; magazine and reserve survive death | Proposed | Designer | M1 | Retest death as reload exploit |
| DEC-14 | English at launch with externalized strings | Proposed | Product owner | M2 | Translation and certification costs change |
| DEC-15 | Storefront price territories and public name unresolved | Open | Product owner and producer | M4, before release preparation completes | Blocks commercial release only |
| DEC-16 | No mass or damage advantage for additional corpses in a full local pile | Proposed | Designer and technical lead | M1 | Protects placement from accumulation grind |
| DEC-17 | Specification-driven and test-driven development | User requirement | Development agent | Every behavior change | Specs and failing tests precede implementation |
| DEC-18 | Clean modularization separation of concerns and environment abstractions | User requirement | Development agent | Every architecture change | Isolate rules engine adapters presentation and platform dependencies |
| DEC-19 | Full Blender and Unreal capabilities with procedural production primarily in Unreal | User direction | Development agent and product owner | Pipeline setup and each content family | Reproducible recipes seeds exports and validated encounter constraints |
| DEC-20 | Pin Unreal 5.8.4 for editor bootstrap; use official localhost MCP and independent commandlet tooling | Accepted setup decision; M0 product acceptance pending | Development agent | Packaged environment gate; ADR 0004 | Revalidate engine plugins automation and packaging before changing the pin |

## External engineering references

DEC-21: Accepted by the product owner's 2 November 2026 request: deliver the [turret approach](design/first-playable.md) as the first runnable slice. [ADR 0005](engineering/adr/0005-first-playable.md) records procedural proxy presentation and the staged scope.

Reviewed 1 November 2026. Pin the engine patch at M0 and check these references against that patch. The architecture and budgets are project proposals inferred from these capabilities; the documentation does not establish their suitability for this game.

- Unreal supports simple and complex collision with different simulation constraints. The proposed gameplay proxy uses simple shapes. [Epic collision documentation](https://dev.epicgames.com/documentation/en-us/unreal-engine/simple-versus-complex-collision-in-unreal-engine).
- Unreal provides custom SaveGame objects and asynchronous save operations. [Epic save documentation](https://dev.epicgames.com/documentation/en-us/unreal-engine/saving-and-loading-your-game-in-unreal-engine).
- Unreal navigation can use local tiles, modifiers, and links. The baseline uses fixed threats to avoid requiring arbitrary navigation over piles. [Epic navigation documentation](https://dev.epicgames.com/documentation/en-us/unreal-engine/overview-of-how-to-modify-the-navigation-mesh-in-unreal-engine).
- Editor source control support does not establish this repo's binary workflow. Choose and verify that workflow at kickoff. [Epic source control documentation](https://dev.epicgames.com/documentation/en-us/unreal-engine/source-control-in-unreal-engine).
