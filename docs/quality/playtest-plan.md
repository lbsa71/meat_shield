# Meat Shield playtest and measurement plan

The first question is whether players begin choosing useful death locations. Test that before evaluating a long campaign or investing in more weapons. A body-use event can suggest usefulness, but observation establishes whether a player intended the placement and understood the result.

## Sessions and experiments

| ID | Gate and participants | Procedure | Evidence and decision |
| --- | --- | --- | --- |
| EXP-01 | M1, eight new participants comfortable with FPS controls | Start from clean courtyard. Explain movement shoot and objective without revealing a placement solution. Observe up to ten minutes, then ask what changed after their deaths. | At least six intentionally place and subsequently exploit a body. If not, revise geometry teaching or feedback before expanding. |
| EXP-02 | M1, internal route comparison | Run the same clean encounter with a naive repeated straight charge and a planned movement/suppression route. Match rules and seed. Repeat at least five times each and record variance. | Planned placement should materially change route access and improve typical completion. Investigate if accumulation alone is equally effective. |
| EXP-03 | M2, six new and four returning participants | Play representative slice under final-intent presentation. Ask participants to locate their latest useful body and explain threat recovery. Include controller users. | Readable placement payoff and combat purpose persist with art and audio; document misunderstandings by location and build. |
| EXP-04 | M3 and M4, eight participants with varied FPS experience | Play full fortress once, then replay a sector from a fresh state. Observe route choice and comfort. | Improvement appears in deliberate placement and use, not just more attempts; identify repetition and return-travel bottlenecks. |
| EXP-05 | M4 and M5, targeted input and comfort sessions | Recruit controller users and participants who use larger text subtitles or camera comfort options. Exercise core flow with preferred settings. | Required information and actions remain accessible; fix barriers before release. This is targeted feedback, not universal accessibility certification. |

Participant counts are planning samples for qualitative decisions. They do not establish conversion, retention, market size, or statistical significance. Recruit fresh participants for learning tests rather than repeatedly testing developers who know the intended solution.

## Facilitator script

Ask participants to reach the marked control. Teach only necessary controls, allow a short safe practice, then let them attempt the courtyard. Avoid telling them where to die or praising a placement during the discovery phase. If a participant gets stuck, record when assistance was offered and separate assisted outcomes.

Afterward ask: What did your last death change? Where would you want the next body? Why shoot the emplacement? Was any body misleading? Was using the completed route satisfying? Which movement or camera action felt uncomfortable? Ask for a route explanation rather than agreement with the design premise.

Use [the playtest report template](../../templates/playtest-report.md). Record build, rules, hardware, input device, settings, recruitment context, participant code, assistance, observation timestamps, and consent for recordings. Keep recordings and participant contact information in the team's agreed access-controlled location rather than this public-facing source tree.

## Measures

| Measure | Definition | Interpretation |
| --- | --- | --- |
| Intentional placement and use | Participant states or visibly demonstrates a plan, places a body at that location, and later exploits it | Primary evidence for signature mechanic; facilitator codes intent |
| Time to first understood benefit | Active time until participant can explain how a previous body helped | Teaching and feedback indicator |
| Placement repeatability | Distance between repeated proxy anchors with matched inputs | Engineering reliability, separate from fun |
| Useful life rhythm | Life duration and completed action, with typical prototype hypothesis 5–15 seconds | Diagnose long setup or uninformative deaths; do not force duration |
| Construction route advantage | Matched clean-state naive and planned runs, with route and time | Detect accumulation grind or bypass |
| Return-travel time | Spawn to next unsolved decision | Checkpoint and traversal pacing |
| Placement rejection rate | Rejections by reason and tile | Detect quota, coverage, or support problems |
| Combat opportunity use | Suppression followed by advance and progress | Check whether shooting enables construction and breach |
| Death-to-control p95 | Lethal event to first accepted movement on new pawn | Instant retry target |
| Completion and classification | Active time deployments rules content and uninterrupted/resumed/assisted | Compare only matched categories |

## Instrumentation and handling

Emit the local events in [data contracts](../engineering/data-contracts.md) using [telemetry.schema.json](../../planning/telemetry.schema.json). Keep body-use events debounced per body and life and label traversal versus intercepted fire. Do not turn usefulness into a score or automatically infer intent from an event.

Internal development logging is local. A remote collection service is outside the release baseline. Playtest exports use participant codes and the minimum session data needed for the experiment; the producer defines access, retention, and deletion with the participant process. Default proposed retention is 90 days after the relevant gate, subject to that agreed process.

## Review and action

At each review, separate observed behavior, participant explanation, instrumentation, and design interpretation. List the three most consequential issues with their location, likely cause, proposed change, owner, and retest. Preserve successful route examples as regression fixtures. Revisit an experiment when a change affects its finding; do not repeat an unchanged study merely to accumulate a larger report.
