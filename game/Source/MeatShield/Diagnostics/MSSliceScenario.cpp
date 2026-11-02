#include "Diagnostics/MSSliceScenario.h"
#include "Gameplay/MSGameMode.h"
#include "Gameplay/MSCharacter.h"
#include "Gameplay/MSTurret.h"
#include "World/MSCorpse.h"
#include "Presentation/MSAudio.h"
#include "Presentation/MSVisuals.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/CameraComponent.h"
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
    // A cold renderer's long first frame must not press and release an input
    // in one tick. Only fixture scheduling is bounded; gameplay/camera clocks
    // and the measured death-to-control duration still use real world time.
    Elapsed+=FMath::Min(Delta,.1f);
    if(Stage==0)
    {
        if(!KeyDown) { Key(EKeys::W,IE_Pressed); Key(EKeys::LeftShift,IE_Pressed); KeyDown=true; }
        SprintObserved|=Player->Sprinting && Player->GetVelocity().Size2D()>600;
        if(Elapsed>1.1f && !KeyUp) { Key(EKeys::W,IE_Released); Key(EKeys::LeftShift,IE_Released); KeyUp=true; }
        if(Elapsed>1.6f && !SpawnCaptured)
        { FScreenshotRequest::RequestScreenshot(FPaths::ProjectSavedDir()/TEXT("turret-spawn.png"),true,false); SpawnCaptured=true; }
        if(Elapsed>1.25f && !CrouchPressed) { Key(EKeys::LeftControl,IE_Pressed); CrouchPressed=true; }
        if(Elapsed>1.65f && !CrouchChecked)
        {
            Check(Player->bIsCrouched && Player->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()<=50,TEXT("native_crouch_capsule"));
            Check(Player->Camera->GetComponentLocation().Z<100 && Player->TargetPoint().Z<90,TEXT("crouched_eye_and_target"));
            if(Player->bIsCrouched) Ceiling=MSVisuals::Part(this,nullptr,TEXT("ClearanceFixture"),FVector(Player->GetActorLocation().X,Player->GetActorLocation().Y,130),{3,3,.3f},FLinearColor(.4f,.4f,.4f),true);
            CrouchChecked=true;
        }
        if(Elapsed>1.85f && !CrouchReleased) { Key(EKeys::LeftControl,IE_Released); CrouchReleased=true; }
        if(Elapsed>2.2f && !ClearanceChecked)
        { Check(Player->bIsCrouched,TEXT("ceiling_prevents_standing")); if(Ceiling) Ceiling->DestroyComponent(); ClearanceChecked=true; }
        if(Elapsed>2.8f && !StandingChecked)
        { Check(!Player->bIsCrouched && Player->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()>=90,TEXT("stands_after_clearance")); StandingChecked=true; }
        if(Elapsed>3.1f)
        {
            int Downed=0; for(TActorIterator<AMSCorpse> It(GetWorld());It;++It) if(It->ActorHasTag(TEXT("PreDowned"))) ++Downed;
            Check(Downed>=40,TEXT("seeded_downed_field"));
            Check(Mode->Run.Health==100 && Turret->Shots==0,TEXT("protected_spawn"));
            Check(Player->GetActorLocation().X>-2700,TEXT("keyboard_movement"));
            Check(SprintObserved && !Player->Sprinting,TEXT("keyboard_sprint_release"));
            Player->SetActorLocation(FVector(-300,0,95)); Elapsed=0; Stage=1;
        }
    }
    else if(Stage==1)
    {
        DeathObserved|=Mode->IsDeathActive();
        if(Mode->IsDeathActive() && !DuplicateChecked)
        {
            DeathStartedAt=GetWorld()->GetTimeSeconds()-Mode->Death.Elapsed;
            Check(PC->GetViewTarget()!=Player && Mode->Run.Health==0,TEXT("death_view_and_control_hold"));
            Mode->ApplyTurretHit(Player,Mode->Run.Life); Mode->Retry(Player);
            Check(Mode->CorpseCount()==1 && Mode->Run.Life==1,TEXT("duplicate_lethal_and_retry_ignored"));
            DuplicateChecked=true;
        }
        if(Mode->IsDeathActive() && Mode->Death.Elapsed>.4 && !DeathCaptured)
        { FScreenshotRequest::RequestScreenshot(FPaths::ProjectSavedDir()/TEXT("ragdoll-fall.png"),true,false); DeathCaptured=true;
            Check(Mode->CorpseCount()>0 && FMath::Abs(Mode->GetCorpses()[0]->Pose()[1].GetUnitAxis(EAxis::Y).Z)>.8f,TEXT("visible_sideways_torso_roll")); }
        if(Mode->IsDeathActive() && Mode->Death.Elapsed>.5 && !LiveMeatChecked && Mode->CorpseCount()>0)
        {
            auto* Body=Mode->GetCorpses()[0].Get(); const FVector P=Body->Center(); FHitResult Hit;
            GetWorld()->LineTraceSingleByChannel(Hit,P+FVector(0,0,180),P-FVector(0,0,30),ECC_Visibility);
            Check(Hit.GetActor()==Body,TEXT("falling_meat_blocks_bullets"));
            Check(!Body->Stable,TEXT("moving_body_not_locked_early"));
            const int Impacts=Body->BulletImpacts;
            Turret->ResolveShot(P+FVector(0,-180,12),P+FVector(0,5,12),nullptr);
            Check(Body->BulletImpacts>Impacts && Body->LastImpactTorque.Size()>1000,TEXT("offcentre_bullet_torque")); LiveMeatChecked=true;
        }
        if(Mode->Death.Phase==MeatShield::DeathPhase::Observe && !CameraClearanceChecked && Mode->CorpseCount()>0)
        {
            Check(FVector::Distance(PC->GetViewTarget()->GetActorLocation(),Mode->GetCorpses()[0]->Center())>220,TEXT("camera_clears_avatar_capsule"));
            CameraClearanceChecked=true;
        }
        if(Mode->Run.Health<=55 && Mode->Run.Health>0 && !HitCaptured)
        { FScreenshotRequest::RequestScreenshot(FPaths::ProjectSavedDir()/TEXT("turret-hit.png"),true,false); HitCaptured=true; }
        if(Mode->Run.Life>1 && !Mode->IsDeathActive() && CameraReturnSeconds==0) CameraReturnSeconds=GetWorld()->GetTimeSeconds()-DeathStartedAt;
        if((Mode->Run.Life>1 && !Mode->IsDeathActive() && Mode->CorpseCount()>0 && Mode->GetCorpses()[0]->Stable) || Elapsed>18)
        {
            Check(DeathObserved,TEXT("death_camera_sequence"));
            Check(Mode->CorpseCount()==1,TEXT("one_persistent_corpse"));
            Check(DeathStartedAt>0 && CameraReturnSeconds>0 && CameraReturnSeconds<=1.2,TEXT("bounded_death_camera_duration"));
            Check(Turret->Hits>=7,TEXT("exposed_trace_damage"));
            Check(Mode->Run.Deployments==1 && Mode->Run.Life==2 && Mode->Run.Health==100 && AMSGameMode::IsProtected(Player->GetActorLocation()),TEXT("single_lethal_reset"));
            if(Mode->CorpseCount()>0)
            {
                FirstCorpse=Mode->GetCorpses()[0]; FirstPose=FirstCorpse->Pose();
                Check(FirstPose.Num()>1 && FMath::Abs(FirstPose[1].GetUnitAxis(EAxis::Z).Z)<.85 && FirstCorpse->Center().Z<80,TEXT("body_topples_to_ground"));
                Check(FirstCorpse->PhysicsMoved && FirstCorpse->Stable && FirstCorpse->ProxyCount()==15,TEXT("ragdoll_moves_then_commits"));
                Check(FirstCorpse->CommittedAt>=1.2f,TEXT("no_fixed_075_second_commit"));
                FHitResult Hit; const FVector Center=FirstCorpse->Center();
                GetWorld()->LineTraceSingleByChannel(Hit,Center+FVector(0,0,180),Center-FVector(0,0,50),ECC_Visibility);
                Check(Hit.GetActor()==FirstCorpse,TEXT("committed_body_blocks_queries"));
            }
            BaselineHits=Turret->Hits; Player->SetActorLocation(FVector(-110,500,95)); Elapsed=0; Stage=2;
        }
    }
    else if(Stage==2 && Elapsed>4)
    {
        Check(Turret->Hits==BaselineHits && Mode->Run.Health==100,TEXT("cover_blocks_damage"));
        Check(Turret->Audio->Played>0 && Player->Audio->Played>0,TEXT("feedback_audio_triggered"));
        Key(EKeys::LeftControl,IE_Pressed); Player->SetActorLocation(FVector(-700,0,95)); Elapsed=0; Stage=5;
    }
    else if(Stage==3)
    {
        if(!RetryPressed) { Key(EKeys::R,IE_Pressed); RetryPressed=true; }
        if(Elapsed>.2f) Key(EKeys::R,IE_Released);
        RetryObserved|=Mode->Run.Life==4 && Mode->Run.Health==100 && AMSGameMode::IsProtected(Player->GetActorLocation());
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
            Check(Actors<=InitialActors+1+Mode->CorpseCount(),TEXT("one_reused_death_camera"));
            bool Kept=Mode->CorpseCount()==2;
            for(AMSCorpse* Body : Mode->GetCorpses()) Kept&=IsValid(Body) && Body->Stable && !Body->IsHidden() && Body->ProxyCount()==15;
            Check(Kept,TEXT("manual_retry_keeps_all_corpses"));
            const FString Report=FString::Printf(TEXT("{\"status\":\"%s\",\"checks\":%d,\"shots\":%d,\"hits\":%d,\"deployments\":%llu,\"corpses\":%d,\"actors\":%d}"),Failed?TEXT("failed"):TEXT("passed"),Checks,Turret->Shots,Turret->Hits,Mode->Run.Deployments,Mode->CorpseCount(),Actors);
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
    else if(Stage==5 && ((Mode->Run.Life>2 && !Mode->IsDeathActive() && Mode->CorpseCount()==2 && Mode->GetCorpses()[1]->Stable) || Elapsed>18))
    {
        Check(Mode->CorpseCount()==2 && Mode->Run.Deployments==2,TEXT("two_distinct_persistent_deaths"));
        Check(Mode->CorpseCount()==2 && Mode->GetCorpses()[1]->Stable && Mode->GetCorpses()[1]->Center().Z<80 && Mode->GetCorpses()[1]->CommittedAt>=1.2f,TEXT("crouched_death_collapses_and_settles"));
        Check(PC->GetViewTarget()==Player && Mode->Run.Health==100,TEXT("first_person_control_returns"));
        bool Unchanged=IsValid(FirstCorpse) && FirstPose.Num()==FirstCorpse->Pose().Num();
        if(Unchanged) { const auto Current=FirstCorpse->Pose(); for(int I=0;I<Current.Num();++I) Unchanged&=Current[I].Equals(FirstPose[I],.001f); }
        Check(Unchanged,TEXT("older_body_pose_never_changes"));
        Key(EKeys::LeftControl,IE_Released); Player->SetActorLocation(FVector(-1400,0,95)); PC->SetControlRotation(FRotator(-12,0,0)); Elapsed=0; Stage=6;
    }
    else if(Stage==6)
    {
        if(Elapsed>.2f && !TwoCaptured)
        { FScreenshotRequest::RequestScreenshot(FPaths::ProjectSavedDir()/TEXT("ragdoll-two.png"),true,false); TwoCaptured=true; }
        if(Elapsed>.4f) { Key(EKeys::LeftControl,IE_Pressed); Player->SetActorLocation(FirstCorpse->Center()+FVector(-200,0,50-FirstCorpse->Center().Z)); Elapsed=0; Stage=7; }
    }
    else if(Stage==7)
    {
        if(Elapsed>.35f && !CoverCaptured)
        { FScreenshotRequest::RequestScreenshot(FPaths::ProjectSavedDir()/TEXT("meat-cover.png"),true,false); CoverCaptured=true; }
        if(Elapsed<=.5f) return;
        const FVector P=FirstCorpse->Center(), Start=P+FVector(200,0,0), End=Player->GetActorLocation()+FVector(0,0,P.Z-Player->GetActorLocation().Z);
        const int Health=Mode->Run.Health;
        const FHitResult Blocked=Turret->ResolveShot(Start,End,Player);
        Check(!FirstCorpse->ApplyBulletImpact(nullptr,P,FVector(1,0,0)),TEXT("fixed_body_rejects_impulse"));
        Check(Blocked.GetActor()==FirstCorpse && Mode->Run.Health==Health,TEXT("meat_intercepts_real_damage_trace"));
        const FHitResult Clear=Turret->ResolveShot(Start+FVector(0,500,0),End,Player);
        Check(Clear.GetActor()==Player && Mode->Run.Health==Health-15,TEXT("uncovered_trace_still_damages"));
        Key(EKeys::LeftControl,IE_Released);
        UAudioMixerBlueprintLibrary::StopRecordingOutput(this,EAudioRecordingExportType::WavFile,TEXT("turret-audio"),FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir())); Elapsed=0; Stage=3;
    }
}
