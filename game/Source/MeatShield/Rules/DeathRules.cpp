#include "DeathRules.h"
#include <cmath>
#include <algorithm>
namespace MeatShield
{
bool DeathConfig::IsValid() const { return std::isfinite(PullBack)&&PullBack>0&&std::isfinite(Observe)&&Observe>0&&std::isfinite(Return)&&Return>0&&std::isfinite(Total()); }
DeathTimeline::DeathTimeline(DeathConfig C):Config(C) {}
bool DeathTimeline::Start(std::uint64_t Life)
{
    if(!Config.IsValid() || Active() || Life==0 || Life<=LastLife) return false;
    LastLife=Life; Elapsed=0; Phase=DeathPhase::PullBack; return true;
}
DeathStep DeathTimeline::Step(double Delta)
{
    if(!Config.IsValid() || !std::isfinite(Delta) || Delta<0) return {false,false};
    if(!Active()) return {};
    Elapsed=std::min(Elapsed+Delta,Config.Total());
    if(Elapsed+1e-8>=Config.Total()) { Phase=DeathPhase::FirstPerson; return {true,true}; }
    if(Elapsed+1e-8>=Config.PullBack+Config.Observe) Phase=DeathPhase::Return;
    else if(Elapsed+1e-8>=Config.PullBack) Phase=DeathPhase::Observe;
    return {};
}
double DeathTimeline::Progress() const
{
    if(Phase==DeathPhase::PullBack) return std::clamp(Elapsed/Config.PullBack,0.0,1.0);
    if(Phase==DeathPhase::Observe) return std::clamp((Elapsed-Config.PullBack)/Config.Observe,0.0,1.0);
    if(Phase==DeathPhase::Return) return std::clamp((Elapsed-Config.PullBack-Config.Observe)/Config.Return,0.0,1.0);
    return 0;
}
}
