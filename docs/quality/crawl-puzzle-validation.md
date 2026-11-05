# Low crawl and terrain puzzle validation

5 November 2026. REQ-030 / MS-014 / TC-034. Windows Development package, Unreal 5.8.4 CL58902052. Source and binary hashes are recorded in [the build manifest](builds/crawl-puzzle-2026-11-05.json).

Z toggles a 36 cm capsule with 30 cm eyes and 85 cm/s crawl. The human has a prone procedural movement pose. The low gun has a 30 cm pivot. Recipe 1 / seed 71031 supplies real hilly terrain collision and 18 initial human bodies, restored from physics-baked rests. Terrain has 9,633 vertices and 18,816 triangles; the same triangle mesh renders and collides. The central pile remains only in legacy comparison mode.

| Gate | Executed result |
| --- | --- |
| Pure C++ rules | 145 checks passed; initial crawl/terrain/layout failures observed before implementation; route-clearance regression failed before widening the route |
| Python tools | 37 tests passed; three evidence-rejection tests observed red before implementation |
| Planning consistency | Passed, structured requirements/backlog/tuning and human-readable artifacts aligned |
| Editor, runtime and packaged cook/stage | Passed |
| TC-034 rendered package | 18 checks passed on each of two fresh launches; identical full initial pose/collision hash 126475818 |
| Native character traversal | Seven waypoints completed without teleporting between waypoints; crossed actual hills with capsule centre reaching approximately 134 cm |
| Single-body cover | One actual baked human intercepted a real turret trace for the crawler; the standing player at the same shelter was hit; exposed crawler also took damage |
| Clearance and controls | Z input, ceiling rejection, standing after clearance, crawl speed, jump/sprint restriction and prone skeletal pose passed |
| Persistence and death | Retry and new deaths preserve all initial poses; actual prone avatar transfers to corpse and returns to crawl at protected spawn |
| Legacy MetaHuman gameplay | 47 checks, two deaths, rendered captures and audible unclipped output passed |
| Legacy primitive gameplay | 43 checks, two deaths, rendered captures and audible unclipped output passed |
| Content packaging | Passed with the temporary loose pose file removed; UFS-packaged data loaded successfully |

Initial bake: all 18 bodies physically moved and committed after supported rest, at simulation ages 2.02–3.28 seconds. Normal play restores those accepted poses immediately without running that solve. Missing pose content was tested: startup logged the required path and bake instruction and exited. The evidence verifier rejects logged errors even if Unreal returns exit code zero. Malformed/version-mismatch branches are implemented but were not separately exercised in a packaged run.

The first traversal route grazed an existing corpse and snagged in one run. The corrected route provides over 180 cm clearance from authored placement centres and completed twice against actual baked collision. An older regression check assumed every native collision capsule points along the human spine; a flat native rest violated that assumption. The human fixture now measures actual neck-to-pelvis collapse while retaining the ground-height, supported-rest and immutable-pose checks. Primitive checks retain their original capsule-axis measurement.

Reviewed 1280x720 packaged captures: [start and hills](evidence/crawl-puzzle-2026-11-05/puzzle-start.png), [single-body crawl shelter](evidence/crawl-puzzle-2026-11-05/puzzle-single-cover.png), [route endpoint](evidence/crawl-puzzle-2026-11-05/puzzle-route-end.png), and [layout overview](evidence/crawl-puzzle-2026-11-05/puzzle-overview.png). The shelter capture follows the standing-hit comparison, hence its 85 health reading.

Limits: traversal is tested separately from automatic combat pacing; this is an initial puzzle layout requiring playtest balance, not a demonstrated optimal live-fire solution. Crawl uses a compact upright capsule and procedural FK pose, not full limb collision or authored terrain-contact IK. Fitted garments remain rough and may intersect at extreme poses. Initial poses are deterministic; subsequent Chaos-driven player deaths are not promised bit-identical. Player deaths persist in the loaded scene, not across quitting. No minimum-hardware or frame-tail performance gate is claimed. Body restore completed during startup with no visible settling wait; full cold-start and performance characterization remain future work.
