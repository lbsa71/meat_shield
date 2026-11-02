#pragma once
#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MSFaceExpressionAnim.generated.h"

// Raw facial curves feed the MetaHuman mesh's assembled RigLogic post-process.
UCLASS(Transient)
class UMSFaceExpressionAnim : public UAnimInstance
{
    GENERATED_BODY()
public:
    uint64 FaceSeed=0;
protected:
    virtual FAnimInstanceProxy* CreateAnimInstanceProxy() override;
    virtual void DestroyAnimInstanceProxy(FAnimInstanceProxy* Proxy) override;
};
