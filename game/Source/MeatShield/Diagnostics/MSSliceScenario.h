#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MSSliceScenario.generated.h"
class AMSCorpse;
// Explicit opt-in development fixture. Never spawned on normal launches.
UCLASS()
class AMSSliceScenario : public AActor
{
    GENERATED_BODY()
public:
    AMSSliceScenario();
    virtual void BeginPlay() override;
    virtual void Tick(float Delta) override;
private:
    bool Failed=false, KeyDown=false, KeyUp=false, SprintObserved=false;
    bool SpawnCaptured=false, HitCaptured=false, RetryPressed=false, RetryObserved=false;
    bool JumpPressed=false, JumpReleased=false, JumpObserved=false, MutePressed=false;
    bool LookInjected=false, LookObserved=false;
    bool DeathObserved=false;
    bool DuplicateChecked=false, DeathCaptured=false;
    UPROPERTY() TObjectPtr<AMSCorpse> FirstCorpse;
    TArray<FTransform> FirstPose;
    double DeathStartedAt=0;
    bool CameraClearanceChecked=false;
    float Elapsed=0;
    int Stage=0, BaselineHits=0, InitialActors=0, Checks=0;
    void Check(bool Pass,const TCHAR* Name);
};
