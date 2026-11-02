#include "TurretRules.h"
#include "DeathRules.h"
#include "SettleRules.h"
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
    DeathTimeline Death;
    Check(DeathConfig{}.IsValid(), "death timing valid");
    Check(!Death.Start(0), "zero life rejected");
    Check(Death.Start(2) && Death.Active(), "death begins once");
    Check(!Death.Start(2) && !Death.Start(3), "duplicate or overlapping death rejected");
    Check(!Death.Step(-1).Valid && Death.Elapsed==0, "invalid death delta does not advance");
    Check(!Death.Step(std::numeric_limits<double>::quiet_NaN()).Valid, "nonfinite death delta rejected");
    Death.Step(.349); Check(Death.Phase==DeathPhase::PullBack && Death.Progress()<1, "pullback boundary");
    Death.Step(.001001); Check(Death.Phase==DeathPhase::Observe, "observe follows pullback");
    Death.Step(.35); Check(Death.Phase==DeathPhase::Return, "return follows observation");
    Check(Death.Step(.35).Completed && !Death.Active(), "one return completion");
    Check(!Death.Step(100).Completed && !Death.Start(2) && !Death.Start(1), "old lives cannot replay death");
    Check(Death.Start(3) && Death.Step(100).Completed, "large delta finishes one new death");
    DeathConfig Invalid; Invalid.Return=-1; Check(!Invalid.IsValid(), "negative camera timing rejected");
    SettleRules Settle;
    const SettleSample Rest{0,0,true,true};
    for(int I=0;I<7;++I) Check(!Settle.Step(.1,Rest).Commit,"early body cannot commit");
    Check(!Settle.Step(-1,Rest).Valid && Settle.Age<.8,"negative settle delta rejected");
    Check(!Settle.Step(.1,{std::numeric_limits<double>::quiet_NaN(),0,true,true}).Valid,"invalid body velocity rejected");
    Settle.Step(.6,{50,0,true,true}); Check(Settle.Quiet==0,"motion resets quiet interval");
    Settle.Step(.1,Rest);Settle.Step(.1,Rest);
    Settle.Step(.1,{0,0,false,true});Check(Settle.Quiet==0,"lost support resets quiet");
    Settle.Step(.1,{0,0,true,false});Check(!Settle.Committed,"upright body never commits");
    Settle.Step(.1,{0,40,true,true});Check(Settle.Quiet==0,"angular motion prevents lock");
    Settle.Step(.1,Rest);Settle.Step(.1,Rest);Settle.Step(.1,Rest);
    Check(Settle.Step(.1,Rest).Commit,"supported quiet body commits");
    Check(!Settle.Step(5,Rest).Commit,"commit emits once");
    SettleRules HitchSettle;Check(!HitchSettle.Step(100,Rest).Commit,"hitch is not proof of sustained quiet");
    std::cout << Checks << " checks, " << Failures << " failures\n";
    return Failures ? 1 : 0;
}
