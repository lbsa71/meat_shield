#pragma once
#include <cmath>
#include <cstdint>

// REQ-025 / TC-029. No Unreal, device, wall-clock or storage dependencies.
namespace MeatShield
{
enum class ThreatPhase { Idle, Acquire, Telegraph, Burst, Recover };
struct ThreatConfig
{
    double RangeCm = 4000, AcquireSeconds = .25, TelegraphSeconds = .6;
    double ShotIntervalSeconds = .12, RecoverySeconds = 1.5;
    int BurstRounds = 6;
    bool IsValid() const;
};
struct ThreatOutput { bool Valid = true, Fire = false, Warn = false; };
class ThreatRules
{
public:
    explicit ThreatRules(ThreatConfig Config = {});
    ThreatOutput Step(double DeltaSeconds, bool HasSight, double DistanceCm, bool Protected);
    ThreatPhase Phase = ThreatPhase::Idle;
    int ShotsInBurst = 0;
private:
    ThreatConfig Config;
    double Remaining = 0;
};
struct LifeState
{
    int Health = 100;
    std::uint64_t Life = 1, Deployments = 0;
    bool Damage(std::uint64_t ExpectedLife, int Amount);
    void Respawn();
};
}
