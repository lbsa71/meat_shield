# Meat Shield implementation backlog

Generated from `planning/backlog.json` and `planning/milestones.json`. Edit those files, then run `python tools/validate_planning.py --write-backlog`.

Task status is maintained in the JSON; partial slice evidence does not complete a task. Estimates are person-days, not elapsed days. Low and high values are scenarios, not confidence bounds. Dependencies indicate deliverable prerequisites; the producer must schedule named capacity and incremental integration before committing dates.

See [requirements](../../planning/requirements.json), [production plan](production-plan.md), and [test plan](../quality/test-plan.md).

## Capacity and effort

| Role | Baseline capacity | Likely named work | Available for iteration |
| --- | ---: | ---: | ---: |
| Engineering | 240 | 139 | 101 |
| Design | 120 | 79 | 41 |
| Art | 120 | 82 | 38 |
| Production | 30 | 12 | 18 |
| QA | 50 | 50 | 0 |
| Audio | 15 | 15 | 0 |

Named work: **377 likely days**, **602.5 days at summed upper bounds**. QA and audio have no unassigned baseline capacity; book extra specialist time if their upper scenarios materialize.

## M0 Kickoff and feasibility

Relative weeks 1â€“1.

- Assign owners and confirm platform scope and engine patch.
- Package from clean checkout and record benchmark and binary workflow.

| ID | Task | Role | Likely days | Dependencies |
| --- | --- | --- | ---: | --- |
| MS-001 | Environment and packaged bootstrap | Engineering | 3 | None |
| MS-002 | Baseline and ownership review | Production | 1 | None |
| MS-003 | Prototype blockout and route fixture | Design | 3 | None |
| MS-004 | Asset scale silhouette and storage fixture | Art | 3 | None |

### MS-001 Environment and packaged bootstrap

Pinned environment and minimal packaged FPS project.

Requirements: REQ-019, REQ-025. Estimate: 2 / 3 / 4.75 days, low / likely / high.

- [ ] Clean checkout packages and launches.
- [ ] Benchmark manifest and binary round trip are recorded.
- [ ] Pinned editor launcher rejects invalid inputs and versions without launching, isolates local caches and reports actual subprocess evidence.
- [ ] Blender background Python, Unreal Python commandlet and localhost MCP discovery with a reversible editor operation pass; rendered checks use the real GPU.
- [ ] First turret approach slice packages and launches; see TC-029 and its evidence without marking later FPS/body acceptance complete.

### MS-002 Baseline and ownership review

M0 decision and staffing record.

Requirements: REQ-019, REQ-022. Estimate: 0.75 / 1 / 1.5 days, low / likely / high.

- [ ] Named leads accept or amend defaults.
- [ ] Actual rates and availability requested; no release date implied.

### MS-003 Prototype blockout and route fixture

Annotated courtyard blockout.

Requirements: REQ-004, REQ-005. Estimate: 2 / 3 / 4.75 days, low / likely / high.

- [ ] Show one three-body route and protected spawn.
- [ ] Record threat arcs and initial movement dimensions.

### MS-004 Asset scale silhouette and storage fixture

Body and wall proxy import fixture.

Requirements: REQ-015, REQ-019. Estimate: 2 / 3 / 4.75 days, low / likely / high.

- [ ] Scale pivot and proxy silhouette agree.
- [ ] Source provenance and binary locking workflow reviewed.

## M1 Core prototype

Relative weeks 2â€“4.

- Demonstrate a 1–3 minute siege and useful 1–3 body route.
- Meet placement and respawn fixture targets; observe deliberate use in at least six of eight new players.
- Record go revise or stop before content expansion.

| ID | Task | Role | Likely days | Dependencies |
| --- | --- | --- | ---: | --- |
| MS-005 | Movement and traversal controller | Engineering | 5 | MS-001 |
| MS-006 | Death transaction and pawn replacement | Engineering | 4 | MS-001 |
| MS-007 | Movement-informed stable body proxies | Engineering | 6 | MS-003, MS-006 |
| MS-008 | Rifle resources and suppressible emplacement | Engineering | 5 | MS-005 |
| MS-009 | Respawn event timing and spawn safety | Engineering | 2 | MS-006, MS-007 |
| MS-010 | Courtyard encounter and capture teaching | Design | 7 | MS-003 |
| MS-011 | Body hardening and latest-body feedback | Art | 6 | MS-004 |
| MS-012 | Observed prototype placement playtest | Design | 3 | MS-007, MS-008, MS-010 |
| MS-013 | Prototype mechanics verification | QA | 3 | MS-008, MS-009, MS-010, MS-011 |
| MS-014 | Prototype gate review | Production | 1 | MS-012, MS-013 |

### MS-005 Movement and traversal controller

Movement fixture and controller.

Requirements: REQ-001, REQ-005, REQ-025, REQ-027. Estimate: 3.25 / 5 / 8 days, low / likely / high.

- [ ] Implement baseline actions and capsule sweeps.
- [ ] Demonstrate supported pile traversal and clearance failures.

### MS-006 Death transaction and pawn replacement

Persistent siege coordinator.

Requirements: REQ-002, REQ-007, REQ-008, REQ-026. Estimate: 2.5 / 4 / 6.5 days, low / likely / high.

- [ ] Duplicate lethal hits emit one death and one pawn.
- [ ] Apply specified lethal and capture ordering.

### MS-007 Movement-informed stable body proxies

Construction fixture and placement subsystem.

Requirements: REQ-003, REQ-005, REQ-006, REQ-013, REQ-026, REQ-027, REQ-028. Estimate: 4 / 6 / 9.5 days, low / likely / high.

- [ ] Use bounded supported anchors and immutable proxies.
- [ ] Repeated placement and cover rays pass fixture targets.

### MS-008 Rifle resources and suppressible emplacement

Weapon and first fixed threat.

Requirements: REQ-009, REQ-023, REQ-007, REQ-025, REQ-027, REQ-028. Estimate: 3.25 / 5 / 8 days, low / likely / high.

- [ ] Resources and reload survive pawn death.
- [ ] Suppression recovers continuously and controls disable permanently.

### MS-009 Respawn event timing and spawn safety

Timing events and spawn fallback fixture.

Requirements: REQ-002, REQ-014, REQ-018, REQ-026. Estimate: 1.25 / 2 / 3.25 days, low / likely / high.

- [ ] Capture death-to-control distribution and reason codes.
- [ ] Protected spawn does not enter a lethal loop.

### MS-010 Courtyard encounter and capture teaching

One complete prototype encounter.

Requirements: REQ-004, REQ-008, REQ-009. Estimate: 4.5 / 7 / 11.25 days, low / likely / high.

- [ ] Useful placement and final breach take place in a short siege.
- [ ] Naive repeated charging is compared with a planned route.

### MS-011 Body hardening and latest-body feedback

Prototype silhouette and feedback.

Requirements: REQ-014, REQ-015, REQ-026. Estimate: 4 / 6 / 9.5 days, low / likely / high.

- [ ] Visual result aligns with useful proxy.
- [ ] Feedback uses pattern and does not delay respawn.

### MS-012 Observed prototype placement playtest

Eight-player prototype report.

Requirements: REQ-004, REQ-018. Estimate: 2 / 3 / 4.75 days, low / likely / high.

- [ ] Record deliberate placement and later exploitation.
- [ ] Compare matched routes and document go revise or stop recommendation.

### MS-013 Prototype mechanics verification

M1 test evidence.

Requirements: REQ-001, REQ-002, REQ-003, REQ-005, REQ-006, REQ-007, REQ-008, REQ-009, REQ-014, REQ-023. Estimate: 2 / 3 / 4.75 days, low / likely / high.

- [ ] Run prototype fixture cases with build and hardware IDs.
- [ ] Log defects and verify fixed-pawn continuous-world behavior.

### MS-014 Prototype gate review

M1 review and forecast.

Requirements: REQ-022, REQ-028, REQ-029, REQ-030. Estimate: 0.75 / 1 / 1.5 days, low / likely / high.

- [ ] Review observed intent and technical targets.
- [ ] Authorize next phase revise or stop with recorded rationale.
- [ ] TC-034: low crawl, single-body cover and deterministic collision-terrain puzzle; packaged route and layout replay evidence.

## M2 Representative vertical slice

Relative weeks 5â€“8.

- One sector integrates final-intent art audio settings save and reset.
- Meet representative performance and full construction stress targets.
- Re-estimate production using observed throughput and fix scope.

| ID | Task | Role | Likely days | Dependencies |
| --- | --- | --- | ---: | --- |
| MS-015 | Tile budgets saturation and pooled visuals | Engineering | 8 | MS-007, MS-014 |
| MS-016 | Versioned snapshots recovery and sector reset | Engineering | 8 | MS-006, MS-014 |
| MS-017 | Settings rebinding and controller foundation | Engineering | 5 | MS-005, MS-014 |
| MS-018 | Representative sector packaged integration | Engineering | 4 | MS-015, MS-016, MS-017 |
| MS-019 | Slice benchmark and stress optimization | Engineering | 3 | MS-018 |
| MS-020 | Representative sector route refinement | Design | 10 | MS-014 |
| MS-021 | Final-intent body weapon and fortress kit | Art | 16 | MS-011, MS-014 |
| MS-022 | Threat weapon and music slice | Audio | 4 | MS-014 |
| MS-023 | Core screens and accessible HUD | Art | 3 | MS-014 |
| MS-024 | Integrated slice playtest | Design | 2 | MS-018, MS-020, MS-021, MS-022, MS-023 |
| MS-025 | Slice save accessibility and stress verification | QA | 5 | MS-018 |
| MS-026 | Slice scope and estimate gate | Production | 1 | MS-019, MS-024, MS-025 |

### MS-015 Tile budgets saturation and pooled visuals

Bounded construction and stress scenario.

Requirements: REQ-003, REQ-013, REQ-021. Estimate: 5.25 / 8 / 12.75 days, low / likely / high.

- [ ] Reserve multi-tile quotas atomically.
- [ ] 1000 deaths preserve cover and plateau memory.

### MS-016 Versioned snapshots recovery and sector reset

Save service and migration fixtures.

Requirements: REQ-010, REQ-011, REQ-012, REQ-021. Estimate: 5.25 / 8 / 12.75 days, low / likely / high.

- [ ] Round trip world relative timers weapon and reset baselines.
- [ ] Interrupted write and corrupt newest generation preserve previous save.

### MS-017 Settings rebinding and controller foundation

Persistent input and presentation settings.

Requirements: REQ-016, REQ-024. Estimate: 3.25 / 5 / 8 days, low / likely / high.

- [ ] Core screens and siege work with controller and rebound controls.
- [ ] Comfort gore subtitles and focus loss behave correctly.

### MS-018 Representative sector packaged integration

Slice candidate build.

Requirements: REQ-008, REQ-010, REQ-012, REQ-017. Estimate: 2.5 / 4 / 6.5 days, low / likely / high.

- [ ] Integrate capture save reset and resource flows.
- [ ] No placeholder state bypasses a required transaction.

### MS-019 Slice benchmark and stress optimization

Performance traces and benchmark report.

Requirements: REQ-002, REQ-013. Estimate: 2 / 3 / 4.75 days, low / likely / high.

- [ ] Run empty half-full and full construction scenes.
- [ ] Report p95 frame and respawn p99 frame and memory windows.

### MS-020 Representative sector route refinement

Slice level and authoring specification.

Requirements: REQ-004, REQ-005, REQ-017. Estimate: 6.5 / 10 / 16 days, low / likely / high.

- [ ] Validate exact movement and body thresholds.
- [ ] Record construction volume coverage and alternative recovery route.

### MS-021 Final-intent body weapon and fortress kit

Slice asset families.

Requirements: REQ-015, REQ-017. Estimate: 10.5 / 16 / 25.5 days, low / likely / high.

- [ ] Review final-intent materials scale silhouette and LOD.
- [ ] Asset provenance and cost captured in representative scene.

### MS-022 Threat weapon and music slice

Slice cue set and adaptive music sketch.

Requirements: REQ-015, REQ-016. Estimate: 2.5 / 4 / 6.5 days, low / likely / high.

- [ ] State cues have visual counterparts.
- [ ] Music continues through death and supports breach.

### MS-023 Core screens and accessible HUD

Menu HUD pause results and settings layouts.

Requirements: REQ-014, REQ-016, REQ-024. Estimate: 2 / 3 / 4.75 days, low / likely / high.

- [ ] Core flow fits text scaling and pseudo-localized labels.
- [ ] Save error and reset consequences are legible.

### MS-024 Integrated slice playtest

Slice observation report.

Requirements: REQ-004, REQ-015, REQ-016, REQ-018. Estimate: 1.25 / 2 / 3.25 days, low / likely / high.

- [ ] Participants explain useful bodies and perceive threats.
- [ ] Record comfort controller and low-detail findings.

### MS-025 Slice save accessibility and stress verification

M2 regression evidence.

Requirements: REQ-010, REQ-011, REQ-012, REQ-013, REQ-015, REQ-016, REQ-021, REQ-024. Estimate: 3.25 / 5 / 8 days, low / likely / high.

- [ ] Verify save corruption reset input and saturation fixtures.
- [ ] Check presentation against final slice assets before signoff.

### MS-026 Slice scope and estimate gate

M2 approved scope and resource forecast.

Requirements: REQ-022. Estimate: 0.75 / 1 / 1.5 days, low / likely / high.

- [ ] Use observed throughput and measured performance.
- [ ] Accept or replace proposed architecture before content expansion.

## M3 Complete fortress alpha

Relative weeks 9â€“16.

- All three sectors work start to victory in a packaged build.
- All baseline features assets and support flows are integrated.
- No required system remains represented only by a placeholder.

| ID | Task | Role | Likely days | Dependencies |
| --- | --- | --- | ---: | --- |
| MS-027 | Service ditch and inner breach sectors | Design | 25 | MS-026 |
| MS-028 | Second emplacement and multi-threat controls | Engineering | 10 | MS-026 |
| MS-029 | Full fortress construction and save integration | Engineering | 12 | MS-028 |
| MS-030 | Production environment character and animation set | Art | 30 | MS-026 |
| MS-031 | Production UI and localized string integration | Art | 7 | MS-023, MS-026 |
| MS-032 | Controller comfort and settings polish | Engineering | 8 | MS-017, MS-026 |
| MS-033 | Completion local records and run classification | Engineering | 6 | MS-016, MS-026 |
| MS-034 | Full fortress alpha regression | QA | 12 | MS-027, MS-029, MS-032, MS-033 |
| MS-035 | Production effects adaptive suite and subtitles | Audio | 6 | MS-022, MS-026 |
| MS-036 | Whole fortress balance and teaching | Design | 10 | MS-027 |
| MS-037 | Packaged smoke automation and build retention | Engineering | 10 | MS-029, MS-033 |
| MS-038 | Alpha completion review | Production | 2 | MS-030, MS-031, MS-034, MS-035, MS-036, MS-037 |

### MS-027 Service ditch and inner breach sectors

Remaining two playable sectors.

Requirements: REQ-004, REQ-017. Estimate: 16.25 / 25 / 40 days, low / likely / high.

- [ ] Implement height crossing and combined placement lessons.
- [ ] Include controls safe spawns return paths and route evidence.

### MS-028 Second emplacement and multi-threat controls

Second fixed threat variant and capture mapping.

Requirements: REQ-008, REQ-009, REQ-017. Estimate: 6.5 / 10 / 16 days, low / likely / high.

- [ ] Share tested threat state machine.
- [ ] All associated threats disable once and remain disabled on resume.

### MS-029 Full fortress construction and save integration

Three-sector state integration.

Requirements: REQ-003, REQ-010, REQ-012, REQ-013, REQ-017. Estimate: 7.75 / 12 / 19.25 days, low / likely / high.

- [ ] Retain earlier geometry through every transition.
- [ ] Reset current sector without losing captured earlier sectors.

### MS-030 Production environment character and animation set

Complete baseline visual content.

Requirements: REQ-015, REQ-017, REQ-028, REQ-029. Estimate: 19.5 / 30 / 48 days, low / likely / high.

- [ ] Populate three sectors with reusable approved asset families.
- [ ] No collision-authoritative details exist only in decorative art.

### MS-031 Production UI and localized string integration

Final screens icons and string catalog.

Requirements: REQ-016, REQ-024. Estimate: 4.5 / 7 / 11.25 days, low / likely / high.

- [ ] All required states have final layouts.
- [ ] Pseudo-localized strings fit with text scaling.

### MS-032 Controller comfort and settings polish

Final input and presentation options.

Requirements: REQ-001, REQ-016, REQ-024. Estimate: 5.25 / 8 / 12.75 days, low / likely / high.

- [ ] Test device switching toggle modes deadzones and focus.
- [ ] Settings persist independently of active run.

### MS-033 Completion local records and run classification

Victory route inspection and local records.

Requirements: REQ-011, REQ-017. Estimate: 4 / 6 / 9.5 days, low / likely / high.

- [ ] Victory commits once.
- [ ] Resume and sector reset cannot improve uninterrupted records.

### MS-034 Full fortress alpha regression

M3 packaged regression and defect list.

Requirements: REQ-001, REQ-003, REQ-006, REQ-008, REQ-010, REQ-012, REQ-016, REQ-017. Estimate: 7.75 / 12 / 19.25 days, low / likely / high.

- [ ] Finish all sectors on both input devices.
- [ ] Test saves and reset at every checkpoint including same-frame deaths.

### MS-035 Production effects adaptive suite and subtitles

Final baseline audio families.

Requirements: REQ-015, REQ-016, REQ-017. Estimate: 4 / 6 / 9.5 days, low / likely / high.

- [ ] Complete threat weapon body environment and breach cues.
- [ ] Essential cues remain legible at low dynamic range.

### MS-036 Whole fortress balance and teaching

Balance revision and full-run playtest report.

Requirements: REQ-004, REQ-009, REQ-017, REQ-018. Estimate: 6.5 / 10 / 16 days, low / likely / high.

- [ ] Compare early and practiced routes.
- [ ] Investigate body farming ammo loops and unreadable placements.

### MS-037 Packaged smoke automation and build retention

Repeatable candidate pipeline and evidence exports.

Requirements: REQ-019, REQ-021, REQ-022. Estimate: 6.5 / 10 / 16 days, low / likely / high.

- [ ] Clean checkout produces versioned packaged artifact.
- [ ] Smoke covers launch capture save resume victory and exit.

### MS-038 Alpha completion review

M3 gate and beta defect plan.

Requirements: REQ-022. Estimate: 1.25 / 2 / 3.25 days, low / likely / high.

- [ ] Verify complete content inventory and required features.
- [ ] Reforecast defects and cut optional polish if necessary.

## M4 Beta and content lock

Relative weeks 17â€“20.

- Full regression compatibility accessibility and benchmark pass.
- No open critical or high severity defects; content locked.
- Storefront commercial decisions and release ownership resolved.

| ID | Task | Role | Likely days | Dependencies |
| --- | --- | --- | ---: | --- |
| MS-039 | Beta crash save compatibility and performance fixes | Engineering | 20 | MS-038 |
| MS-040 | Beta balance onboarding and route review | Design | 10 | MS-038 |
| MS-041 | Accessibility and pseudo-localization review | Design | 4 | MS-038 |
| MS-042 | Beta visual polish and cost reduction | Art | 12 | MS-038 |
| MS-043 | Final mix and intelligibility pass | Audio | 3 | MS-038 |
| MS-044 | Beta full regression compatibility and soak | QA | 15 | MS-038 |
| MS-045 | Commercial choices release materials and support setup | Production | 3 | MS-038 |
| MS-046 | Beta gate and content lock | Production | 1 | MS-039, MS-040, MS-041, MS-042, MS-043, MS-044, MS-045 |

### MS-039 Beta crash save compatibility and performance fixes

Stabilized beta build and regression fixes.

Requirements: REQ-002, REQ-006, REQ-012, REQ-013, REQ-019. Estimate: 13 / 20 / 32 days, low / likely / high.

- [ ] Resolve critical high defects and cover regression.
- [ ] Meet benchmark and recover supported old save fixtures.

### MS-040 Beta balance onboarding and route review

Content lock balance revision.

Requirements: REQ-004, REQ-009, REQ-017. Estimate: 6.5 / 10 / 16 days, low / likely / high.

- [ ] Check repeated-charge comparisons and recovery routes.
- [ ] Keep all required placements within approved quotas.

### MS-041 Accessibility and pseudo-localization review

Accessibility review and repair list.

Requirements: REQ-016, REQ-024. Estimate: 2.5 / 4 / 6.5 days, low / likely / high.

- [ ] Observe controller-only and comfort-settings sessions.
- [ ] Resolve clipped mandatory text and nonredundant cues.

### MS-042 Beta visual polish and cost reduction

Content-locked presentation.

Requirements: REQ-015, REQ-016. Estimate: 7.75 / 12 / 19.25 days, low / likely / high.

- [ ] Fix silhouette and low-quality visibility issues.
- [ ] Reduce visual costs without changing useful collision.

### MS-043 Final mix and intelligibility pass

Mix review and approved cue balance.

Requirements: REQ-015, REQ-016. Estimate: 2 / 3 / 4.75 days, low / likely / high.

- [ ] Check death continuity suppression cues and subtitle coverage.
- [ ] Verify independent volume groups and dynamic range.

### MS-044 Beta full regression compatibility and soak

Beta test matrix and candidate defect status.

Requirements: REQ-001, REQ-002, REQ-003, REQ-005, REQ-006, REQ-007, REQ-008, REQ-009, REQ-010, REQ-011, REQ-012, REQ-013, REQ-014, REQ-015, REQ-016, REQ-017, REQ-021, REQ-023, REQ-024. Estimate: 9.75 / 15 / 24 days, low / likely / high.

- [ ] Run fixture hardware save and 1000-death matrices.
- [ ] Recheck fixes from MS-039 through MS-043 before M4 acceptance.

### MS-045 Commercial choices release materials and support setup

Release decision register and distribution checklist.

Requirements: REQ-020. Estimate: 2 / 3 / 4.75 days, low / likely / high.

- [ ] Resolve commercial name store territories and licensing review.
- [ ] Assign release and support owners with actual account access.

### MS-046 Beta gate and content lock

M4 acceptance and remaining release checklist.

Requirements: REQ-022. Estimate: 0.75 / 1 / 1.5 days, low / likely / high.

- [ ] No unresolved critical or high defects.
- [ ] Freeze content and approve candidate preparation.

## M5 Release candidate and handover

Relative weeks 21â€“24.

- Candidate passes release checklist and clean-machine checks.
- Recovery support known issues and provenance are reviewed.
- Product owner explicitly accepts a candidate for release.

| ID | Task | Role | Likely days | Dependencies |
| --- | --- | --- | ---: | --- |
| MS-047 | Release candidate fixes and package verification | Engineering | 16 | MS-046 |
| MS-048 | Candidate clean-machine and compatibility checks | QA | 12 | MS-046 |
| MS-049 | Store screenshots trailer capture and key art | Art | 5 | MS-046 |
| MS-050 | Credits known issues support and rollback instructions | Production | 2 | MS-046 |
| MS-051 | Final complete-run observation | Design | 5 | MS-046 |
| MS-052 | Candidate audio delivery and credits verification | Audio | 2 | MS-046 |
| MS-053 | Final candidate regression and release checklist | QA | 3 | MS-047, MS-048, MS-049, MS-050, MS-051, MS-052 |
| MS-054 | Archive candidate source and developer handover | Engineering | 4 | MS-047 |
| MS-055 | Candidate acceptance and release handover | Production | 1 | MS-053, MS-054 |

### MS-047 Release candidate fixes and package verification

Versioned release candidate and fix log.

Requirements: REQ-012, REQ-019, REQ-020. Estimate: 10.5 / 16 / 25.5 days, low / likely / high.

- [ ] Close candidate blockers and verify install upgrade saves.
- [ ] Repeat relevant regressions for each changed system.

### MS-048 Candidate clean-machine and compatibility checks

Release candidate evidence matrix.

Requirements: REQ-012, REQ-016, REQ-020. Estimate: 7.75 / 12 / 19.25 days, low / likely / high.

- [ ] Clean install launch input capture save resume and completion.
- [ ] Repeat candidate checks after MS-047 changes.

### MS-049 Store screenshots trailer capture and key art

Approved release media files.

Requirements: REQ-020. Estimate: 3.25 / 5 / 8 days, low / likely / high.

- [ ] Show real gameplay and useful-death route.
- [ ] Use final commercial title and licensed source assets.

### MS-050 Credits known issues support and rollback instructions

Support and release records.

Requirements: REQ-020. Estimate: 1.25 / 2 / 3.25 days, low / likely / high.

- [ ] Archive provenance credits contact and triage process.
- [ ] Document compatible rollback candidate and save restoration.

### MS-051 Final complete-run observation

Final playtest report.

Requirements: REQ-004, REQ-017, REQ-018. Estimate: 3.25 / 5 / 8 days, low / likely / high.

- [ ] Observe new and returning participants through the fortress.
- [ ] Record comprehension comfort and unresolved release concerns.

### MS-052 Candidate audio delivery and credits verification

Final audio package and source records.

Requirements: REQ-015, REQ-020. Estimate: 1.25 / 2 / 3.25 days, low / likely / high.

- [ ] No missing cues or broken routing.
- [ ] Music and voice ownership and credits are recorded.

### MS-053 Final candidate regression and release checklist

Final checklist for exact candidate hash.

Requirements: REQ-020, REQ-022. Estimate: 2 / 3 / 4.75 days, low / likely / high.

- [ ] No critical or high defects and no unresolved progress-loss risk.
- [ ] Installer package and recovery path match reviewed candidate.

### MS-054 Archive candidate source and developer handover

Rebuildable source and artifact archive.

Requirements: REQ-019, REQ-020. Estimate: 2.5 / 4 / 6.5 days, low / likely / high.

- [ ] Record revision engine content rules checksums and build recipe.
- [ ] Another developer reproduces the package from documented inputs.

### MS-055 Candidate acceptance and release handover

M5 release acceptance record.

Requirements: REQ-020, REQ-022. Estimate: 0.75 / 1 / 1.5 days, low / likely / high.

- [ ] Product owner names accepted candidate or defers release.
- [ ] Release operator and support owner acknowledge handover.
