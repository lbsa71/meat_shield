#include "Presentation/MSBodyAnim.h"
#include "Animation/AnimInstanceProxy.h"
#include "AnimNodes/AnimNode_PoseSnapshot.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMesh.h"
namespace {
class FMSBodyProxy : public FAnimInstanceProxy
{
    FAnimNode_PoseSnapshot Pose;
public:
    explicit FMSBodyProxy(UAnimInstance* I):FAnimInstanceProxy(I) { Pose.Mode=ESnapshotSourceMode::SnapshotPin; }
    virtual FAnimNode_Base* GetCustomRootNode() override { return &Pose; }
    virtual void PreUpdate(UAnimInstance* Instance,float Delta) override
    {
        FAnimInstanceProxy::PreUpdate(Instance,Delta);
        Pose.PreUpdate(Instance); // Custom native roots are outside generated BP pre-update lists.
        const auto* Anim=CastChecked<UMSBodyAnim>(Instance);
        if(Anim->UseCapturedPose) { Pose.Snapshot=Anim->CapturedPose; return; }
        const auto* Mesh=Instance->GetSkelMeshComponent()->GetSkeletalMeshAsset();
        const auto& Ref=Mesh->GetRefSkeleton();
        auto& S=Pose.Snapshot; S.LocalTransforms=Ref.GetRefBonePose(); S.BoneNames.Reset(); S.bIsValid=true; S.SkeletalMeshName=Mesh->GetFName();
        for(int I=0;I<Ref.GetNum();++I) S.BoneNames.Add(Ref.GetBoneName(I));
        const int Pelvis=Ref.FindBoneIndex(TEXT("pelvis"));
        if(Anim->Crouched && Pelvis>=0) S.LocalTransforms[Pelvis].AddToTranslation(FVector(0,0,-64));
        auto Direction=[&](FName Bone,FName Child,FVector Desired)
        {
            const int B=Ref.FindBoneIndex(Bone), C=Ref.FindBoneIndex(Child); if(B<0 || C<0) return;
            TArray<FTransform> CS; CS.SetNum(S.LocalTransforms.Num());
            for(int I=0;I<CS.Num();++I) { int P=Ref.GetParentIndex(I); CS[I]=P<0?S.LocalTransforms[I]:S.LocalTransforms[I]*CS[P]; }
            const FQuat Turn=FQuat::FindBetweenNormals((CS[C].GetLocation()-CS[B].GetLocation()).GetSafeNormal(),Desired.GetSafeNormal());
            const int Parent=Ref.GetParentIndex(B);
            S.LocalTransforms[B].SetRotation((CS[Parent].GetRotation().Inverse()*Turn*CS[B].GetRotation()).GetNormalized());
        };
        for(int Side:{-1,1})
        {
            const FString Suffix=Side<0?TEXT("_l"):TEXT("_r");
            auto N=[&](const TCHAR* Name){return FName(*(FString(Name)+Suffix));};
            const float Swing=FMath::Sin(Anim->Phase+(Side<0?0:PI))*Anim->Movement;
            Direction(N(TEXT("upperarm")),N(TEXT("lowerarm")),FVector(Side*.08f,-Swing*.4f,-1));
            Direction(N(TEXT("lowerarm")),N(TEXT("hand")),FVector(0,Anim->Crouched?.8f:.35f,-1));
            Direction(N(TEXT("thigh")),N(TEXT("calf")),Anim->Crouched?FVector(0,1,.7f):FVector(0,Swing*.5f,-1));
            Direction(N(TEXT("calf")),N(TEXT("foot")),Anim->Crouched?FVector(0,-.25f,-1):FVector(0,-FMath::Max(0.f,Swing)*.3f,-1));
        }
    }
};
}
FAnimInstanceProxy* UMSBodyAnim::CreateAnimInstanceProxy() { return new FMSBodyProxy(this); }
void UMSBodyAnim::DestroyAnimInstanceProxy(FAnimInstanceProxy* Proxy) { delete Proxy; }
