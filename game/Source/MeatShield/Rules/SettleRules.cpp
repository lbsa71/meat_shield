#include "SettleRules.h"
#include <cmath>
#include <algorithm>
namespace MeatShield
{
SettleStep SettleRules::Step(double Delta,SettleSample S)
{
    if(!std::isfinite(Delta)||Delta<0||!std::isfinite(S.LinearCmS)||S.LinearCmS<0||!std::isfinite(S.AngularDegS)||S.AngularDegS<0) return {false,false};
    if(Committed) return {};
    Age=std::min(Age+Delta,1000000.0);
    if(S.Supported && S.Collapsed && S.LinearCmS<=15 && S.AngularDegS<=15) Quiet+=std::min(Delta,.1);
    else Quiet=0;
    if(Age>=1.2 && Quiet>=.35) { Committed=true; return {true,true}; }
    return {};
}
}
