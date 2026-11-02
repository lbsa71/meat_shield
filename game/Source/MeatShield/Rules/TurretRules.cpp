#include "TurretRules.h"
namespace MeatShield
{
bool ThreatConfig::IsValid() const
{
    return std::isfinite(RangeCm) && RangeCm > 0 && std::isfinite(AcquireSeconds) && AcquireSeconds >= 0
        && std::isfinite(TelegraphSeconds) && TelegraphSeconds >= 0
        && std::isfinite(ShotIntervalSeconds) && ShotIntervalSeconds > 0
        && std::isfinite(RecoverySeconds) && RecoverySeconds >= 0 && BurstRounds > 0 && BurstRounds <= 64;
}
ThreatRules::ThreatRules(ThreatConfig InConfig) : Config(InConfig) {}
ThreatOutput ThreatRules::Step(double Delta, bool Sight, double Distance, bool Protected)
{
    ThreatOutput Out;
    if (!Config.IsValid() || !std::isfinite(Delta) || Delta < 0 || !std::isfinite(Distance) || Distance < 0)
    { Out.Valid = false; return Out; }
    const bool Visible = Sight && !Protected && Distance <= Config.RangeCm;
    if (Phase == ThreatPhase::Recover)
    {
        Remaining -= Delta;
        if (Remaining <= 1e-8) Phase = ThreatPhase::Idle;
        return Out;
    }
    if (!Visible) { Phase = ThreatPhase::Idle; Remaining = 0; ShotsInBurst = 0; return Out; }
    if (Phase == ThreatPhase::Idle) { Phase = ThreatPhase::Acquire; Remaining = Config.AcquireSeconds; return Out; }
    Remaining -= Delta;
    if (Remaining > 1e-8) return Out;
    if (Phase == ThreatPhase::Acquire) { Phase = ThreatPhase::Telegraph; Remaining = Config.TelegraphSeconds; Out.Warn = true; }
    else
    {
        if (Phase == ThreatPhase::Telegraph) { Phase = ThreatPhase::Burst; ShotsInBurst = 0; }
        Out.Fire = true;
        if (++ShotsInBurst >= Config.BurstRounds) { Phase = ThreatPhase::Recover; Remaining = Config.RecoverySeconds; }
        else Remaining = Config.ShotIntervalSeconds;
    }
    return Out;
}
bool LifeState::Damage(std::uint64_t ExpectedLife, int Amount)
{
    if (ExpectedLife != Life || Health <= 0 || Amount <= 0) return false;
    Health = Amount >= Health ? 0 : Health - Amount;
    if (Health == 0) { ++Deployments; return true; }
    return false;
}
void LifeState::Respawn() { Health = 100; ++Life; }
}
