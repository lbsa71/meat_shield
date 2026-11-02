#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Rules/TurretRules.h"
#include "MSTurret.generated.h"
class UStaticMeshComponent;
class UPointLightComponent;
class UMSAudio;
UCLASS()
class AMSTurret : public AActor
{
    GENERATED_BODY()
public:
    AMSTurret();
    virtual void BeginPlay() override;
    virtual void Tick(float Delta) override;
    MeatShield::ThreatRules Rules;
    int Shots = 0, Hits = 0;
    bool HasSight = false;
    UPROPERTY() TObjectPtr<UMSAudio> Audio;
private:
    UPROPERTY() TObjectPtr<USceneComponent> Aim;
    UPROPERTY() TObjectPtr<USceneComponent> Barrel;
    UPROPERTY() TObjectPtr<USceneComponent> Muzzle;
    UPROPERTY() TObjectPtr<UStaticMeshComponent> Flash;
    UPROPERTY() TObjectPtr<UStaticMeshComponent> Tracer;
    UPROPERTY() TObjectPtr<UStaticMeshComponent> Sensor;
    UPROPERTY() TObjectPtr<UPointLightComponent> Light;
    float FlashTime = 0, Recoil = 0, Spin = 0;
    void Fire(class AMSCharacter* Target);
};
