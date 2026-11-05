#pragma once
#include <array>
#include <cmath>
#include <cstdint>
namespace MeatShield::Puzzle {
inline constexpr uint32_t Seed=71031, Version=1;
inline constexpr double CrawlRadius=18, CrawlHalfHeight=18, CrawlEye=12, CrawlSpeed=85, MuzzleHeight=30;
struct Stance { double Radius,HalfHeight,Eye,Speed; };
inline Stance Profile(bool Crawl) { return Crawl?Stance{CrawlRadius,CrawlHalfHeight,CrawlEye,CrawlSpeed}:Stance{34,90,75,450}; }
struct Placement { double X,Y,Yaw; };
inline double Height(double X,double Y) {
    if(!std::isfinite(X)||!std::isfinite(Y)) return 0;
    // Smooth compact hills: deterministic recipe, no environment or random globals.
    auto Hill=[&](double CX,double CY,double RX,double RY,double H) {
        const double D=(X-CX)*(X-CX)/(RX*RX)+(Y-CY)*(Y-CY)/(RY*RY);
        return D<1?H*(1-D)*(1-D):0.;
    };
    return Hill(-1150,650,850,800,155)+Hill(-350,-620,900,800,150)
          +Hill(550,650,850,800,155)+Hill(700,-1000,650,450,70);
}
inline std::array<Placement,7> Route() {
    return {{{-1500,-400,0},{-1000,-400,0},{-550,-400,0},{-100,-400,0},{400,-400,0},{850,-450,0},{1250,-450,0}}};
}
inline std::array<Placement,18> Layout() {
    // Stable numbered cover stations and side-route rests, authored to recipe 1.
    return {{{-1650,0,90},{-1250,-100,100},{-850,80,80},{-450,220,100},
             {-50,180,80},{350,-100,90},{750,-200,100},{1150,0,80},
             {-1500,600,25},{-1050,650,-30},{-600,700,130},{-100,700,45},
             {450,700,-50},{1000,650,15},{-1400,-700,30},{-900,-650,-20},
             {-300,-650,120},{400,-700,35}}};
}
}
