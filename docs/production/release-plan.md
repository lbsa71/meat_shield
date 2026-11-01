# Meat Shield release and support plan

The release target is a downloadable Windows PC build. Storefront, price, territories, public title, and launch date remain product decisions. This plan defines preparation and acceptance; no remote issues, storefront entry, publication, or messages to developers have been created.

## Candidate identity

Every candidate needs its source and content revisions, engine/toolchain manifest, rules and save schema versions, build configuration, package checksum, benchmark preset, supported save fixtures, known issues, and test results. Acceptance applies to that exact package. A changed candidate needs the affected regressions rerun and an updated acceptance record.

## Release checklist

| Workstream | Acceptance | Accountable role |
| --- | --- | --- |
| Gameplay | Complete three-sector siege, no unreliable cover, no spawn loops, no resource softlocks | Designer |
| Quality | No open critical or high defects; accepted lower-severity issues documented | QA owner |
| Performance | Publish minimum and recommended specifications only after measured hardware coverage and quality review | Technical lead |
| Install and runtime | Clean machine installation launch input display audio saves exit and reinstall behave as documented | Build owner and QA |
| Progress | New run resume checkpoint reset corrupted newest save and supported upgrades pass | Technical lead and QA |
| Compatibility | Test oldest supported save fixture and update path; newer incompatible saves are preserved and explained | Technical lead |
| Input and access | Keyboard mouse controller rebinding text scale subtitles comfort and reduced gore pass the full flow | Designer and QA |
| Provenance | Engine dependencies tools art audio fonts credits and source records reviewed for intended distribution | Producer |
| Commercial | Final name price channel territories and platform account ownership resolved | Product owner |
| Platform preparation | Release owner checks current official platform packaging content disclosure and submission requirements for chosen channel | Producer |
| Media | Screenshots and trailer demonstrate real useful-body gameplay in the accepted content and final branding | Artist and designer |
| Support | Contact owner intake template diagnostic instructions known issues and escalation coverage exist | Producer |
| Recovery | Last good package and save backup restored in a rehearsal; rollback compatibility understood | Build owner and QA |
| Acceptance | Product owner records go or defer for exact candidate with QA and technical lead evidence | Product owner |

Current store and legal requirements must be checked when a channel and territories are selected. This handoff makes no claim of name clearance, age classification, contractual approval, or platform acceptance.

## Release materials

Prepare a short description centered on death as construction, a clear gameplay trailer, representative screenshots, installation and controls guide, accessibility feature list, content description, credits, support contact, and known issues. A trailer should show the same location before death, useful placement, immediate return, and successful traversal. Avoid showing later-scope features as if they exist.

The release candidate must include a version label in the menu and diagnostic export so reports can be tied to source and rules. Diagnostics should expose hardware and logs selected for the report, while excluding unrelated local files. There is no mandatory online account or remote gameplay service in the baseline.

## Rollback and save recovery

Keep the last accepted candidate, its build inputs, and the previous-good save fixtures. Before release, rehearse installing candidate N, making a save, upgrading to N+1, and recovering both a corrupted N+1 save and the N save backup. Rolling binaries back is not sufficient if the older binary cannot read newer saves; restore a compatible backup or forward-fix the reader. Never silently delete a newer incompatible save.

If a released build loses progress, corrupts saves, causes repeat crashes, or invalidates support geometry, the release owner pauses further distribution changes, preserves reports and affected saves, and coordinates a fix or rollback with QA. Publish a player-facing explanation through the selected channel only under the release owner's authorization.

## Launch support cadence

Reserve named engineering, QA, and producer availability for the first two working days after release and daily triage during the first week. Actual launch timing must fit that coverage; no weekend support is assumed. Classify reports by severity and reproducibility, link duplicates, and identify whether a compatible workaround exists.

Critical issues are crashes, progress loss, corrupt saves, persistent spawn loops, and broken required routes. High issues prevent an important baseline feature or exclude a supported input path. Medium and low issues affect polish or have a clear acceptable workaround. Hotfixes must retain the same build identity and regression discipline as the first candidate.

At the end of launch support, archive the accepted build record, outstanding issues, actual schedule and cost, playtest lessons, and remaining scope proposals. Do not reopen deferred systems as an automatic post-launch commitment.
