#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MSYard.generated.h"
UCLASS()
class AMSYard : public AActor
{
    GENERATED_BODY()
public:
    AMSYard();
    virtual void BeginPlay() override;
};
