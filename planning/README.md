# Meat Shield structured planning data

These files make the proposed baseline easy to review, validate, and import into a team tracker. They are not runtime game assets or evidence of completed work.

| File | Contents |
| --- | --- |
| [requirements.json](requirements.json) | 24 requirements with provenance target gate source document and test IDs |
| [backlog.json](backlog.json) | 55 tasks with role dependency estimate deliverable and acceptance |
| [milestones.json](milestones.json) | Six gates relative weeks role capacity and optional extension scenario |
| [assets.json](assets.json) | 20 asset or content families with owner quantities tasks and acceptance |
| [tuning.example.json](tuning.example.json) | Versioned initial gameplay and performance hypotheses |
| [save.example.json](save.example.json) | Illustrative committed run snapshot, not a complete authored level |
| [telemetry.schema.json](telemetry.schema.json) | JSON Schema for local event envelope and ten event payloads |
| [toolchain.json](toolchain.json) | Exact Unreal patch and changelist required by the project launcher |

Edit JSON with UTF-8 encoding and preserve stable IDs. `source` provenance means the user's concept; `analysis` means a suggestion in the earlier concept discussion; `proposed` means a new implementation or production default. Acceptance numbers beyond the original concept are proposed measurable targets in every category.

Estimates use person-days. Low, likely, and high are planning scenarios, not probability distributions. M0–M5 are sequential gates, not calendar dates. Every baseline task has P0 priority because it supports the proposed release or its acceptance; sequence comes from dependencies and gates, not that shared priority. Scope cuts require updating requirements and release promises before reprioritizing tasks.

Run `python tools/validate_planning.py --write-backlog` after editing tasks or capacities. Commit the JSON and regenerated [backlog](../docs/production/backlog.md) together. Run the validator without the flag for a read-only consistency check.

The validator checks the limited schema and relationships needed by these files, including task coverage dependencies capacity and example invariants. It does not provide a general JSON Schema engine, fetch external sources, run engine builds, or validate gameplay fun. Future engine fixture IDs and source provenance records must be filled during their assigned tasks.
