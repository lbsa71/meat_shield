#pragma once
namespace MeatShield
{
struct SettleSample { double LinearCmS=0, AngularDegS=0; bool Supported=false, Collapsed=false; bool UsePoseEnvelope=false,PoseWithinEnvelope=false; };
struct SettleStep { bool Valid=true, Commit=false; };
class SettleRules
{
public:
    SettleStep Step(double Delta,SettleSample Sample);
    double Age=0,Quiet=0;
    bool Committed=false;
};
}
