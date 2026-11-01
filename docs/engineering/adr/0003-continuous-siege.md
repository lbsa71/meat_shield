# Continuous siege proposal

Status: Proposed. Owners: Designer and technical lead. Decision gate: M1.

## Context

Resetting the world on every death would erase construction or force repeated setup. Keeping every defender permanently defeated could remove the need to construct routes. Respawning at the death point could bypass the route entirely.

## Proposed decision

Keep the siege running through deaths and replace only the pawn. Bodies and completed objectives persist. Fixed defensive devices recover from temporary rifle suppression on continuous timers; reaching their controls disables them permanently and advances the assault point. Weapon ammunition and reload progress persist across death.

## Consequences and acceptance

World and weapon state must live outside the pawn. Tests must cover lethal damage during capture, suppression across death, save during a state transition, and victory emission once. A naive repeated charge must not outperform a learned placement route solely because threats reset or ammo refills. Accept only after observation establishes why shooting and body placement both matter.
