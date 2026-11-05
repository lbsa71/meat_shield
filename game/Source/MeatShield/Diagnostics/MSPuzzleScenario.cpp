#include "Diagnostics/MSPuzzleScenario.h"
#include "Gameplay/MSCharacter.h"
#include "Gameplay/MSGameMode.h"
#include "Gameplay/MSTurret.h"
#include "World/MSCorpse.h"
#include "Rules/PuzzleRules.h"
#include "Presentation/MSVisuals.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "EngineUtils.h"
#include "InputKeyEventArgs.h"
#include "UnrealClient.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#include "Misc/Crc.h"
#include "HAL/PlatformMisc.h"
AMSPuzzleScenario::AMSPuzzleScenario() { PrimaryActorTick.bCanEverTick=true; PrimaryActorTick.TickGroup=TG_PostPhysics; }
void AMSPuzzleScenario::Check(bool Pass,const TCHAR* Label) { ++Checks; if(!Pass) ++Failures; UE_LOG(LogTemp,Display,TEXT("MS_PUZZLE_CHECK %s %s"),Pass?TEXT("PASS"):TEXT("FAIL"),Label); }
uint32 AMSPuzzleScenario::LayoutHash() const {
    TArray<AMSCorpse*> Bodies; for(TActorIterator<AMSCorpse> I(GetWorld());I;++I) if(I->ActorHasTag(TEXT("PreDowned"))) Bodies.Add(*I);
    Bodies.Sort([](const AMSCorpse& A,const AMSCorpse& B){return A.LifeId<B.LifeId;}); FString S;
    for(auto* B:Bodies) { S+=FString::Printf(TEXT("%llu"),B->LifeId); for(const auto& P:B->Pose()) S+=P.ToString(); for(const auto& P:B->RenderPose()) S+=P.ToString(); }
    return FCrc::StrCrc32(*S);
}
void AMSPuzzleScenario::Tick(float Delta)
{
    Super::Tick(Delta); Clock+=FMath::Min(Delta,.1f); Total+=FMath::Min(Delta,.1f);
    auto* Mode=GetWorld()->GetAuthGameMode<AMSGameMode>(); auto* PC=GetWorld()->GetFirstPlayerController();
    auto* P=PC?Cast<AMSCharacter>(PC->GetPawn()):nullptr; if(!P || !Mode || !Mode->Turret) return;
    auto* T=Mode->Turret.Get(); T->SetActorTickEnabled(false);
    auto Key=[&](FKey K,EInputEvent E){PC->InputKey(FInputKeyEventArgs::CreateSimulated(K,E,E==IE_Released?0:1));};
    auto Capture=[&](const TCHAR* Name){FScreenshotRequest::RequestScreenshot(FPaths::ProjectSavedDir()/Name,true,false);};
    auto Place=[&](double X,double Y){P->GetCharacterMovement()->StopMovementImmediately();P->SetActorLocation(FVector(X,Y,MeatShield::Puzzle::Height(X,Y)+P->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()+2),false,nullptr,ETeleportType::TeleportPhysics);};
    if(Total>110) { Check(false,TEXT("bounded_scenario_timeout")); Stage=10; }
    if(Stage==0 && Clock>.8f) {
        int Count=0; bool Stable=true; for(TActorIterator<AMSCorpse> I(GetWorld());I;++I) if(I->ActorHasTag(TEXT("PreDowned"))) { ++Count; Stable&=I->Stable && I->ProxyCount()==24; }
        Check(Count==18 && Stable,TEXT("initial_bodies_ready_without_live_simulation"));
        Check(T->ShotOrigin().Z>=25 && T->ShotOrigin().Z<=45,TEXT("low_muzzle_under_standing_aim"));
        bool Surface=true; int Rays=0;
        for(int X=-1800;X<1500;X+=150) for(int Y=-1200;Y<=1200;Y+=150) {
            FHitResult H; FCollisionQueryParams Q; for(TActorIterator<AMSCorpse> I(GetWorld());I;++I) Q.AddIgnoredActor(*I); Q.AddIgnoredActor(P); Q.AddIgnoredActor(T);
            const bool Hit=GetWorld()->LineTraceSingleByChannel(H,FVector(X,Y,500),FVector(X,Y,-100),ECC_Visibility,Q);
            Surface &= Hit && FMath::Abs(H.ImpactPoint.Z-MeatShield::Puzzle::Height(X,Y))<.1; ++Rays;
        }
        Check(Surface && Rays>300,TEXT("rendered_heightfield_matches_collision_at_grid_vertices"));
        InitialHash=LayoutHash(); UE_LOG(LogTemp,Display,TEXT("MS_PUZZLE_HASH=%u"),InitialHash);
        Capture(TEXT("puzzle-start.png")); Stage=1; Clock=0;
    } else if(Stage==1 && Clock>.4f) { Key(EKeys::Z,IE_Pressed); Stage=2; Clock=0;
    } else if(Stage==2 && Clock>.35f) {
        Key(EKeys::Z,IE_Released);
        Check(P->LowCrawling && P->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()==18,TEXT("Z_enters_36cm_crawl"));
        Check(P->Camera->GetRelativeLocation().Z<=12.1 && P->GetCharacterMovement()->MaxWalkSpeed==85,TEXT("crawl_eye_and_speed"));
        const FVector Torso=P->GetMesh()->GetSocketLocation(TEXT("neck_01"))-P->GetMesh()->GetSocketLocation(TEXT("pelvis"));
        Check(FMath::Abs(Torso.GetSafeNormal().Z)<.25,TEXT("live_human_is_prone"));
        Ceiling=MSVisuals::Part(this,nullptr,TEXT("CrawlCeiling"),P->GetActorLocation()+FVector(0,0,50),{3,3,.2f},FLinearColor(.3f,.3f,.3f),true);
        Check(!P->SetLowCrawl(false) && P->LowCrawling,TEXT("blocked_expansion_preserves_crawl"));
        Key(EKeys::LeftShift,IE_Pressed); Key(EKeys::SpaceBar,IE_Pressed); Key(EKeys::W,IE_Pressed);
        Stage=3; Clock=0;
    } else if(Stage==3 && Clock>.5f) {
        Check(P->LowCrawling && P->GetVelocity().Size2D()<=86 && !P->GetCharacterMovement()->IsFalling(),TEXT("sprint_jump_do_not_bypass_crawl"));
        Key(EKeys::LeftShift,IE_Released); Key(EKeys::SpaceBar,IE_Released); Key(EKeys::W,IE_Released); Ceiling->DestroyComponent();
        Check(P->SetLowCrawl(false) && P->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()==90,TEXT("stand_after_clearance"));
        P->SetLowCrawl(true);
        AMSCorpse* First=nullptr; for(TActorIterator<AMSCorpse> I(GetWorld());I;++I) if(I->LifeId==((uint64(1)<<63)+8)) First=*I;
        if(First) {
            const FVector Behind=First->Center()+(First->Center()-T->ShotOrigin()).GetSafeNormal2D()*100;
            Place(Behind.X,Behind.Y); T->Tick(1.f); const int Health=Mode->Run.Health;
            const auto Hit=T->ResolveShot(T->ShotOrigin(),P->TargetPoint(),P);
            UE_LOG(LogTemp,Display,TEXT("MS_PUZZLE_SHELTER body=%s player=%s hit=%s"),*First->Center().ToString(),*P->GetActorLocation().ToString(),*GetNameSafe(Hit.GetActor()));
            Check(Hit.GetActor()==First && Mode->Run.Health==Health,TEXT("one_actual_body_stops_crawl_damage"));
            Check(P->SetLowCrawl(false),TEXT("single_shelter_has_standing_clearance"));
            const auto Standing=T->ResolveShot(T->ShotOrigin(),P->TargetPoint(),P);
            Check(Standing.GetActor()==P,TEXT("same_single_body_exposes_standing_player"));
            P->SetLowCrawl(true); Place(Behind.X,Behind.Y);

        } else Check(false,TEXT("first_shelter_exists"));
        PC->SetControlRotation(FRotator(0,0,0)); Stage=4; Clock=0;
    } else if(Stage==4 && Clock>.4f) { Capture(TEXT("puzzle-single-cover.png")); Stage=5; Clock=0;
    } else if(Stage==5 && Clock>.4f) {
        Place(1200,-450); const int Health=Mode->Run.Health;
        const auto Hit=T->ResolveShot(T->ShotOrigin(),P->TargetPoint(),P);
        Check(Hit.GetActor()==P && Mode->Run.Health==Health-15,TEXT("open_crawler_takes_damage"));
        Mode->Retry(P); Check(P->LowCrawling && LayoutHash()==InitialHash,TEXT("retry_preserves_stance_and_all_initial_poses"));
        Place(-1900,-400); Stage=6; Clock=0;
    } else if(Stage==6) {
        const auto Points=MeatShield::Puzzle::Route();
        const FVector2D Here(P->GetActorLocation()); const FVector2D Difference=FVector2D(Points[Waypoint].X,Points[Waypoint].Y)-Here;
        P->AddMovementInput(FVector(Difference.X,Difference.Y,0).GetSafeNormal());
        PC->SetControlRotation(FVector(Difference.X,Difference.Y,0).Rotation());
        if(Difference.Size()<40) { ++Waypoint; UE_LOG(LogTemp,Display,TEXT("MS_PUZZLE_ROUTE waypoint=%d location=%s"),Waypoint,*P->GetActorLocation().ToString()); }
        if(Waypoint==7) { P->GetCharacterMovement()->StopMovementImmediately(); Check(true,TEXT("native_low_crawl_traverses_hilly_route")); Capture(TEXT("puzzle-route-end.png")); Stage=7; Clock=0; }
    } else if(Stage==7 && Clock>.5f) {
        for(int I=0;I<7;++I) Mode->ApplyTurretHit(P,Mode->Run.Life,FVector(-1,0,0),P->TargetPoint());
        Check(Mode->IsDeathActive() && Mode->CorpseCount()==1 && Mode->GetCorpses()[0]->IsTransferredAvatar(),TEXT("prone_pose_transfers_to_death")); Stage=8; Clock=0;
    } else if(Stage==8 && Clock>2) {
        Check(!Mode->IsDeathActive() && Mode->Run.Health==100 && P->LowCrawling,TEXT("crawl_death_returns_to_start"));
        Check(LayoutHash()==InitialHash,TEXT("deaths_do_not_rearrange_initial_puzzle"));
        Overview=GetWorld()->SpawnActor<ACameraActor>(FVector(-1000,-2000,2100),FRotator(-48,70,0)); Overview->GetCameraComponent()->SetFieldOfView(90); PC->SetViewTarget(Overview); Stage=9; Clock=0;
    } else if(Stage==9 && Clock>.6f) { Capture(TEXT("puzzle-overview.png")); Stage=10; Clock=0;
    } else if(Stage==10 && Clock>.6f) {
        const FString Report=FString::Printf(TEXT("{\"status\":\"%s\",\"checks\":%d,\"failures\":%d,\"layout_hash\":%u,\"route_waypoints\":%d}"),Failures?TEXT("failed"):TEXT("passed"),Checks,Failures,InitialHash,Waypoint);
        FFileHelper::SaveStringToFile(Report,*(FPaths::ProjectSavedDir()/TEXT("puzzle-scenario.json"))); UE_LOG(LogTemp,Display,TEXT("MS_PUZZLE_RESULT=%s"),*Report); FPlatformMisc::RequestExitWithStatus(false,Failures?1:0); SetActorTickEnabled(false);
    }
}
