#pragma once
#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MSGarmentAnim.generated.h"
UCLASS(Transient)
class UMSGarmentAnim : public UAnimInstance
{
    GENERATED_BODY()
protected:
    virtual FAnimInstanceProxy* CreateAnimInstanceProxy() override;
    virtual void DestroyAnimInstanceProxy(FAnimInstanceProxy* Proxy) override;
};
