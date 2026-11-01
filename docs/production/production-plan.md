# Meat Shield production and resource plan

The original delivery scenario is 24 working weeks, followed by a separately funded five-week contingency if needed. Its human-team capacity model remains a comparison for scope and effort. The product owner has since directed AI-led development under [AGENTS.md](../../AGENTS.md); this scenario is not the active delivery forecast. Reforecast from actual implementation and validation throughput after environment setup and the prototype. No calendar launch date is committed.

## Milestones and gate evidence

| Gate | Relative weeks | Deliverable | Acceptance evidence |
| --- | --- | --- | --- |
| M0 Kickoff and feasibility | 1 | Packaged bootstrap, owners, benchmark, engine and asset workflow | Clean-checkout build, manifests, decision record |
| M1 Core prototype | 2–4 | One short greybox siege, stable useful bodies, instant retry, rifle and threat | Placement and occlusion fixtures, respawn timings, eight-player observed test, route comparisons |
| M2 Representative slice | 5–8 | One integrated sector with final-intent presentation, settings, saving and reset | Packaged sector, full-budget construction traces, corruption recovery, accessibility review, scope forecast |
| M3 Complete fortress alpha | 9–16 | All three sectors and baseline features integrated | Full completion, asset inventory, controller and keyboard runs, save/reset matrix, alpha defect list |
| M4 Beta and content lock | 17–20 | Stable and measured complete game | Regression, compatibility, accessibility, performance, no critical or high defects, release decisions |
| M5 Candidate and handover | 21–24 | Reviewed exact candidate and reproducible developer handover | Clean-machine tests, credits and source records, rollback rehearsal, release acceptance |

Each gate can produce **go**, **revise**, or **stop**. A failing central mechanic or progress-loss defect cannot be waived as cosmetic polish. The product owner makes the scope decision with design and technical evidence; the producer records it in the [milestone review template](../../templates/milestone-review.md).

## Critical delivery chain

The main chain is environment and blockout → reliable death and construction → observed intentional placement → bounded geometry and durable world state → representative integrated slice → complete fortress → stabilization → candidate verification. Asset and audio production can run alongside code once interfaces and scale are established.

The [backlog](backlog.md) contains dependency IDs. Those dependencies and person-day estimates are an implementation work breakdown, not a resource-leveled daily schedule. Tasks with several days of work can be integrated incrementally; art and QA reviews begin on early increments and finish against the final candidate. At kickoff the producer must place named people and availability on that graph, reserve review time, and test whether each gate fits. Do not promise these weeks as a fixed-price delivery schedule.

## Staffing and capacity model

Assume five workdays per week and no holidays in this initial capacity calculation. Availability must be reduced for actual leave, part-time work, and other commitments before a start date is agreed.

| Role | Baseline availability | Baseline person-days | Backlog likely days | Available for iteration and integration |
| --- | --- | ---: | ---: | ---: |
| Engineering | Two full-time engineers for 24 weeks | 240 | 139 | 101 |
| Design | One full-time designer for 24 weeks | 120 | 79 | 41 |
| Art | One full-time artist for 24 weeks | 120 | 82 | 38 |
| Production | 0.25 full-time equivalent for 24 weeks | 30 | 12 | 18 |
| QA | Contracted gate blocks | 50 | 50 | 0 |
| Audio | Contracted slice production and mix blocks | 15 | 15 | 0 |
| Total | Capacity model | 575 | 377 | 198 |

The 377 likely days cover named deliverables. The remaining 198 days are available capacity for implementation discovery, reviews, playtest-driven changes, coordination, bug fixing beyond named tasks, and integration. They are not hidden completed work or a second set of deliverables. The estimate range for the named backlog is 602.5 days at the sum of upper bounds; this is a scenario, not a statistical confidence interval. It exceeds baseline capacity by 27.5 days overall, with shortages in design, art, QA, and audio; spare engineering capacity cannot automatically cover those roles. Rebook specialists, change allocation, reduce scope, or fund an extension if that scenario develops.

Role capacity by gate is stored in [milestones.json](../../planning/milestones.json). QA is allocated 3, 5, 12, 15, and 15 days across M1–M5. Audio is allocated 4, 6, 3, and 2 days across M2–M5. Book these windows early; delayed specialists can block otherwise finished milestones.

The optional five-week extension adds 50 engineering days, 25 design days, 25 art days, 6.25 production days, 10 QA days, and 3 audio days: **119.25 additional days**, or **694.25 days including the full baseline**. These are a scenario for funding, not already assigned task capacity. Extra subcontracting must be agreed with vendors. Even this extension leaves QA and audio short if their entire upper scenarios occur; the producer must price that exposure explicitly.

## Budget worksheet

Use one agreed currency and actual loaded daily rates. Let E, D, A, P, Q, and S be engineering, design, art, production, QA, and audio rates per person-day.

```text
Baseline labor = 240E + 120D + 120A + 30P + 50Q + 15S
Optional full extension = 50E + 25D + 25A + 6.25P + 10Q + 3S
Total funding = chosen labor scenario + approved nonlabor costs
```

The producer must price build hardware and runner storage, source and artifact hosting, licensed art and tools, participant recruitment, translation if added, distribution fees, commercial name review, and launch/support work. Record quote source, date, tax treatment, currency, payment schedule, and owner in the actual budget. Do not substitute the likely-task total for the cost of retaining the proposed team across the full schedule.

If only a prototype is funded, authorize M0 and M1 explicitly and re-quote M2 onward after the gate. No public pricing or current vendor fee has been assumed in this plan.

## Cadence and responsibility

Use a short daily delivery check, weekly playable review, and weekly risk and forecast review. Demonstrate route changes and player behavior in a packaged build. Keep the current build, gate forecast, blocked tasks, and top three risks visible.

| Decision | Accountable role | Required contributors |
| --- | --- | --- |
| Player promise and release scope | Product owner | Designer producer and technical lead |
| Movement placement and encounter rules | Designer | Technical lead QA and artist |
| Architecture build performance and saves | Technical lead | Engineers QA and designer |
| Asset silhouette and presentation | Artist | Designer technical lead and audio contractor |
| Forecast budget and vendor bookings | Producer | All discipline owners |
| Verification status and defect severity | QA owner | Engineering design and producer |
| Candidate release acceptance | Product owner | Producer QA technical lead and release operator |

## Scope control and cuts

First cut optional voice notices, decorative prop variation, trailer polish, and cosmetic intensity. Then simplify the second emplacement's presentation while retaining the combined-angle encounter. Reducing a sector or changing the platform requires a product scope change and new promise; do not quietly turn three sectors into two.

Do not cut instant retry, reliable body geometry, recoverable progress, essential accessibility, or the test evidence that proves the premise. If those cannot fit, revisit the production baseline at the gate rather than hiding the gap in contingency.
