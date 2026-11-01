# Meat Shield project brief

Meat Shield is a fast siege FPS about turning the ground that killed you into the route that lets you advance. The player is one mind conducting an assault through successive bodies. Every useful death gives the next life a spatial advantage.

## Player promise

Storm a fortress, respawn immediately, and exploit the physical remains of earlier attempts. The player should understand why a particular placement helped, then begin arranging deliberate sequences of movement, combat, and sacrifice. Shooting buys time to reach useful positions; construction creates the route for the final push.

The intended audience is players who enjoy expressive FPS movement, spatial problem solving, short retries, and improving their own completion times. This is a product hypothesis to test, not a market-size finding.

## Design pillars

- **Death changes the route.** Placement, approach, and movement matter more than raw corpse count.
- **The assault continues.** Respawns preserve the battlefield, threats, objectives, and musical momentum.
- **Construction can be trusted.** A settled body remains reliable cover and traversable terrain.
- **Realism serves readability.** Materials, lighting, sound, and impacts feel physical; control and camera remain responsive.
- **Mastery includes sacrifice.** The main result is completion time. Body count is context, with no reward for farming deaths.

## Proposed release scope

| Item | Baseline |
| --- | --- |
| Platform | Windows PC, offline single player |
| Mode | One continuous fortress siege, fresh run and resumable run |
| Content | Three connected sectors, introduced in [level design](design/level-design.md) |
| Playtime hypothesis | 15–30 minutes for a first full clear, with faster repeat clears |
| Arsenal | One automatic rifle, reload, aim, and temporary defensive suppression |
| Movement | Walk sprint crouch jump short dive and contextual low vault |
| Threats | Two fixed emplacement variants sharing the same state machine |
| Progression | Captured checkpoints and learned routes; no stat upgrades |
| Replay | Local best time per ruleset and a full reset option |
| Presentation | Photorealistic industrial fortress, restrained horror, dry administrative delivery |
| Settings | Rebinding, controller support, camera comfort, subtitles, reduced gore, audio and display settings |
| Language | English UI and subtitles, localization-ready string IDs |
| Distribution | Storefront undecided; packaged build and release materials planned |

The baseline excludes multiplayer, procedural maps, terrain excavation, destructible corpse structures, movable corpse editing, roaming enemy squads, a large arsenal, meta progression, online leaderboards, console ports, and a large narrative campaign. These are scope boundaries, not technical impossibility claims. A pitch or reference to moats does not require a terrain destruction system in this release; the level uses existing ditches and body-built crossings.

## Success and stop conditions

M1 must demonstrate a useful body within one to three placements, understandable death feedback, reliable traversal, and control returning within the proposed respawn budget. In a small observed playtest, at least six of eight new players should intentionally place and exploit a body without a facilitator explaining a solution. These are directional gate criteria, not statistical market validation.

M2 must integrate one representative sector with final-intent presentation, saving, settings, and a corpse stress test. If readability depends on lengthy death cameras, route geometry cannot remain stable, or performance requires erasing useful cover, stop content expansion and revise the central mechanic.

## Business and ownership decisions

The product owner must choose the commercial title, distribution channel, price model, release territories, and funding envelope. No price, revenue forecast, trademark availability, or release date is asserted here. The producer should obtain actual labor and vendor rates before converting the [resource model](production/production-plan.md) into a budget.
