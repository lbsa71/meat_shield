#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MSPuzzleScenario.generated.h"
UCLASS()
class AMSPuzzleScenario : public AActor {
    GENERATED_BODY()
public:
    AMSPuzzleScenario();
    virtual void Tick(float Delta) override;
private:
    float Clock=0,Total=0; int Stage=0,Checks=0,Failures=0,Waypoint=0; uint32 InitialHash=0;
    UPROPERTY() TObjectPtr<class UStaticMeshComponent> Ceiling;
    UPROPERTY() TObjectPtr<class ACameraActor> Overview;
    void Check(bool Pass,const TCHAR* Label);
    uint32 LayoutHash() const;
};
