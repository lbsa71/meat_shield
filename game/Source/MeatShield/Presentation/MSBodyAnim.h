#pragma once
#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Animation/PoseSnapshot.h"
#include "MSBodyAnim.generated.h"
UCLASS(Transient)
class UMSBodyAnim : public UAnimInstance
{
    GENERATED_BODY()
public:
    FPoseSnapshot CapturedPose;
    bool UseCapturedPose=false,Crouched=false,Crawling=false;
    float Phase=0,Movement=0;
protected:
    virtual FAnimInstanceProxy* CreateAnimInstanceProxy() override;
    virtual void DestroyAnimInstanceProxy(FAnimInstanceProxy* Proxy) override;
};
