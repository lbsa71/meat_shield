#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Rules/SettleRules.h"
#include "MSCorpse.generated.h"
class UPrimitiveComponent;
class UPhysicsConstraintComponent;
class UPhysicalMaterial;
class UMSHumanRagdoll;
// REQ-003 / REQ-026: moving presentation first, separate immutable authority on commit.
UCLASS()
class AMSCorpse : public AActor
{
    GENERATED_BODY()
public:
    AMSCorpse();
    bool Initialize(uint64 Life,FVector Velocity,FVector ShotDirection,bool Crouched=false,class USkeletalMeshComponent* Source=nullptr);
    bool ApplyBulletImpact(UPrimitiveComponent* Part,FVector Point,FVector Direction);
    bool InitializeDowned(uint64 Id);
    int BulletImpacts=0;
    FVector LastImpactTorque=FVector::ZeroVector;
    virtual void Tick(float Delta) override;
    FVector Center() const;
    TArray<FTransform> Pose() const;
    bool Stable=false, PhysicsMoved=false;
    float MaxOffAxisRotation=0;
    uint64 LifeId=0;
    float CommittedAt=0;
    int32 ProxyCount() const { return Proxies.Num(); }
    bool IsHuman() const { return Human!=nullptr; }
    bool HumanFaceAttached() const;
    bool IsTransferredAvatar() const;
    TArray<FTransform> RenderPose() const;
private:
    friend class AMSSliceScenario; // Builds isolated collision fixtures using real body shapes.
    UPROPERTY() TArray<TObjectPtr<UPrimitiveComponent>> Bodies;
    UPROPERTY() TArray<TObjectPtr<UPrimitiveComponent>> Proxies;
    UPROPERTY() TArray<TObjectPtr<UPhysicsConstraintComponent>> Joints;
    UPROPERTY() TObjectPtr<UPhysicalMaterial> Material;
    UPROPERTY() TObjectPtr<UMSHumanRagdoll> Human;
    TArray<FTransform> RestReference;
    TArray<FVector> Dimensions;
    TArray<uint8> Shapes;
    FVector InitialCenter;
    FVector InitialShoulderAxis;
    float Age=0;
    MeatShield::SettleRules Settling;
    float NextSettleLog=2;
    int Part(FName Name,FVector Position,uint8 Shape,FVector Size,float Mass,FLinearColor Color,FRotator Rotation=FRotator::ZeroRotator);
    void Joint(int A,int B,FVector Position,float Swing);
    void CommitPose();
};
