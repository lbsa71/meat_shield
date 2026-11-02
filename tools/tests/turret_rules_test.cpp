#include "TurretRules.h"
#include <iostream>
#include <limits>
#include <stdexcept>
using namespace MeatShield;
int Failures = 0, Checks = 0;
void Check(bool Value, const char* Message) { ++Checks; if (!Value) { ++Failures; std::cerr << "FAIL " << Message << '\n'; } }
void EnterBurst(ThreatRules& T) { T.Step(0, true, 1000, false); T.Step(.25, true, 1000, false); T.Step(.6, true, 1000, false); }
int main()
{
    Check(ThreatConfig{}.IsValid(), "default configuration valid");
    auto Bad = ThreatConfig{}; Bad.ShotIntervalSeconds = 0; Check(!Bad.IsValid(), "zero interval rejected");
    Bad = {}; Bad.RangeCm = std::numeric_limits<double>::infinity(); Check(!Bad.IsValid(), "infinite range rejected");
    Bad = {}; Bad.BurstRounds = 0; Check(!Bad.IsValid(), "empty burst rejected");
    ThreatRules T;
    Check(!T.Step(100, true, 1000, true).Fire && T.Phase == ThreatPhase::Idle, "safe spawn never acquires");
    Check(!T.Step(1, true, 4001, false).Fire && T.Phase == ThreatPhase::Idle, "outside range idle");
    T.Step(0, true, 4000, false); Check(T.Phase == ThreatPhase::Acquire, "exact range included");
    Check(!T.Step(.249, true, 4000, false).Warn, "acquisition boundary not early");
    Check(T.Step(.001001, true, 4000, false).Warn && T.Phase == ThreatPhase::Telegraph, "warning on acquire complete");
    Check(!T.Step(.599, true, 1000, false).Fire, "warning completes before shot");
    Check(T.Step(.001001, true, 1000, false).Fire && T.ShotsInBurst == 1, "first shot on warning complete");
    Check(!T.Step(.119, true, 1000, false).Fire, "cadence boundary not early");
    Check(T.Step(.001001, true, 1000, false).Fire, "cadence fires on interval");
    int Shots = 2;
    for (int I=0; I<4; ++I) Shots += T.Step(.12, true, 1000, false).Fire;
    Check(Shots == 6 && T.Phase == ThreatPhase::Recover, "burst exactly six rounds");
    Check(!T.Step(1.49, true, 1000, false).Fire && T.Phase == ThreatPhase::Recover, "cooldown preserved");
    T.Step(.011, true, 1000, false); Check(T.Phase == ThreatPhase::Idle, "cooldown returns to idle");
    EnterBurst(T); Check(T.Phase == ThreatPhase::Burst, "can reacquire after cooldown");
    Check(!T.Step(1, false, 1000, false).Fire && T.Phase == ThreatPhase::Idle, "cover cancels burst");
    T.Step(0, true, 1000, false); Check(!T.Step(.1, true, 1000, false).Fire, "reacquisition requires new warning");
    const auto Before = T.Phase;
    Check(!T.Step(-1, true, 1000, false).Valid && T.Phase == Before, "negative delta rejected without mutation");
    Check(!T.Step(std::numeric_limits<double>::quiet_NaN(), true, 1000, false).Valid, "NaN time rejected");
    Check(!T.Step(1, true, -1, false).Valid, "negative range rejected");
    Check(!T.Step(1, true, std::numeric_limits<double>::infinity(), false).Valid, "infinite distance rejected");
    ThreatRules Hitch; EnterBurst(Hitch); Check(Hitch.Step(1000, true, 1000, false).Fire && Hitch.ShotsInBurst == 2, "hitch emits at most one shot");
    LifeState L;
    Check(!L.Damage(1, 15) && L.Health == 85, "nonlethal damage");
    Check(!L.Damage(1, -5) && L.Health == 85, "negative damage ignored");
    Check(!L.Damage(2, 1000) && L.Health == 85, "stale or foreign life ignored");
    Check(L.Damage(1, 1000) && L.Health == 0 && L.Deployments == 1, "lethal clamped and counted once");
    Check(!L.Damage(1, 1000) && L.Deployments == 1, "duplicate lethal ignored");
    L.Respawn(); Check(L.Health == 100 && L.Life == 2 && L.Deployments == 1, "new life preserves run count");
    Check(!L.Damage(1, 1000) && L.Health == 100, "previous life cannot damage replacement");
    std::cout << Checks << " checks, " << Failures << " failures\n";
    return Failures ? 1 : 0;
}
