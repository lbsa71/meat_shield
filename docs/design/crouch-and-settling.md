# Crouch, meat cover and physics pile

Current owner-directed contract, 2 November 2026. REQ-001, REQ-006, REQ-026, REQ-027 / TC-031. See [ADR 0007](../engineering/adr/0007-crouch-and-settling.md) for decisions and superseded approaches.

Hold Left Ctrl or C to crouch; release stands only with clearance. Native CharacterMovement owns the 1 m capsule, 2.2 m/s crouch speed and clearance. Lower the eye and turret torso target with stance. Sprint does not override crouch speed.

All meat stops shots at the first surface while falling and after commitment. A shared ballistic adapter determines damage and tracer endpoint. Lethal shots transfer their actual impact point to the new rig; later hits apply impulse at the struck physics part. Small deterministic torso aim offsets vary moment arms. The falling-body fixture checks pronounced sideways torso rotation, rather than merely counting an impulse call. Fixed bodies reject impacts and remain immutable.

The turret muzzle is about 95 cm above ground. From its actual muzzle, two prone real-body proxies with a combined height of 72 cm must intercept a crouched torso ray (55 cm target) while a standing torso ray (115 cm target) clears them. The fixture lies in the south lane, outside authored concrete sightlines. Cover follows actual silhouettes and gaps.

The initial content is one central gravity-built pile, replacing the scattered authored field. Seed 71031 creates 42 articulated rigs in a separated 3 by 2 by 7 aerial grid centered at (-1050,0) cm. Horizontal spacing is 160 cm, vertical spacing 220 cm, initial heights 450–1770 cm, horizontal jitter 5 cm and initial pitch/roll within 15 degrees. All are active together within two seconds. CCD prevents fast falling parts tunneling. The safe start and concrete cover stay outside the drop footprint. Play remains available while the batch falls.

Poured rigs collide with each other, the world and older fixed geometry. Free collision-only constraints suppress self contacts between parts of the same rig, without constraining motion. Temporary filters and anatomical joints are destroyed at commitment. Stronger poured-body damping (linear 2, angular 6) and solver iterations (16 position, 8 velocity) reduce contact jitter. Player-death rigs retain their existing active-contact policy.

Player deaths require a supported collapsed torso, minimum age 1.2 s and 0.35 s with maximum part speed at most 15 cm/s and 15 deg/s. The 1.05 s camera return is independent, so physical collapse may continue after control returns. Support beneath any physical part is valid when a torso bridges a gap.

For the dense initial pile, require supported gravity-produced rest and an observed stable pose envelope for all 15 parts throughout the same quiet interval: positions within 2 cm and observable orientation within 5 degrees of a fixed reference. Any exceedance resets the reference and quiet time. Spheres have no observable orientation, and capsule axial twist does not change their geometry; compare capsule axis direction and full box rotation. This accepts bounded solver vibration without freezing visibly moving rigs. Minimum age and physical support still apply. Natural curled or inclined resting poses are accepted in the pile. There is no age-only force-lock or eviction.

Use stable high-range IDs for initial bodies, separate from player deployments. Build once per level; retries do not repour or duplicate it. Final primitives match the physical resting transforms and preserve all earlier cover.

The native and rendered diagnostics require all 42 to move under gravity, settle and stack above a single-body height within 90 seconds. A timeout fails acceptance without forcing a pose. Exercise batch activation, low muzzle height, two-body crouch/standing traces, support, pose stability, older transforms, retries, actual damage interception, camera timing, controls and fresh rendered/audio evidence. Review the completed central pile capture. Large-count performance and save restoration remain future gates.
