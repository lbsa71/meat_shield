#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MSRealismScenario.generated.h"
UCLASS()
class AMSRealismScenario : public AActor
{
    GENERATED_BODY()
public:
    AMSRealismScenario();
    virtual void BeginPlay() override;
    virtual void Tick(float Delta) override;
    virtual void EndPlay(const EEndPlayReason::Type Reason) override;
private:
    UPROPERTY() TObjectPtr<class UMSHumanRagdoll> Human;
    UPROPERTY() TObjectPtr<class ACameraActor> Camera;
    float Elapsed=0;
    int Stage=0,Samples=0;
    double FrameMs=0,GameMs=0,RenderMs=0,GPUMs=0;
    bool Gallery=false;
    bool ArmProbe=false;
    TArray<FQuat> ArmStart;
    float ArmMotion[4]={0,0,0,0};
    FDelegateHandle CaptureHandle;
    uint64 RequestedSeed=0;
    bool WaitingForCapture=false;
    void CaptureProcessed();
    void Capture(const FString& Name);
};
