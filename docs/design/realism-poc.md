# MetaHuman realism proof of concept

Default play now follows [the low-crawl puzzle contract](crawl-puzzle.md), REQ-030. The flat floor and central pile described below remain legacy comparison fixtures.

REQ-028 / TC-032, owner direction 2 November 2026. Realistic human bodies and varied pained facial expressions are core presentation experiments, not final release acceptance.

Use Epic MetaHuman Creator to assemble a small optimized source set. Seed identity/appearance and expression per life. Compare neutral, brow pinch/squint grimace, jaw tension and open-mouth pain, with bounded intensity and asymmetry. Inspect close-ups before the full pile. Maintain face/body pose synchronization through falling physics and fixed settlement. Evaluate a frozen expression on settled corpses before enabling continuing facial evaluation for every old body. Do not claim realistic or distinct expressions from curve values alone; require visible packaged differences.

Keep character presentation, physics/support authority, expression policy and editor asset generation separate. Runtime must not require Creator, cloud requests or editor tooling. Store source character settings, recipes, versions, content provenance and required cooked assets using a validated binary workflow. Report missing data/authentication honestly; no silent mannequin fallback. Optimize hair, facial evaluation and body LODs based on measured 1/10/42-body cases. Preserve actual bullet and movement silhouettes.

Use Unreal procedural tools for terrain/ground and dressing. The first comparison preserves flat authoritative collision in the firing lane and protected spawn. Generate rough soil/gravel appearance, shallow cosmetic surface variation and constrained edge dressing from stable seeds. No decoration may become invisible cover or block the test route. Evaluate directional light, sky lighting, ambient occlusion/contact shadows and material roughness in matched captures. Change rendering features only with measured cost and a decision record. PCG placement and Landscape/mesh authoring are distinct; runtime landscape sculpting is not assumed.

Required evidence: real assembled MetaHuman and cooked dependencies; rendered close-up neutral/pain comparison; correct face attachment during ragdoll and after settling; deterministic variation and no asset duplication; 42-body pile and existing gameplay regression; matched ground/lighting captures; CPU/GPU frame and memory measurements with hardware/settings; packaged launch without Creator or cloud access. Record any blocked portion separately from completed independent work.

## Player embodiment and uniforms

REQ-029 / TC-033, owner playtest 2 November 2026. Correct the stiff-arm regression using observed bilateral shoulder and elbow motion relative to the torso, not whole-body movement alone. Disable animation drives on dead joints and retain bounded anatomical constraints. Settlement must wait for the arms as well as the torso.

The first-person player must be the same MetaHuman that falls: share source meshes and deterministic outfit selection, and transfer the last evaluated local bone pose and mesh transform before physics starts. Preserve crouched posture. Head hiding for first-person rendering must not alter the transferred skeleton. Existing capsule movement remains authoritative. Seed belongs to the life/run context and is preserved by the corpse; the presentation component does not decide death or respawn.

The first wardrobe experiment uses reproducible fitted skeletal uniform shells, three military camouflage palettes and seeded fabric patterns. Geometry and skin weights are baked in Unreal; runtime selects material parameters and attaches garments to the same skeleton. This is an initial fitted uniform, not a claim of production tactical tailoring or Chaos cloth simulation. Garments remain cosmetic and must not own bullet collision. Evaluate Chaos Outfit parametric fitting and Mutable for richer wardrobe assembly separately from simulating loose cloth on every old corpse.

TC-033 checks shoulder/elbow articulation on both sides under gravity, live-to-dead mesh/seed/pose continuity for standing and crouching, varied deterministic outfits, missing content errors, frozen garment/head attachment, retained bodies and fresh packaged visual evidence.

Owner art direction: tattered and worn military clothing is appropriate. Treat wear as intentional seeded garment variation; deformation or attachment defects are still defects.

Native rotation evidence follows the anatomical shoulder axis over the fall: require more than 15 degrees of off-axis change (yaw or roll), which a straight backward pitch alone cannot produce. Primitive comparison retains its strong sideways-roll case. Do not test a MetaHuman using the arbitrary local axes of a primitive collision capsule. Capture fixtures must retain the requested life/style until the screenshot-completed callback; changing style in the request frame invalidates its label.

Both user-facing repository launchers select the human profile after the owner requested continuous embodiment. Direct executable launch without `-MSRealism` remains the development primitive comparison.
