#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Rules/TurretRules.h"
#include "MSGameMode.generated.h"
class AMSTurret;
class AMSCharacter;
UCLASS()
class AMSGameMode : public AGameModeBase
{
    GENERATED_BODY()
public:
    AMSGameMode();
    virtual void BeginPlay() override;
    virtual void RestartPlayer(AController* Player) override;
    void ApplyTurretHit(AMSCharacter* Player, uint64 Life);
    void Retry(AMSCharacter* Player);
    MeatShield::LifeState Run;
    UPROPERTY() TObjectPtr<AMSTurret> Turret;
    static FVector SpawnPoint() { return FVector(-2900,0,110); }
    static bool IsProtected(FVector P) { return P.X < -1950; }
};
