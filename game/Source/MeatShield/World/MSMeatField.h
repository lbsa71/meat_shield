#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MSMeatField.generated.h"
class UWorld;
class AMSCorpse;
namespace MSMeatField { void Build(UWorld* World); }
UCLASS()
class AMSMeatPile : public AActor
{
    GENERATED_BODY()
public:
    AMSMeatPile();
    virtual void BeginPlay() override;
    virtual void Tick(float Delta) override;
private:
    UPROPERTY() TArray<TObjectPtr<AMSCorpse>> Bodies;
};
