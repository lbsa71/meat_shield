#include "Gameplay/MSGameMode.h"
#include "Engine/World.h"
#include "Gameplay/MSCharacter.h"
#include "Gameplay/MSTurret.h"
#include "Presentation/MSHUD.h"
#include "Presentation/MSAudio.h"
#include "Presentation/MSDeathCamera.h"
#include "World/MSCorpse.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "World/MSYard.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Diagnostics/MSSliceScenario.h"
#include "Diagnostics/MSRealismScenario.h"
#include "Diagnostics/MSPuzzleScenario.h"
#include "Presentation/MSHumanAppearance.h"
#include "Misc/CommandLine.h"
AMSGameMode::AMSGameMode()
{
    PrimaryActorTick.bCanEverTick=true; PrimaryActorTick.TickGroup=TG_PostPhysics;
    DefaultPawnClass=AMSCharacter::StaticClass(); HUDClass=AMSHUD::StaticClass();
}
void AMSGameMode::BeginPlay()
{
    Super::BeginPlay();
    GetWorld()->SpawnActor<AMSYard>();
    Turret=GetWorld()->SpawnActor<AMSTurret>(FVector(1600,0,0),FRotator(0,180,0));
    const bool Scenario=FParse::Param(FCommandLine::Get(),TEXT("MSScenario"));
#if !UE_BUILD_SHIPPING
    if(FParse::Param(FCommandLine::Get(),TEXT("MSPuzzleScenario"))) GetWorld()->SpawnActor<AMSPuzzleScenario>();
    if(Scenario) GetWorld()->SpawnActor<AMSSliceScenario>();
    if(FParse::Param(FCommandLine::Get(),TEXT("MSRealismGallery")) || FParse::Param(FCommandLine::Get(),TEXT("MSRealismBenchmark")) || FParse::Param(FCommandLine::Get(),TEXT("MSArmProbe"))) GetWorld()->SpawnActor<AMSRealismScenario>();
#endif
    UE_LOG(LogTemp,Display,TEXT("MS_SLICE_READY recipe=yard-01 rules=turret-01 scenario=%d"),Scenario);
}
void AMSGameMode::RestartPlayer(AController* Controller)
{
    Super::RestartPlayer(Controller);
    if(auto* Player=Cast<AMSCharacter>(Controller->GetPawn()))
    { Player->SetActorLocation(SpawnPoint(),false,nullptr,ETeleportType::TeleportPhysics); Controller->SetControlRotation(FRotator::ZeroRotator); }
}
void AMSGameMode::ApplyTurretHit(AMSCharacter* Player,uint64 Life,FVector ShotDirection,FVector ImpactPoint)
{
    if(!Player || Life!=Run.Life || Death.Active()) return;
    Player->Impact();
    const bool Lethal=Run.Damage(Life,15);
    if(Lethal)
    {
        UE_LOG(LogTemp,Display,TEXT("MS_LETHAL life=%llu deployments=%llu"),Life,Run.Deployments);
        BeginDeath(Player,ShotDirection,ImpactPoint.IsNearlyZero()?Player->TargetPoint():ImpactPoint);
    }
}
void AMSGameMode::Retry(AMSCharacter* Player)
{
    if(!Player || Death.Active()) return;
    RestorePlayer(Player);
}
void AMSGameMode::BeginDeath(AMSCharacter* Player,FVector ShotDirection,FVector ImpactPoint)
{
    if(!Death.Start(Run.Life)) return;
    auto* Body=GetWorld()->SpawnActor<AMSCorpse>(Player->GetActorLocation()+FVector(0,0,Player->bIsCrouched?-14:0),Player->GetActorRotation());
    Body->Initialize(Run.Life,Player->GetVelocity(),ShotDirection,Player->bIsCrouched,Player->Human?Player->GetMesh():nullptr); Corpses.Add(Body);
    Body->ApplyBulletImpact(nullptr,ImpactPoint,ShotDirection);
    if(!DeathCamera) DeathCamera=GetWorld()->SpawnActor<AMSDeathCamera>();
    DeathCamera->BeginDeath(Player->Camera->GetComponentTransform(),Player->GetActorForwardVector(),Body,SpawnPoint()+FVector(0,0,Player->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()-90+Player->EyeOffset()),Player);
    DyingPlayer=Player;
    Player->GetCharacterMovement()->StopMovementImmediately(); Player->GetCharacterMovement()->DisableMovement();
    Player->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision); Player->SetActorHiddenInGame(true);
    if(auto* PC=Cast<APlayerController>(Player->GetController()))
    { PC->SetIgnoreMoveInput(true); PC->SetIgnoreLookInput(true); PC->SetViewTarget(DeathCamera); }
}
void AMSGameMode::Tick(float Delta)
{
    Super::Tick(Delta); if(!Death.Active()) return;
    const auto Out=Death.Step(Delta);
    if(DeathCamera) DeathCamera->UpdateDeath(Out.Completed?MeatShield::DeathPhase::Return:Death.Phase,Out.Completed?1.f:float(Death.Progress()));
    if(Out.Completed && DyingPlayer) { RestorePlayer(DyingPlayer); DyingPlayer=nullptr; }
}
void AMSGameMode::RestorePlayer(AMSCharacter* Player)
{
    Run.Respawn();
    Player->RefreshAvatar(Run.Life);
    Player->SetActorHiddenInGame(false); Player->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    Player->GetCharacterMovement()->SetMovementMode(MOVE_Walking); Player->StopJumping();
    Player->GetCharacterMovement()->StopMovementImmediately();
    Player->SetActorLocationAndRotation(SpawnPoint()+FVector(0,0,Player->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()-90),FRotator::ZeroRotator,false,nullptr,ETeleportType::TeleportPhysics);
    if(auto* PC=Cast<APlayerController>(Player->GetController()))
    { PC->SetControlRotation(FRotator::ZeroRotator); PC->SetIgnoreMoveInput(false); PC->SetIgnoreLookInput(false); PC->SetViewTarget(Player); }
    Player->Audio->Play(MSound::Respawn,SpawnPoint());
}
