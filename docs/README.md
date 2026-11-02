# Meat Shield planning index

This handoff defines the proposed game and the work required to prove, build, and release it. Begin with the prototype gate; production proceeds only when placement, feedback, traversal, and performance are convincing.

| Area | Artifact | Primary reader |
| --- | --- | --- |
| Kickoff | [Developer handoff](handoff.md) | Entire team |
| Product | [Project brief](brief.md) | Product owner and producer |
| Provenance | [Source decisions and assumptions](source-and-decisions.md) | Leads and reviewers |
| Mechanics | [Game design](design/game-design.md) | Design and engineering |
| First build | [Turret approach slice](design/first-playable.md) | Development and product owner |
| Death loop | [Persistent ragdoll and return camera](design/persistent-deaths.md) | Development and product owner |
| Cover refinement | [Crouch and complete settling](design/crouch-and-settling.md) | Development and product owner |
| Run | [Run and rebuild the prototype](engineering/running-the-prototype.md) | Product owner and development |
| Content | [Level design](design/level-design.md) | Level design and environment art |
| Presentation | [Art audio UX and accessibility](design/presentation.md) | Art audio UI and design |
| Procedural production | [Unreal and Blender content pipeline](design/procedural-production.md) | Content tooling and art |
| Architecture | [Technical design](engineering/technical-design.md) | Engineering |
| Contracts | [Data contracts](engineering/data-contracts.md) | Engineering and tools |
| Delivery | [Development workflow](engineering/development-workflow.md) | Developers and build owner |
| Environment | [Unreal and Blender setup](engineering/toolchain-setup.md) | Development agent and product owner |
| Setup evidence | [Toolchain validation and open issues](quality/toolchain-validation.md) | Engineering and QA |
| Playable evidence | [First playable validation](quality/first-playable-validation.md) | Engineering and product owner |
| Death-loop evidence | [Persistent ragdoll validation](quality/persistent-deaths-validation.md) | Engineering and product owner |
| Impact and initial field evidence | [Impact and field validation](quality/impact-field-validation.md) | Engineering and product owner |
| Crouch and cover evidence | [Crouch and settling validation](quality/crouch-and-settling-validation.md) | Engineering and product owner |
| Automation decision | [Pinned editor and local MCP](engineering/adr/0004-editor-automation.md) | Engineering |
| Engine | [Engine decision proposal](engineering/adr/0001-engine.md) | Technical lead |
| Bodies | [Corpse construction proposal](engineering/adr/0002-corpse-geometry.md) | Engineering and design |
| Persistence | [Continuous siege proposal](engineering/adr/0003-continuous-siege.md) | Design and engineering |
| Production | [Production and resource plan](production/production-plan.md) | Producer and discipline leads |
| Work | [Implementation backlog](production/backlog.md) | Delivery team |
| Risk | [Risk register](production/risks.md) | Producer and leads |
| Release | [Release and support plan](production/release-plan.md) | Producer QA and build owner |
| Verification | [Test plan](quality/test-plan.md) | QA and engineering |
| Evidence | [Playtest and measurement plan](quality/playtest-plan.md) | Design and QA |
| Data | [Structured planning files](../planning/README.md) | Producer and tools |
| Records | [Review templates](../templates/README.md) | Review facilitators |

## Authority and changes

The user's concept is recorded separately from analysis suggestions and new planning defaults in [source decisions](source-and-decisions.md). None of the analysis suggestions has been represented as a user-approved production decision.

For the proposed implementation baseline, `planning/requirements.json` defines requirement IDs, `planning/tuning.example.json` defines initial numeric tuning, `planning/backlog.json` defines work, and `planning/milestones.json` defines gates and capacity. Design documents explain behavior; technical documents explain implementation proposals. An unresolved contradiction goes to the design lead and technical lead before the affected work starts.

When a decision changes, update its register entry, affected requirements, tuning, task acceptance criteria, tests, and milestone forecasts in the same review. Record actual acceptance in a milestone review; blank templates and planned tests are not evidence of completion.
