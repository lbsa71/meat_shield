#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Rules/TurretRules.h"
#include "Rules/DeathRules.h"
#include "MSGameMode.generated.h"
class AMSTurret;
class AMSCharacter;
class AMSCorpse;
class AMSDeathCamera;
UCLASS()
class AMSGameMode : public AGameModeBase
{
    GENERATED_BODY()
public:
    AMSGameMode();
    virtual void BeginPlay() override;
    virtual void Tick(float Delta) override;
    virtual void RestartPlayer(AController* Player) override;
    void ApplyTurretHit(AMSCharacter* Player, uint64 Life,FVector ShotDirection=FVector(-1,0,0),FVector ImpactPoint=FVector::ZeroVector);
    void Retry(AMSCharacter* Player);
    bool IsDeathActive() const { return Death.Active(); }
    int32 CorpseCount() const { return Corpses.Num(); }
    const TArray<TObjectPtr<AMSCorpse>>& GetCorpses() const { return Corpses; }
    MeatShield::DeathTimeline Death;
    MeatShield::LifeState Run;
    UPROPERTY() TObjectPtr<AMSTurret> Turret;
    static FVector SpawnPoint() { return FVector(-2900,0,110); }
    static bool IsProtected(FVector P) { return P.X < -1950; }
private:
    UPROPERTY() TArray<TObjectPtr<AMSCorpse>> Corpses;
    UPROPERTY() TObjectPtr<AMSDeathCamera> DeathCamera;
    UPROPERTY() TObjectPtr<AMSCharacter> DyingPlayer;
    void BeginDeath(AMSCharacter* Player,FVector ShotDirection,FVector ImpactPoint);
    void RestorePlayer(AMSCharacter* Player);
};
