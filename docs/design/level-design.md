# Meat Shield level and content plan

Build one fortress from a reusable industrial kit. The release baseline contains three connected sectors, three forward assault points including the starting point, three control interactions, two gates, and a final overlook. The courtyard prototype becomes sector one rather than a throwaway fourth level.

## Fortress progression

```mermaid
flowchart LR
    S[Protected starting alcove] --> A[Sector 1 Broken approach]
    A --> G1[Control 1 and gate]
    G1 --> B[Sector 2 Service ditch]
    B --> G2[Control 2 and gate]
    G2 --> C[Sector 3 Inner breach]
    C --> V[Final control and overlook]
```

The diagram expresses order, not physical scale. Each sector has an accessible service return path to its current assault point, a clearly visible destination, and protected space to read the next threat. Completed sectors retain their bodies and gates; no backtracking is required to finish.

## Sector specifications

| Sector | Spatial lesson | Setup and intended solution | Threat and payoff |
| --- | --- | --- | --- |
| 1 Broken approach | A body completes existing shelter | Start with a roughly 24 m approach. A low damaged wall leaves a firing gap; one useful body plugs it and a second can improve crouched shelter. The control is reachable after a short final push. | Single frontal emplacement. Three rifle hits create a crossing window; capture disables it and opens the first gate. |
| 2 Service ditch | Added height and a filled interruption | Start with a shallow dry ditch roughly 0.6 m deep and an exit above unaided vault reach. One to three supported placements create a step and fill a short interruption. Provide a longer exposure-heavy alternate path for recovery. | Angled emplacement covers the exit. Capture advances the assault point and opens the second gate. |
| 3 Inner breach | Combine shelter height and timing | Two angles share a short approach. Place bodies at two distinct locations to break sightlines, then use the rifle window to reach the final control. | One of each fixed threat variant. Final capture disables both and opens a safe overlook. |

Dimensions are blockout seeds, not final measurements. Tune geometry against the actual capsule, step, jump, vault, camera height, and corpse proxy. Avoid a deep unsupported bridge puzzle, which the baseline construction model cannot solve.

## Prototype encounter

The first playable has one protected spawn, one rifle, one frontal emplacement, the damaged wall, one small height opportunity, and a single capture control. Aim for a successful siege lasting 1–3 minutes and typical experimental lives of 5–15 seconds. Neither is a minimum forced wait.

Record three route demonstrations from a clean state: accidental beneficial death, deliberate placement followed by successful use, and a skilled approach that combines shooting with a precise placement. Compare against a naive repeated straight charge. A fully saturated spawn-side pile must not win the encounter on its own.

An expert deathless route may exist if it does not dominate the intended experience. Do not make a mandatory sacrifice counter or an invisible body-count lock. Geometry and threat timing create the need to alter the route.

## Blockout review checklist

- Trace standing and crouched muzzle and eye lines against the actual proxy, including gaps between shapes.
- Verify that every intended placement is supported, inside a construction volume, and reachable under a suppressible threat.
- Show the objective and at least the first useful placement from a protected observation position.
- Keep spawn-to-first-decision travel below 3 seconds in the first sector; later captures eliminate solved return travel.
- Test one, two, and three bodies separately. Establish the useful threshold without requiring an arbitrary pile size.
- Check that a suppression window from the protected spawn does not allow a straight sprint through the entire encounter and capture. Tune sightlines, sensor access, and route geometry before adding a forced-death lock.
- Leave room to turn, vault, and recover near an overfilled opening. Test the sector reset from every capture state.
- Mark construction coverage, tile capacity, no-build safety zones, kill volumes, and safe spawn backups in the editor.
- Test widest FOV, reduced gore, low visual quality, and controller traversal before dressing the space.
- Keep decorative meshes from obstructing gameplay rays or making a blocked ray appear visually clear.

## Authoring and signoff

Each sector submission includes an overhead blockout screenshot, annotated threat arcs, construction volume bounds, route timings, before-and-after placement captures, checkpoint transaction IDs, and the content revision. The designer owns route intent; engineering validates collision and performance; art validates silhouette and lighting. A sector cannot move to dressing while its intended route depends on unstable or invisible collision.

Asset families and quantities are tracked in [the asset register](../../planning/assets.json). The three sectors share the fortress kit, player body, rifle, threat rigs, UI, effects, and music system. New families require a scope change and resource forecast.
