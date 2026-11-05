# ADR 0009: repeatable terrain and low crawl

Accepted scope: owner request, 5 November 2026. DEC-29 / REQ-030.

Separate pure height/layout rules, Unreal terrain collision adapter, pose-file storage, movement stance and presentation. Runtime procedural mesh triangles own both visible hills and collision. Bake initial human rests once and store bone snapshots; seeded Chaos simulation alone cannot promise replay determinism. Normal startup restores baked immutable poses. New player deaths retain physical settling and the existing separate stable cover proxies.

The puzzle launch supersedes the flat cosmetic ground and central pile for play. Legacy scenario and gallery flags preserve their tested comparison fixtures. Low crawl is a separate stance with clearance-checked capsule expansion; do not overload native crouch into inconsistent half heights. Live avatar pose and turret target follow that stance. Recipe changes require rebaking and rerunning the route/occlusion checks. This is one prototype puzzle, not the planned three-sector fortress.
