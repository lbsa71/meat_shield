#pragma once
#include "CoreMinimal.h"
#include "Camera/CameraActor.h"
#include "Rules/DeathRules.h"
#include "MSDeathCamera.generated.h"
class AMSCorpse;
UCLASS()
class AMSDeathCamera : public ACameraActor
{
    GENERATED_BODY()
public:
    void BeginDeath(FTransform Eye,FVector Forward,AMSCorpse* Body,FVector ReturnEye,AActor* Avatar);
    void UpdateDeath(MeatShield::DeathPhase Phase,float Progress);
private:
    UPROPERTY() TObjectPtr<AMSCorpse> Corpse;
    FVector Start,Zoom,Home;
    FQuat StartRotation;
};
