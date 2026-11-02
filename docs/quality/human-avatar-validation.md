# Human avatar, limp arms and uniforms validation

REQ-028 / REQ-029, TC-032 / TC-033. 2 November 2026, source `fa7da08` on `codex/metahuman-realism-poc`, Unreal 5.8.4 CL58902052. See the [build manifest](builds/human-avatar-2026-11-02.json), [content provenance](../../planning/realism-content.json) and [architecture decision](../engineering/adr/0008-human-avatar-and-wardrobe.md).

The packaged human scenario passes 47 checks with two deaths, 42 initial bodies and 74 total scene actors. Standing and crouched deaths retain the living human mesh, life-derived outfit and evaluated LOD pose. Old body, face and garment bone transforms remain frozen. Crouch, clearance, native input, off-axis torso rotation, actual moving/fixed meat interception, real damage prevention, camera return, retry persistence, audio and quit pass. The primitive comparison separately passes all 43 existing checks. Current human audio RMS is 0.02772 and peak is 0.35577, with no logged packaged runtime errors. Fresh gameplay captures are 888 by 500 pixels; the requested window size is not treated as capture evidence.

The pinned-torso gravity fixture first failed: hands stayed near shoulder height despite some joint motion. Disabling dead-joint drives and broadening the shoulder/elbow ranges yields independent shoulder/elbow motion of 33.6, 39.5, 30.7 and 43.2 degrees, with both hands more than 45 cm below their shoulders. Native rotation checks use anatomical shoulder-axis change across the fall, excluding a straight backward pitch; primitive collider axes are not anatomical axes.

The first-person proxy sleeves have been replaced by the living human. Crouch initially restored an incorrect class-default mesh offset; constructor defaults now agree with the assembled body. Transfer validation compares evaluated bones at the active LOD, rather than uninitialized transforms of removed bones. Both standing and crouched transfers pass.

The content workflow exposed rigid/root-bound garments when render-data skin weights were extracted without initialized rendering resources. The recipe now rejects that environment and bakes with commandlet rendering enabled. Garments preserve the body's binding, use an outward fitted shell, copy the body's post-physics pose and add no collision. Full-body packaged captures show desert, urban and woodland variants plus vest variation. This remains one source human identity with prototype fitted clothing. The owner accepts tattered clothing as art direction; actual detachment remains a defect.

The capture-order regression first reported requested seed 4 but rendered seed 1. Profile changes now wait for the screenshot-processed callback. Both packaged galleries pass all four requested/rendered seed comparisons (4, 1, 2, 3), and the neutral/pain captures visibly differ. The packaged startup ensure from an editor-only GameFeatureData scan was removed by keeping that scan editor-only.

All 136 native rules checks, 34 tooling tests, planning consistency, editor compilation and Windows build/cook/stage/archive pass. Git LFS local integrity and largest-source binary round-trip pass. All 194 LFS objects (330 MB) were uploaded; a separate fresh remote clone fetched and restored the 112,160,947-byte source character and 1,055,103-byte fitted uniform with SHA-256 matches. No plan or passing unit test alone is counted as integrated acceptance.

## Settled-scene measurements

RTX 4090 Laptop GPU (driver 616.56), i9-14900HX, 31.65 GiB physical RAM, DX12/SM6, 1280 by 720 offscreen. Each separate launch warms for 18 simulation seconds and samples 180 frames. Counts below are pre-downed bodies plus the live avatar. All sampled bodies had settled. These means do not measure pour/death peaks, tails, minimum hardware or a formal release budget. Sequential cache, power and background-system effects are uncontrolled; non-monotonic CPU/frame means do not prove that more bodies are faster.

| Pre-downed bodies | Frame mean ms | Game thread ms | Render thread ms | GPU ms | Process physical MiB |
| --- | --- | --- | --- | --- | --- |
| 1 | 5.537 | 3.032 | 4.866 | 2.086 | 1440.3 |
| 10 | 4.456 | 2.492 | 3.855 | 2.202 | 1444.6 |
| 42 | 3.938 | 2.172 | 3.489 | 2.538 | 1738.0 |

## Reviewed packaged captures

[First-person body](evidence/human-avatar-2026-11-02/avatar-first-person.png), [falling dressed avatar](evidence/human-avatar-2026-11-02/ragdoll-fall.png), [meat cover](evidence/human-avatar-2026-11-02/meat-cover.png), [42-body pile](evidence/human-avatar-2026-11-02/pile-42.png), [neutral face](evidence/human-avatar-2026-11-02/face-neutral.png), [open-mouth pain](evidence/human-avatar-2026-11-02/face-pain.png), [desert uniform](evidence/human-avatar-2026-11-02/uniform-desert.png), [urban uniform](evidence/human-avatar-2026-11-02/uniform-urban.png), [woodland uniform](evidence/human-avatar-2026-11-02/uniform-woodland.png).

The current fitted shells retain rough/open hems and occasional skin intersections, especially in steep downward first-person views. Intentional tattered art direction does not count those artifacts as finished tailoring.

Remaining production work includes authored tactical tailoring, loose cloth evaluation, additional identities, locomotion/foot planting, larger-count and transient performance budgets, save/recovery and clean-machine validation. Persistence is within the loaded session only. The full siege/construction game is not implemented by this POC.
