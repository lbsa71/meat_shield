#include "Diagnostics/MSSliceScenario.h"
#include "Gameplay/MSGameMode.h"
#include "Gameplay/MSCharacter.h"
#include "Gameplay/MSTurret.h"
#include "Presentation/MSAudio.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "AudioMixerBlueprintLibrary.h"
#include "EngineUtils.h"
#include "InputKeyEventArgs.h"
#include "UnrealClient.h"
#include "Misc/App.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
AMSSliceScenario::AMSSliceScenario() { PrimaryActorTick.bCanEverTick=true; }
void AMSSliceScenario::BeginPlay()
{
    Super::BeginPlay();
    FApp::SetUnfocusedVolumeMultiplier(1.f); // Only this opt-in output-capture fixture.
    UAudioMixerBlueprintLibrary::StartRecordingOutput(this,30);
    for(TActorIterator<AActor> It(GetWorld());It;++It) ++InitialActors;
}
void AMSSliceScenario::Check(bool Pass,const TCHAR* Name)
{
    ++Checks; Failed|=!Pass;
    UE_LOG(LogTemp,Display,TEXT("MS_SCENARIO %s %s"),Pass?TEXT("PASS"):TEXT("FAIL"),Name);
}
void AMSSliceScenario::Tick(float Delta)
{
    Super::Tick(Delta);
    auto* Mode=GetWorld()->GetAuthGameMode<AMSGameMode>();
    auto* Player=Cast<AMSCharacter>(UGameplayStatics::GetPlayerPawn(this,0));
    if(!Mode || !Player || !Mode->Turret) return;
    auto* Turret=Mode->Turret.Get(); auto* PC=Cast<APlayerController>(Player->GetController());
    if(!PC) return;
    auto Key=[&](FKey K,EInputEvent E) { PC->InputKey(FInputKeyEventArgs::CreateSimulated(K,E,E==IE_Released?0:1)); };
    Elapsed+=Delta;
    if(Stage==0)
    {
        if(!KeyDown) { Key(EKeys::W,IE_Pressed); Key(EKeys::LeftShift,IE_Pressed); KeyDown=true; }
        SprintObserved|=Player->Sprinting && Player->GetVelocity().Size2D()>600;
        if(Elapsed>1.1f && !KeyUp) { Key(EKeys::W,IE_Released); Key(EKeys::LeftShift,IE_Released); KeyUp=true; }
        if(Elapsed>1.6f && !SpawnCaptured)
        { FScreenshotRequest::RequestScreenshot(FPaths::ProjectSavedDir()/TEXT("turret-spawn.png"),true,false); SpawnCaptured=true; }
        if(Elapsed>2)
        {
            Check(Mode->Run.Health==100 && Turret->Shots==0,TEXT("protected_spawn"));
            Check(Player->GetActorLocation().X>-2700,TEXT("keyboard_movement"));
            Check(SprintObserved && !Player->Sprinting,TEXT("keyboard_sprint_release"));
            Player->SetActorLocation(FVector(-300,0,95)); Elapsed=0; Stage=1;
        }
    }
    else if(Stage==1)
    {
        if(Mode->Run.Health<=55 && Mode->Run.Health>0 && !HitCaptured)
        { FScreenshotRequest::RequestScreenshot(FPaths::ProjectSavedDir()/TEXT("turret-hit.png"),true,false); HitCaptured=true; }
        if(Mode->Run.Deployments>0 || Elapsed>14)
        {
            Check(Turret->Hits>=7,TEXT("exposed_trace_damage"));
            Check(Mode->Run.Deployments==1 && Mode->Run.Life==2 && Mode->Run.Health==100 && AMSGameMode::IsProtected(Player->GetActorLocation()),TEXT("single_lethal_reset"));
            BaselineHits=Turret->Hits; Player->SetActorLocation(FVector(-110,500,95)); Elapsed=0; Stage=2;
        }
    }
    else if(Stage==2 && Elapsed>4)
    {
        Check(Turret->Hits==BaselineHits && Mode->Run.Health==100,TEXT("cover_blocks_damage"));
        Check(Turret->Audio->Played>0 && Player->Audio->Played>0,TEXT("feedback_audio_triggered"));
        UAudioMixerBlueprintLibrary::StopRecordingOutput(this,EAudioRecordingExportType::WavFile,TEXT("turret-audio"),FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir()));
        Elapsed=0; Stage=3;
    }
    else if(Stage==3)
    {
        if(!RetryPressed) { Key(EKeys::R,IE_Pressed); RetryPressed=true; }
        if(Elapsed>.2f) Key(EKeys::R,IE_Released);
        RetryObserved|=Mode->Run.Life==3 && Mode->Run.Health==100 && AMSGameMode::IsProtected(Player->GetActorLocation());
        if(Elapsed>.6f && !JumpPressed) { Key(EKeys::SpaceBar,IE_Pressed); JumpPressed=true; }
        if(Elapsed>.8f && !JumpReleased) { Key(EKeys::SpaceBar,IE_Released); JumpReleased=true; }
        JumpObserved|=Elapsed>.6f && Player->GetActorLocation().Z>AMSGameMode::SpawnPoint().Z+40;
        if(Elapsed>1.15f && !LookInjected)
        { PC->InputKey(FInputKeyEventArgs::CreateSimulated(EKeys::MouseX,IE_Axis,12)); LookInjected=true; }
        LookObserved|=LookInjected && FMath::Abs(PC->GetControlRotation().Yaw)>1;
        if(Elapsed>1.5f && !MutePressed) { Key(EKeys::M,IE_Pressed); MutePressed=true; }
        if(Elapsed>1.7f) Key(EKeys::M,IE_Released);
        if(Elapsed>2.2f)
        {
            Check(RetryObserved,TEXT("keyboard_retry"));
            Check(JumpObserved,TEXT("keyboard_jump"));
            Check(LookObserved,TEXT("mouse_look"));
            Check(GetWorld()->GetSubsystem<UMSAudioSettings>()->Muted,TEXT("keyboard_mute"));
            int Actors=0; for(TActorIterator<AActor> It(GetWorld());It;++It) ++Actors;
            Check(Actors<=InitialActors+2,TEXT("retry_does_not_duplicate_world"));
            const FString Report=FString::Printf(TEXT("{\"status\":\"%s\",\"checks\":%d,\"shots\":%d,\"hits\":%d,\"deployments\":%llu,\"actors\":%d}"),Failed?TEXT("failed"):TEXT("passed"),Checks,Turret->Shots,Turret->Hits,Mode->Run.Deployments,Actors);
            FFileHelper::SaveStringToFile(Report,*(FPaths::ProjectSavedDir()/TEXT("turret-scenario.json")));
            UE_LOG(LogTemp,Display,TEXT("MS_SCENARIO_RESULT=%s"),*Report);
            if(Failed) FPlatformMisc::RequestExitWithStatus(false,1);
            else { Key(EKeys::Escape,IE_Pressed); Stage=4; Elapsed=0; }
        }
    }
    else if(Stage==4 && Elapsed>2)
    {
        Check(false,TEXT("escape_quit_did_not_exit"));
        FPlatformMisc::RequestExitWithStatus(false,1);
    }
}
