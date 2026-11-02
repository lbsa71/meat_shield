#include "World/MSHumanRagdoll.h"
#include "Presentation/MSFaceExpressionAnim.h"
#include "Presentation/MSHumanAppearance.h"
#include "Presentation/MSBodyAnim.h"
#include "PhysicsEngine/ConstraintInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/SkeletalMesh.h"
#include "PhysicsEngine/PhysicsAsset.h"
#include "PhysicsEngine/SkeletalBodySetup.h"
#include "GameFramework/Actor.h"

bool UMSHumanRagdoll::Initialize(uint64 Seed,FVector Velocity,bool Crouched,USkeletalMeshComponent* Source)
{
    auto* Owner=GetOwner();
    Appearance=NewObject<UMSHumanAppearance>(Owner); Owner->AddInstanceComponent(Appearance); Appearance->RegisterComponent();
    if(!Appearance->Initialize(Seed)) return false;
    Body=Appearance->Body; Face=Appearance->Face;
    auto* Mesh=Body->GetSkeletalMeshAsset();
    if(!Mesh->GetPhysicsAsset()) return false;
    if(Source)
    {
        FPoseSnapshot Snapshot; Source->SnapshotPose(Snapshot);
        Body->SetWorldTransform(Source->GetComponentTransform());
        auto* Anim=CastChecked<UMSBodyAnim>(Body->GetAnimInstance()); Anim->CapturedPose=Snapshot; Anim->UseCapturedPose=true;
    }
    else
    {
        auto* Anim=CastChecked<UMSBodyAnim>(Body->GetAnimInstance());
        const auto& Ref=Mesh->GetRefSkeleton(); auto& Pose=Anim->CapturedPose;
        Pose.LocalTransforms=Ref.GetRefBonePose(); Pose.bIsValid=true; Pose.SkeletalMeshName=Mesh->GetFName();
        for(int I=0;I<Ref.GetNum();++I) Pose.BoneNames.Add(Ref.GetBoneName(I));
        Anim->UseCapturedPose=true;
    }
    Body->TickAnimation(0,false); Body->RefreshBoneTransforms(); Body->UpdateComponentToWorld();
    if(Source)
    {
        TransferredAvatar=Body->GetSkeletalMeshAsset()==Source->GetSkeletalMeshAsset();
        int Errors=0;
        // Removed LOD bones have no evaluated transform and cannot be compared.
        for(FBoneIndexType I:Body->GetAnimInstance()->GetRequiredBones().GetBoneIndicesArray()) if(!Body->GetBoneTransform(I).Equals(Source->GetBoneTransform(I),.05f))
        {
            TransferredAvatar=false;
            if(++Errors<5) UE_LOG(LogTemp,Display,TEXT("MS_POSE_DIFFERENCE %s source=%s dest=%s"),*Body->GetBoneName(I).ToString(),*Source->GetBoneTransform(I).ToString(),*Body->GetBoneTransform(I).ToString());
        }
        UE_LOG(LogTemp,Display,TEXT("MS_AVATAR_TRANSFER life=%llu matched=%d crouched=%d"),Seed,TransferredAvatar,Crouched);
    }
    Body->SetCollisionProfileName(TEXT("Ragdoll")); Body->SetCollisionResponseToChannel(ECC_Visibility,ECR_Block);
    Body->SetCollisionResponseToChannel(ECC_Pawn,ECR_Ignore); Body->SetCollisionResponseToChannel(ECC_Camera,ECR_Ignore);
    // Query-only shape mirrors become the separate fixed cover on settlement.
    // Physics and constraints come from Epic's assembled PhysicsAsset.
    auto Add=[&](FName Bone,uint8 Kind,FVector Size,FTransform Local)
    {
        UPrimitiveComponent* Shape=nullptr;
        if(Kind==0) { auto* C=NewObject<UCapsuleComponent>(Owner); C->InitCapsuleSize(Size.X,Size.Z); Shape=C; }
        else if(Kind==1) { auto* C=NewObject<UBoxComponent>(Owner); C->InitBoxExtent(Size); Shape=C; }
        else { auto* C=NewObject<USphereComponent>(Owner); C->InitSphereRadius(Size.X); Shape=C; }
        Owner->AddInstanceComponent(Shape); Shape->SetupAttachment(Owner->GetRootComponent());
        Shape->SetCollisionEnabled(ECollisionEnabled::NoCollision); Shape->RegisterComponent();
        Shapes.Add(Shape); Sizes.Add(Size); Kinds.Add(Kind); Bones.Add(Bone); LocalShapes.Add(Local);
    };
    for(const USkeletalBodySetup* Setup:Mesh->GetPhysicsAsset()->SkeletalBodySetups)
    {
        for(const auto& C:Setup->AggGeom.SphylElems) Add(Setup->BoneName,0,FVector(C.Radius,C.Radius,C.Length*.5+C.Radius),C.GetTransform());
        for(const auto& C:Setup->AggGeom.BoxElems) Add(Setup->BoneName,1,FVector(C.X,C.Y,C.Z)*.5,C.GetTransform());
        for(const auto& C:Setup->AggGeom.SphereElems) Add(Setup->BoneName,2,FVector(C.Radius),C.GetTransform());
    }
    if(Shapes.Num()<2) return false;
    // Keep the common center/pose contract: index one is a chest body.
    int Chest=Bones.IndexOfByPredicate([](FName N){return N.ToString().StartsWith(TEXT("spine_"));});
    if(Chest!=INDEX_NONE) { Shapes.Swap(1,Chest); Sizes.Swap(1,Chest); Kinds.Swap(1,Chest); Bones.Swap(1,Chest); LocalShapes.Swap(1,Chest); }
    for(auto* C:Body->Constraints)
    {
        C->SetOrientationDriveTwistAndSwing(false,false); C->SetOrientationDriveSLERP(false);
        C->SetAngularVelocityDriveTwistAndSwing(false,false); C->SetAngularVelocityDriveSLERP(false);
        C->SetAngularDriveParams(0,0,0);
        // The assembled collision rig's arm cones restrict a fully limp hang.
        // Keep elbows as hinges, with enough range to extend beside the torso.
        const FString Bone=C->ConstraintBone1.ToString();
        if(Bone.StartsWith(TEXT("upperarm_")))
        { C->SetAngularSwing1Limit(ACM_Limited,80); C->SetAngularSwing2Limit(ACM_Limited,80); C->SetAngularTwistLimit(ACM_Limited,45); }
        else if(Bone.StartsWith(TEXT("lowerarm_"))) C->SetAngularTwistLimit(ACM_Limited,90);
    }
    Body->SetAllBodiesSimulatePhysics(true); Body->SetSimulatePhysics(true);
    Body->SetAllPhysicsLinearVelocity(Velocity*.45f); Body->SetAllBodiesBelowPhysicsBlendWeight(NAME_None,1,false,true);
    for(auto* Instance:Body->Bodies) if(Instance) { Instance->LinearDamping=2; Instance->AngularDamping=6; Instance->UpdateDampingProperties(); Instance->SetUseCCD(true); }
    Sync();
    UE_LOG(LogTemp,Display,TEXT("MS_HUMAN_READY life=%llu shapes=%d face=%s post=%s"),Seed,Shapes.Num(),*GetNameSafe(Face->GetAnimInstance()),*GetNameSafe(Face->GetPostProcessInstance()));
    return true;
}
void UMSHumanRagdoll::Sync()
{
    for(int I=0;I<Shapes.Num();++I) if(auto* Instance=Body->GetBodyInstance(Bones[I]))
        Shapes[I]->SetWorldTransform(LocalShapes[I]*Instance->GetUnrealWorldTransform());
}
void UMSHumanRagdoll::Motion(MeatShield::SettleSample& Sample) const
{
    for(const auto* Instance:Body->Bodies) if(Instance)
    {
        Sample.LinearCmS=FMath::Max(Sample.LinearCmS,double(Instance->GetUnrealWorldVelocity().Size()));
        Sample.AngularDegS=FMath::Max(Sample.AngularDegS,double(FMath::RadiansToDegrees(Instance->GetUnrealWorldAngularVelocityInRadians().Size())));
    }
}
void UMSHumanRagdoll::Freeze()
{
    // Preserve the evaluated component-space bone transforms when physics ends.
    Body->bPauseAnims=true; Body->bNoSkeletonUpdate=true;
    Face->bPauseAnims=true; Face->bNoSkeletonUpdate=true;
    Body->SetAllBodiesSimulatePhysics(false); Body->SetSimulatePhysics(false); Body->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Body->SetComponentTickEnabled(false); Face->SetComponentTickEnabled(false);
    for(auto* Garment:{Appearance->Uniform.Get(),Appearance->Vest.Get()})
    { Garment->bPauseAnims=true; Garment->bNoSkeletonUpdate=true; Garment->SetComponentTickEnabled(false); }
}
bool UMSHumanRagdoll::Collapsed() const
{
    const FVector Axis=Body->GetSocketLocation(TEXT("neck_01"))-Body->GetSocketLocation(TEXT("pelvis"));
    return FMath::Abs(Axis.GetSafeNormal().Z)<.6;
}
FVector UMSHumanRagdoll::ShoulderAxis() const
{ return (Body->GetSocketLocation(TEXT("upperarm_r"))-Body->GetSocketLocation(TEXT("upperarm_l"))).GetSafeNormal(); }
bool UMSHumanRagdoll::FaceAttached() const
{ return FVector::Distance(Body->GetSocketLocation(TEXT("head")),Face->GetSocketLocation(TEXT("head")))<2; }
TArray<FTransform> UMSHumanRagdoll::RenderPose() const
{
    TArray<FTransform> Result;
    for(auto* Mesh:{Body.Get(),Face.Get(),Appearance->Uniform.Get(),Appearance->Vest.Get()})
        for(int I=0;I<Mesh->GetNumBones();++I) Result.Add(Mesh->GetBoneTransform(I));
    return Result;
}
bool UMSHumanRagdoll::Impact(FVector Point,FVector Direction,FVector& Torque)
{
    FName Nearest=NAME_None; double Best=MAX_dbl;
    for(FName Bone:Bones) if(auto* Instance=Body->GetBodyInstance(Bone))
    { double D=FVector::DistSquared(Point,Instance->GetCOMPosition()); if(D<Best){Best=D;Nearest=Bone;} }
    if(Nearest.IsNone()) return false;
    const FVector Impulse=Direction.GetSafeNormal()*5000;
    Torque=FVector::CrossProduct(Point-Body->GetBodyInstance(Nearest)->GetCOMPosition(),Impulse);
    Body->WakeAllRigidBodies(); Body->AddImpulseAtLocation(Impulse,Point,Nearest); return true;
}
