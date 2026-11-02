#pragma once
#include <cstdint>
namespace MeatShield
{
enum class DeathPhase { FirstPerson, PullBack, Observe, Return };
struct DeathConfig
{
    double PullBack=.35, Observe=.35, Return=.35;
    bool IsValid() const;
    double Total() const { return PullBack+Observe+Return; }
};
struct DeathStep { bool Valid=true, Completed=false; };
class DeathTimeline
{
public:
    explicit DeathTimeline(DeathConfig Config={});
    bool Start(std::uint64_t Life);
    DeathStep Step(double Delta);
    bool Active() const { return Phase!=DeathPhase::FirstPerson; }
    double Progress() const;
    DeathPhase Phase=DeathPhase::FirstPerson;
    double Elapsed=0;
private:
    DeathConfig Config;
    std::uint64_t LastLife=0;
};
}
