#include "FaceRules.h"
#include <algorithm>
namespace MeatShield {
FaceExpression ExpressionFor(std::uint64_t Life)
{
    FaceExpression Face; Face.Preset=unsigned(Life%4);
    if(!Face.Preset) return Face;
    // Brows L/R, blink L/R, squint L/R, jaw, nose L/R, corners L/R, lip press UL/UR/DL/DR.
    static constexpr std::array<float,15> Profiles[] = {
        {}, {.8f,.8f,.55f,.55f,.7f,.7f,.1f,.4f,.4f,.4f,.4f,.4f,.4f,.4f,.4f},
        {.6f,.6f,.25f,.25f,.4f,.4f,.7f,.3f,.3f,.55f,.55f,0,0,0,0},
        {.95f,.95f,.95f,.95f,.85f,.85f,.02f,.5f,.5f,.25f,.25f,.8f,.8f,.8f,.8f}};
    Face.Values=Profiles[Face.Preset];
    const float Variation=float((Life*6364136223846793005ULL+1442695040888963407ULL)>>56)/255.f;
    const float Intensity=.85f+Variation*.15f;
    for(float& V:Face.Values) V*=Intensity;
    Face.Values[1]=std::clamp(Face.Values[1]-.03f-Variation*.04f,0.f,1.f);
    Face.Values[3]=std::clamp(Face.Values[3]+Variation*.05f,0.f,1.f);
    return Face;
}
}
