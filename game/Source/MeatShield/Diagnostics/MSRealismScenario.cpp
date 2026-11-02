#include "Diagnostics/MSRealismScenario.h"
#include "World/MSHumanRagdoll.h"
#include "World/MSCorpse.h"
#include "Presentation/MSFaceExpressionAnim.h"
#include "Presentation/MSHumanAppearance.h"
#include "Presentation/MSBodyAnim.h"
#include "Gameplay/MSGameMode.h"
#include "Gameplay/MSTurret.h"
#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "UnrealClient.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#include "Misc/CommandLine.h"
#include "HAL/PlatformMemory.h"
#include "HAL/PlatformMisc.h"
#include "RenderTimer.h"
#include "DynamicRHI.h"
#include "PhysicsEngine/ConstraintInstance.h"
#include "PhysicsEngine/PhysicsAsset.h"
#include "PhysicsEngine/SkeletalBodySetup.h"
AMSRealismScenario::AMSRealismScenario() { PrimaryActorTick.bCanEverTick=true; }
void AMSRealismScenario::BeginPlay()
{
    Super::BeginPlay(); Gallery=FParse::Param(FCommandLine::Get(),TEXT("MSRealismGallery"));
    ArmProbe=FParse::Param(FCommandLine::Get(),TEXT("MSArmProbe"));
    if(Gallery) CaptureHandle=FScreenshotRequest::OnScreenshotRequestProcessed().AddUObject(this,&AMSRealismScenario::CaptureProcessed);
    auto* Mode=GetWorld()->GetAuthGameMode<AMSGameMode>(); Mode->Turret->SetActorTickEnabled(false);
    Camera=GetWorld()->SpawnActor<ACameraActor>(); Camera->GetCameraComponent()->SetFieldOfView(Gallery?32:75);
    auto* PC=GetWorld()->GetFirstPlayerController(); PC->SetViewTarget(Camera); PC->SetIgnoreMoveInput(true); PC->SetIgnoreLookInput(true);
    if(Gallery || ArmProbe)
    {
        auto* Model=GetWorld()->SpawnActor<AActor>(FVector(-2100,0,90),FRotator::ZeroRotator);
        auto* Root=NewObject<USceneComponent>(Model); Model->AddInstanceComponent(Root); Model->SetRootComponent(Root); Root->RegisterComponent(); Root->SetWorldLocation(FVector(-2100,0,90));
        Human=NewObject<UMSHumanRagdoll>(Model); Model->AddInstanceComponent(Human); Human->RegisterComponent();
        if(!Human->Initialize(4,FVector::ZeroVector,false)) { FPlatformMisc::RequestExit(false); return; }
        if(!ArmProbe) { Human->Body->SetAllBodiesSimulatePhysics(false); Human->Body->SetSimulatePhysics(false); Human->Body->bNoSkeletonUpdate=true; }
        Human->Face->SetForcedLOD(1);
        if(FParse::Param(FCommandLine::Get(),TEXT("MSWardrobePreview")))
        { Human->Body->bNoSkeletonUpdate=false; CastChecked<UMSBodyAnim>(Human->Body->GetAnimInstance())->UseCapturedPose=false; }
        if(ArmProbe) for(const USkeletalBodySetup* Setup:Human->Body->GetPhysicsAsset()->SkeletalBodySetups)
        {
            const FString N=Setup->BoneName.ToString();
            if(!N.Contains(TEXT("arm_")) && !N.StartsWith(TEXT("hand_"))) if(auto* B=Human->Body->GetBodyInstance(Setup->BoneName)) B->SetInstanceSimulatePhysics(false);
        }
        if(ArmProbe) for(auto* C:Human->Body->Constraints) UE_LOG(LogTemp,Display,TEXT("MS_JOINT %s %s swing1=%d/%.1f swing2=%d/%.1f twist=%d/%.1f"),*C->ConstraintBone1.ToString(),*C->ConstraintBone2.ToString(),int(C->GetAngularSwing1Motion()),C->GetAngularSwing1Limit(),int(C->GetAngularSwing2Motion()),C->GetAngularSwing2Limit(),int(C->GetAngularTwistMotion()),C->GetAngularTwistLimit());
    }
    else { Camera->SetActorLocation(FVector(-1590,-460,290)); Camera->SetActorRotation((FVector(-1050,0,65)-Camera->GetActorLocation()).Rotation()); }
}
void AMSRealismScenario::Capture(const FString& Name)
{
    if(Gallery) { RequestedSeed=Human->Appearance->Seed; WaitingForCapture=true; }
    FScreenshotRequest::RequestScreenshot(FPaths::ProjectSavedDir()/Name,false,false);
}
void AMSRealismScenario::CaptureProcessed()
{
    if(!WaitingForCapture) return;
    const bool Same=Human->Appearance->Seed==RequestedSeed;
    UE_LOG(LogTemp,Display,TEXT("MS_CAPTURE_PROBE %s requested=%llu rendered=%llu"),Same?TEXT("passed"):TEXT("failed"),RequestedSeed,Human->Appearance->Seed);
    WaitingForCapture=false;
    if(!Same) { FPlatformMisc::RequestExitWithStatus(false,1); return; }
    ++Stage; Elapsed=0;
    if(Stage<4) Human->Appearance->SetLife(Stage,false);
}
void AMSRealismScenario::EndPlay(const EEndPlayReason::Type Reason)
{ FScreenshotRequest::OnScreenshotRequestProcessed().Remove(CaptureHandle); Super::EndPlay(Reason); }
void AMSRealismScenario::Tick(float Delta)
{
    Super::Tick(Delta); Elapsed+=FMath::Min(Delta,.1f);
    if(ArmProbe)
    {
        const TCHAR* Bones[]={TEXT("upperarm_l"),TEXT("lowerarm_l"),TEXT("upperarm_r"),TEXT("lowerarm_r")};
        for(int I=0;I<4;++I)
        {
            const FName Bone(Bones[I]), Parent=Human->Body->GetParentBone(Bone);
            const FQuat Local=Human->Body->GetBoneQuaternion(Parent).Inverse()*Human->Body->GetBoneQuaternion(Bone);
            if(ArmStart.Num()<4) ArmStart.Add(Local);
            else ArmMotion[I]=FMath::Max(ArmMotion[I],float(FMath::RadiansToDegrees(ArmStart[I].AngularDistance(Local))));
        }
        if(Elapsed>5)
        {
            bool Hang=true;
            for(const TCHAR* Side:{TEXT("l"),TEXT("r")})
            {
                const FVector Shoulder=Human->Body->GetSocketLocation(FName(*(FString(TEXT("upperarm_"))+Side)));
                const FVector Hand=Human->Body->GetSocketLocation(FName(*(FString(TEXT("hand_"))+Side)));
                Hang &= Shoulder.Z-Hand.Z>45 && FMath::Abs(Hand.Y)<30;
                UE_LOG(LogTemp,Display,TEXT("MS_ARM_HANG %s shoulder=%s hand=%s"),Side,*Shoulder.ToString(),*Hand.ToString());
            }
            const bool Pass=Hang && ArmMotion[0]>15 && ArmMotion[1]>5 && ArmMotion[2]>15 && ArmMotion[3]>5;
            UE_LOG(LogTemp,Display,TEXT("MS_ARM_PROBE %s degrees=%.1f,%.1f,%.1f,%.1f"),Pass?TEXT("passed"):TEXT("failed"),ArmMotion[0],ArmMotion[1],ArmMotion[2],ArmMotion[3]); FPlatformMisc::RequestExit(false);
        }
        return;
    }
    if(Gallery)
    {
        if(!Human) return;
        const FVector Head=Human->Face->GetSocketLocation(TEXT("FACIAL_C_FacialRoot"));
        const bool Wardrobe=FParse::Param(FCommandLine::Get(),TEXT("MSWardrobePreview"));
        Camera->GetCameraComponent()->SetFieldOfView(Wardrobe?45:32);
        Camera->SetActorLocation(Head+(Wardrobe?FVector(400,0,-70):FVector(105,0,5)));
        Camera->SetActorRotation((Head+FVector(0,0,Wardrobe?-70:-1)-Camera->GetActorLocation()).Rotation());
        if(Elapsed>2.5f && Stage<4 && !WaitingForCapture)
        {
            Capture(FString::Printf(TEXT("%s-%d.png"),Wardrobe?TEXT("uniform"):TEXT("realism-face"),Stage));
            UE_LOG(LogTemp,Display,TEXT("MS_FACE_CAPTURE profile=%d jaw=%s brow=%s head=%s"),Stage,*Human->Face->GetBoneQuaternion(TEXT("FACIAL_C_Jaw"),EBoneSpaces::ComponentSpace).ToString(),*Human->Face->GetSocketLocation(TEXT("FACIAL_L_ForeheadIn" )).ToString(),*Head.ToString());
        }
        if(Stage==4 && Elapsed>1) { UE_LOG(LogTemp,Display,TEXT("MS_REALISM_GALLERY_COMPLETE")); FPlatformMisc::RequestExit(false); }
        return;
    }
    if(Elapsed<18) return;
    ++Samples; FrameMs+=Delta*1000; GameMs+=FPlatformTime::ToMilliseconds(GGameThreadTime); RenderMs+=FPlatformTime::ToMilliseconds(GRenderThreadTime); GPUMs+=FPlatformTime::ToMilliseconds(RHIGetGPUFrameCycles());
    if(Samples==1) Capture(TEXT("realism-pile.png"));
    if(Samples>=180)
    {
        int Count=0,Settled=0; for(TActorIterator<AMSCorpse> It(GetWorld());It;++It) { ++Count; Settled+=It->Stable; }
        auto Memory=FPlatformMemory::GetStats();
        const FString Report=FString::Printf(TEXT("{\"bodies\":%d,\"settled\":%d,\"samples\":%d,\"frame_ms\":%.3f,\"game_thread_ms\":%.3f,\"render_thread_ms\":%.3f,\"gpu_ms\":%.3f,\"process_physical_mib\":%.1f}"),Count,Settled,Samples,FrameMs/Samples,GameMs/Samples,RenderMs/Samples,GPUMs/Samples,Memory.UsedPhysical/1048576.);
        FFileHelper::SaveStringToFile(Report,*(FPaths::ProjectSavedDir()/FString::Printf(TEXT("realism-benchmark-%d.json"),Count)));
        UE_LOG(LogTemp,Display,TEXT("MS_REALISM_BENCHMARK %s"),*Report); FPlatformMisc::RequestExit(false);
    }
}
