#include "World/MSMeatField.h"
#include "World/MSPuzzle.h"
#include "World/MSPuzzlePoseStore.h"
#include "Rules/PuzzleRules.h"
#include "HAL/PlatformMisc.h"
#include "World/MSCorpse.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Misc/CommandLine.h"
void MSMeatField::Build(UWorld* World)
{
    for(TActorIterator<AMSMeatPile> It(World);It;++It) return;
    World->SpawnActor<AMSMeatPile>();
}
AMSMeatPile::AMSMeatPile() { PrimaryActorTick.bCanEverTick=true; }
void AMSMeatPile::BeginPlay()
{
    Super::BeginPlay(); FRandomStream Random(71031);
    if(MSPuzzle::Enabled())
    {
        if(FParse::Param(FCommandLine::Get(),TEXT("MSPuzzleBake"))) {
            int I=0; for(const auto& P:MeatShield::Puzzle::Layout()) {
                auto* B=GetWorld()->SpawnActor<AMSCorpse>(FVector(P.X,P.Y,MeatShield::Puzzle::Height(P.X,P.Y)+180),FRotator(35,P.Yaw,15));
                B->Tags.Add(TEXT("PreDowned")); B->InitializeDowned((uint64(1)<<63)+ ++I); Bodies.Add(B);
            }
        } else {
            TArray<FMSPuzzlePose> Poses; FString Error;
            if(!MSPuzzlePoseStore::Read(Poses,Error)) { UE_LOG(LogTemp,Error,TEXT("REQ-030 %s"),*Error); FPlatformMisc::RequestExitWithStatus(false,2); return; }
            int I=0; for(const auto& P:Poses) {
                auto* B=GetWorld()->SpawnActor<AMSCorpse>();
                if(!B->RestoreInitialPose((uint64(1)<<63)+ ++I,P)) { UE_LOG(LogTemp,Error,TEXT("REQ-030 initial human restore failed")); FPlatformMisc::RequestExitWithStatus(false,2); return; }
                Bodies.Add(B);
            }
            UE_LOG(LogTemp,Display,TEXT("MS_PUZZLE_READY seed=%u bodies=%d"),MeatShield::Puzzle::Seed,Bodies.Num());
        }
        return;
    }
    int Count=42;
    if(FParse::Param(FCommandLine::Get(),TEXT("MSRealismBenchmark"))) { FParse::Value(FCommandLine::Get(),TEXT("MSHumanCount="),Count); Count=FMath::Clamp(Count,1,42); }
    if(FParse::Param(FCommandLine::Get(),TEXT("MSRealismGallery")) || FParse::Param(FCommandLine::Get(),TEXT("MSArmProbe"))) Count=0;
    for(int I=0;I<Count;++I)
    {
        const FVector P(-1050+(I%3-1)*160+Random.FRandRange(-5,5),((I/3)%2-.5f)*160+Random.FRandRange(-5,5),450+(I/6)*220);
        auto* Body=GetWorld()->SpawnActor<AMSCorpse>(P,FRotator(Random.FRandRange(-15,15),Random.FRandRange(-180,180),Random.FRandRange(-15,15)));
        Body->Tags.Add(TEXT("PreDowned")); Bodies.Add(Body);
        Body->InitializeDowned((uint64(1)<<63)+I+1);
    }
}
void AMSMeatPile::Tick(float Delta)
{
    Super::Tick(Delta);
    for(AMSCorpse* Body : Bodies) if(!Body->Stable) return;
    if(MSPuzzle::Enabled() && FParse::Param(FCommandLine::Get(),TEXT("MSPuzzleBake"))) {
        TArray<FMSPuzzlePose> Poses; for(AMSCorpse* B:Bodies) { FMSPuzzlePose P; if(!B->CaptureInitialPose(P)) return; Poses.Add(P); }
        FString Error; const bool OK=MSPuzzlePoseStore::Write(Poses,Error);
        if(!OK) UE_LOG(LogTemp,Error,TEXT("%s"),*Error);
        FPlatformMisc::RequestExitWithStatus(false,OK?0:2);
    }
    SetActorTickEnabled(false);
}
