#include "Gameplay/MSGameMode.h"
#include "Engine/World.h"
#include "Gameplay/MSCharacter.h"
#include "Gameplay/MSTurret.h"
#include "Presentation/MSHUD.h"
#include "Presentation/MSAudio.h"
#include "World/MSYard.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Diagnostics/MSSliceScenario.h"
#include "Misc/CommandLine.h"
AMSGameMode::AMSGameMode()
{
    DefaultPawnClass=AMSCharacter::StaticClass(); HUDClass=AMSHUD::StaticClass();
}
void AMSGameMode::BeginPlay()
{
    Super::BeginPlay();
    GetWorld()->SpawnActor<AMSYard>();
    Turret=GetWorld()->SpawnActor<AMSTurret>(FVector(1600,0,0),FRotator(0,180,0));
    const bool Scenario=FParse::Param(FCommandLine::Get(),TEXT("MSScenario"));
#if !UE_BUILD_SHIPPING
    if(Scenario) GetWorld()->SpawnActor<AMSSliceScenario>();
#endif
    UE_LOG(LogTemp,Display,TEXT("MS_SLICE_READY recipe=yard-01 rules=turret-01 scenario=%d"),Scenario);
}
void AMSGameMode::RestartPlayer(AController* Controller)
{
    Super::RestartPlayer(Controller);
    if(auto* Player=Cast<AMSCharacter>(Controller->GetPawn()))
    { Player->SetActorLocation(SpawnPoint(),false,nullptr,ETeleportType::TeleportPhysics); Controller->SetControlRotation(FRotator::ZeroRotator); }
}
void AMSGameMode::ApplyTurretHit(AMSCharacter* Player,uint64 Life)
{
    if(!Player || Life!=Run.Life) return;
    Player->Impact();
    const bool Lethal=Run.Damage(Life,15);
    if(Lethal)
    {
        UE_LOG(LogTemp,Display,TEXT("MS_LETHAL life=%llu deployments=%llu"),Life,Run.Deployments);
        Retry(Player);
    }
}
void AMSGameMode::Retry(AMSCharacter* Player)
{
    if(!Player) return;
    Run.Respawn();
    Player->GetCharacterMovement()->StopMovementImmediately();
    Player->SetActorLocationAndRotation(SpawnPoint(),FRotator::ZeroRotator,false,nullptr,ETeleportType::TeleportPhysics);
    if(Player->GetController()) Player->GetController()->SetControlRotation(FRotator::ZeroRotator);
    Player->Audio->Play(MSound::Respawn,SpawnPoint());
}
