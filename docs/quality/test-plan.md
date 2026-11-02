# Meat Shield verification plan

These are planned tests, not executed game results. Verify core mechanics in small repeatable fixtures, then repeat the relevant cases in packaged representative and full-fortress builds. Each result records the build, content and rules versions, hardware manifest, case ID, observed result, and evidence link.

## Gate coverage

M0 verifies bootstrap and binary workflow. M1 verifies movement, placement, instant retry, combat, capture, and observations of intent. M2 adds bounded stress, saving, recovery, settings, and final-intent readability. M3 verifies the complete fortress and all baseline flows. M4 repeats the complete matrix and supported upgrade cases. M5 tests the exact candidate on clean machines and confirms recovery and handover.

Use automated fixtures for idempotency, state transitions, proxy queries, snapshot round trips, and numerical validation. Use manual or instrumented play for movement feel, camera comfort, art alignment, learning, and complete controller flow. A scripted completion is not evidence that players understand the premise.

## Cases

| ID | Requirements | Setup and action | Expected result |
| --- | --- | --- | --- |
| TC-001 | REQ-001 | Traverse flat ground walls stairs a one-body step and three-body pile with each movement action on keyboard and controller | Responsive actions, no snag or launch, no vault through blocked destination |
| TC-002 | REQ-002 | Record at least 200 deaths across empty and full-budget scenes, with capture effects and saves active | Current ragdoll-camera slice returns control within 1.2 s; no map reload; later full-gate benchmark follows the accepted death presentation |
| TC-003 | REQ-003 | Commit a body, die nearby repeatedly, shoot it, fill visual pool, cross checkpoint and return | Authoritative transforms support and occlusion remain unchanged |
| TC-004 | REQ-004 | Clean courtyard, compare zero one two and three intentional placements; observe eight new players | Placement changes a demonstrated route; at least six deliberately place and exploit a body without a supplied solution |
| TC-005 | REQ-005 | Die at wall edges over pits on ledges and on existing bodies, including invalid endpoints | Supported bounded anchors only; no wall crossing floating support or distant teleport |
| TC-006 | REQ-006 | Fire and sight rays across proxy centers edges and visible gaps from standing and crouched positions | First-hit occlusion agrees across weapon threat and stable geometry; no invisible shield |
| TC-007 | REQ-007 | Die during telegraph burst suppression reload and gate animation | Continuous world timers and resource state; only controlled pawn resets |
| TC-008 | REQ-008 | Complete capture, duplicate request, take lethal hit on completion update, then die one update after valid capture | Same-update lethal cancels unfinished capture; valid capture applies threat gate spawn and objective once |
| TC-009 | REQ-009 | Hit sensor three times in window, continue shooting while suppressed, wait and reach control | Four-second suppression cannot be extended by farming hits; recovery telegraphs; control disables permanently |
| TC-010 | REQ-010 | Capture sector one, build in sector two, reset sector two, cancel reset, then fresh-run confirmation | Reset restores entry state only for current sector, retains all-run time and deployments, marks assisted; cancellation changes nothing |
| TC-011 | REQ-011 | Run pause focus-loss load resume reset and victory sequences; compare local records | Active timer excludes pause/load; class never reverts; records compare matching rules content and class; victory once |
| TC-012 | REQ-012 | Save with bodies partial reload suppression and captured gates, close and resume | Restore authoritative geometry objective state relative timers weapon and checkpoint; spawn healthy at captured point |
| TC-013 | REQ-013 | Fill a tile, add 100 deaths there, then place in a different tile; cross tile boundary with a two-shape body | Atomic tile reservation; no cap overflow; full patch cannot steal capacity elsewhere; existing cover remains |
| TC-014 | REQ-014 | Observe latest valid and rejected body feedback from normal and low-quality modes with color removed | Pattern icon or text communicates outcome; no feedback delays control or implies nonexistent support |
| TC-015 | REQ-015 | Review representative scene in full and reduced gore; die through each music intensity and threat state | Stable silhouette and convincing materials; no musical restart or missing essential cue; cosmetic effects stay bounded |
| TC-016 | REQ-016 | Complete menu-to-victory controller-only with rebound controls large text reduced shake blur and gore | All required actions available, focus visible, prompts correct, no cue depends only on color or sound |
| TC-017 | REQ-017 | Fresh full siege through all three sectors, inspect final route and restart | Three controls two gates two threat variants and complete victory flow; no hidden placeholder bypass |
| TC-018 | REQ-018 | Capture a life death body commit later body use rejected placement respawn capture and finish in local events | Schema-conforming versioned events link the right IDs, order and timestamps; use events are debounced and not awarded points |
| TC-019 | REQ-019 | Check launcher configuration failures, dry run, subprocess exit and version evidence; run Blender background Python, Unreal commandlet and reversible localhost MCP operation; another developer builds packages and launches an exact checkout | Invalid setup fails with actionable context, caches and secrets remain local, rendered and headless modes stay distinct; reproducible recipe and binary workflow work without untracked author files |
| TC-020 | REQ-020 | Install exact candidate on clean machine, make save upgrade and rehearse compatible rollback or backup restore | Reviewed package identity matches test record; support credits known issues and recovery instructions usable |
| TC-021 | REQ-021 | Import negative durations NaN transforms unknown IDs over-cap shapes bad references and mismatched rules | Validation rejects unsafe data with actionable context before gameplay or load commits |
| TC-022 | REQ-022 | Review each gate record against required artifacts exact build and outstanding defects | Decision uses actual linked evidence, named owners and scope; no unchecked plan is counted as a passed test |
| TC-023 | REQ-023 | Die with empty magazine full magazine and partial reload; hold fire across death; drain all ammo and resupply | Death never adds ammo skips reload or bypasses cadence; current assault-point resupply prevents softlock |
| TC-024 | REQ-024 | Change all settings start a new run restart app reset one settings group and enable pseudo-localization | Settings persist independently; group reset is scoped; mandatory text remains readable |
| TC-025 | REQ-003 REQ-005 | Repeat the same dive velocity position and stance at least 30 times from clean fixture state, then stand on result | Placement within 0.15 m and consistent support; visual impact variation does not move useful geometry |
| TC-026 | REQ-013 | Empty half-full full-budget 10-minute traces plus 1000-death soak and final two saturated 10-minute windows | p95 frame at most 16.7 ms p99 at most 25 ms on benchmark; memory plateaus, growth above 5% investigated; useful cover retained |
| TC-027 | REQ-012 | Interrupt writes corrupt newest generation load older supported and unknown newer fixtures exhaust disk space | Previous valid generation recoverable; migrations explicit; newer or failed saves not silently overwritten |
| TC-028 | REQ-002 REQ-016 | Die while holding inputs, open menu during possession, switch device and lose/regain focus | No stuck fire repeated interaction accidental capture or extra pawn; focus loss pauses offline siege |
| TC-029 | REQ-025 | Build the turret-approach slice, exercise safe spawn, walk/sprint/jump, exposed approach, cover, lethal reset, mute and quit in the packaged Windows game | Visible procedural locomotion/aim/recoil/hit animations and audible effects; first blocking trace governs damage; bounded burst and one reset per life; standalone launch works |
| TC-030 | REQ-002 REQ-003 REQ-026 | Kill the player twice, inject duplicate lethal requests and R during death, inspect pull-back/return, actual falling physics, frozen poses and old-body collision, then retry again | One corpse per lethal life, all earlier corpses retained and stable, visible fall, protected first-person return within 1.2 s, one camera completion and no temporary camera leak |
| TC-031 | REQ-001 REQ-006 REQ-026 REQ-027 | Crouch/release under open and blocked ceilings, observe full collapse before commitment, fire through active/fixed meat and compare health and impact point | Native crouch/clearance, stance-aware aim, no penetration, fixed proxies only after supported quiet pose, earlier cover stays unchanged; off-centre shot produces angular velocity, fixed poses resist impacts, all 42 central bodies start within 2 s and form a settled pile within 90 s; two-body crouch cover blocks the real low muzzle ray, standing remains exposed, and retries preserve the pile |

| TC-032 | REQ-028 | Assemble optimized MetaHumans; compare expression close-ups, 1/10/42-body piles and matched procedural ground/lighting; package without editor/cloud | Real human visuals, varied readable expressions, attached faces, stable cover, deterministic recipes, measured cost and actionable missing-asset errors |
| TC-033 | REQ-029 | Bilateral arm physics; continuous live/dead human pose and outfit; seeded fitted uniforms; standing/crouched transfer; packaged renders and regression | Independent limp arms, matching evaluated LOD pose and life seed, three palettes, attached/frozen garments, retained cover and no packaged runtime errors |

## Matrix and evidence requirements

At M2, use the nominated performance PC at 1080p and record the actual preset and drivers. At M4, add a lower-spec candidate and a higher-spec reference machine, supported window modes and aspect ratios, keyboard/mouse and controller, minimum and large text, full and reduced gore, and camera comfort settings. These machines determine published specifications; no hardware requirement is invented here.

Save fixtures cover empty, populated, full-tile, multi-sector, sector-entry reset, interrupted latest write, old supported version, and unknown newer version. Keep fixtures in source control after bootstrap with content manifests and expected results. Synthetic fixtures must not overwrite a developer's actual play save.

Gate evidence belongs in the project tracker or a reviewed results directory created during implementation, with recordings and traces in the chosen artifact store. Links must remain accessible to the receiving team. A bug should include the relevant case and [bug template](../../.github/ISSUE_TEMPLATE/bug_report.md).

## Exit criteria

No critical or high defect may remain at M4 or release. A critical defect includes crash, corrupt or lost progress, irreversible required-route failure, or repeated spawn death. A high defect blocks a baseline feature or supported input path. Lower defects need an owner and explicit disposition. A performance failure is a gate failure until the target, measured hardware floor, or scope is deliberately revised with evidence.

This repository's [planning validator](../../tools/validate_planning.py) verifies references and fixture structure. It does not execute these gameplay cases or replace a test result.
