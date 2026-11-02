#include "Presentation/MSGarmentAnim.h"
#include "Animation/AnimInstanceProxy.h"
#include "AnimNodes/AnimNode_CopyPoseFromMesh.h"
#include "Components/SkeletalMeshComponent.h"
namespace {
class FMSGarmentProxy : public FAnimInstanceProxy
{
    FAnimNode_CopyPoseFromMesh Copy;
public:
    explicit FMSGarmentProxy(UAnimInstance* I):FAnimInstanceProxy(I) { Copy.bUseAttachedParent=true; Copy.bUseMeshPose=true; }
    virtual FAnimNode_Base* GetCustomRootNode() override { return &Copy; }
    virtual void PreUpdate(UAnimInstance* I,float Delta) override
    { FAnimInstanceProxy::PreUpdate(I,Delta); Copy.SourceMeshComponent=Cast<USkeletalMeshComponent>(I->GetSkelMeshComponent()->GetAttachParent()); Copy.PreUpdate(I); }
};
}
FAnimInstanceProxy* UMSGarmentAnim::CreateAnimInstanceProxy() { return new FMSGarmentProxy(this); }
void UMSGarmentAnim::DestroyAnimInstanceProxy(FAnimInstanceProxy* Proxy) { delete Proxy; }
