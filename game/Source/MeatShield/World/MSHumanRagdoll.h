#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Rules/SettleRules.h"
#include "MSHumanRagdoll.generated.h"
class USkeletalMeshComponent;
class UPrimitiveComponent;
// REQ-028: native skeletal physics adapter; expression policy stays in Rules.
UCLASS()
class UMSHumanRagdoll : public UActorComponent
{
    GENERATED_BODY()
public:
    bool Initialize(uint64 Seed,FVector Velocity,bool Crouched,USkeletalMeshComponent* Source=nullptr);
    UPROPERTY() TObjectPtr<class UMSHumanAppearance> Appearance;
    void RestorePose(const struct FMSPuzzlePose& Pose);
    void CapturePose(struct FMSPuzzlePose& Pose) const;
    void Sync();
    void Freeze();
    void Motion(MeatShield::SettleSample& Sample) const;
    bool Impact(FVector Point,FVector Direction,FVector& Torque);
    bool Collapsed() const;
    FVector ShoulderAxis() const;
    bool FaceAttached() const;
    bool TransferredAvatar=false;
    TArray<FTransform> RenderPose() const;
    UPROPERTY() TObjectPtr<USkeletalMeshComponent> Body;
    UPROPERTY() TObjectPtr<USkeletalMeshComponent> Face;
    UPROPERTY() TArray<TObjectPtr<UPrimitiveComponent>> Shapes;
    TArray<FVector> Sizes;
    TArray<uint8> Kinds;
private:
    TArray<FName> Bones;
    TArray<FTransform> LocalShapes;
};
