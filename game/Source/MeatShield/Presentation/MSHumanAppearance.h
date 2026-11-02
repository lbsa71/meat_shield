#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MSHumanAppearance.generated.h"
class USkeletalMeshComponent;
UCLASS()
class UMSHumanAppearance : public UActorComponent
{
    GENERATED_BODY()
public:
    bool Initialize(uint64 Life,USkeletalMeshComponent* ExistingBody=nullptr,bool FirstPerson=false);
    void SetLife(uint64 Life,bool Neutral);
    uint64 Seed=0;
    UPROPERTY() TObjectPtr<USkeletalMeshComponent> Body;
    UPROPERTY() TObjectPtr<USkeletalMeshComponent> Face;
    UPROPERTY() TObjectPtr<USkeletalMeshComponent> Uniform;
    UPROPERTY() TObjectPtr<USkeletalMeshComponent> Vest;
};
