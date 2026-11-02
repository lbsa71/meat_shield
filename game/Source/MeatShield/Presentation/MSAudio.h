#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Subsystems/WorldSubsystem.h"
#include "MSAudio.generated.h"
class UAudioComponent;
enum class MSound { Shot, Impact, Step, Warning, Respawn };
UCLASS()
class UMSAudioSettings : public UWorldSubsystem
{
    GENERATED_BODY()
public:
    bool Muted = false;
};
UCLASS()
class UMSAudio : public UActorComponent
{
    GENERATED_BODY()
public:
    UMSAudio();
    void Play(MSound Kind, FVector Location, bool Spatial = false);
    virtual void TickComponent(float Delta, ELevelTick Type, FActorComponentTickFunction* Tick) override;
    int Played = 0;
private:
    UPROPERTY() TArray<TObjectPtr<UAudioComponent>> Voices;
    TArray<float> Remaining;
    int Next = 0;
};
