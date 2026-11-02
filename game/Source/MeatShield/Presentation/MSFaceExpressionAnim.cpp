#include "Presentation/MSFaceExpressionAnim.h"
#include "Rules/FaceRules.h"
#include "Animation/AnimInstanceProxy.h"
#include "AnimNodes/AnimNode_CopyPoseFromMesh.h"
#include "AnimNodes/AnimNode_ModifyCurve.h"
#include "Components/SkeletalMeshComponent.h"

namespace {
class FMSFaceProxy : public FAnimInstanceProxy
{
public:
    FAnimNode_CopyPoseFromMesh Copy;
    FAnimNode_ModifyCurve Curves;
    explicit FMSFaceProxy(UAnimInstance* Instance):FAnimInstanceProxy(Instance)
    {
        Copy.bUseAttachedParent=true; Copy.bUseMeshPose=true; Copy.bCopyCurves=false;
        Curves.SourcePose.SetLinkNode(&Copy); Curves.Alpha=1; Curves.ApplyMode=EModifyCurveApplyMode::Blend;
        const TCHAR* Names[]={TEXT("browDownL"),TEXT("browDownR"),TEXT("eyeBlinkL"),TEXT("eyeBlinkR"),
            TEXT("eyeSquintInnerL"),TEXT("eyeSquintInnerR"),TEXT("jawOpen"),TEXT("noseWrinkleL"),TEXT("noseWrinkleR"),
            TEXT("mouthCornerDepressL"),TEXT("mouthCornerDepressR"),TEXT("mouthPressUL"),TEXT("mouthPressUR"),TEXT("mouthPressDL"),TEXT("mouthPressDR")};
        for(const TCHAR* Name:Names) { Curves.CurveNames.Add(FName(*(FString(TEXT("CTRL_expressions_"))+Name))); Curves.CurveValues.Add(0); }
    }
    virtual FAnimNode_Base* GetCustomRootNode() override { return &Curves; }
    virtual void PreUpdate(UAnimInstance* Instance,float Delta) override
    {
        FAnimInstanceProxy::PreUpdate(Instance,Delta);
        Copy.SourceMeshComponent=Cast<USkeletalMeshComponent>(Instance->GetSkelMeshComponent()->GetAttachParent());
        Copy.PreUpdate(Instance);
        const auto Profile=MeatShield::ExpressionFor(CastChecked<UMSFaceExpressionAnim>(Instance)->FaceSeed);
        for(int I=0;I<Curves.CurveValues.Num();++I) Curves.CurveValues[I]=Profile.Values[I];
    }
};
}
FAnimInstanceProxy* UMSFaceExpressionAnim::CreateAnimInstanceProxy() { return new FMSFaceProxy(this); }
void UMSFaceExpressionAnim::DestroyAnimInstanceProxy(FAnimInstanceProxy* Proxy) { delete Proxy; }
