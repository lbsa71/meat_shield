#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MSCharacter.generated.h"
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
class UStaticMeshComponent;
class UMSAudio;
struct FInputActionValue;
UCLASS()
class AMSCharacter : public ACharacter
{
    GENERATED_BODY()
public:
    AMSCharacter();
    virtual void BeginPlay() override;
    virtual void SetupPlayerInputComponent(UInputComponent* Component) override;
    virtual void Tick(float Delta) override;
    void Impact();
    void Retry();
    FVector TargetPoint() const { return GetActorLocation()+FVector(0,0,bIsCrouched?5:25); }
    float EyeOffset() const { return bIsCrouched?35.f:65.f; }
    bool Sprinting = false;
    float HitPulse = 0, StepClock = 0;
    UPROPERTY() TObjectPtr<UCameraComponent> Camera;
    UPROPERTY() TObjectPtr<UMSAudio> Audio;
private:
    UPROPERTY() TObjectPtr<UInputMappingContext> Mapping;
    UPROPERTY() TArray<TObjectPtr<UInputAction>> Actions;
    UPROPERTY() TObjectPtr<UStaticMeshComponent> LeftArm;
    UPROPERTY() TObjectPtr<UStaticMeshComponent> RightArm;
    float Stride = 0;
    UInputAction* Axis(FName Name, FKey Positive, FKey Negative);
    UInputAction* Button(FName Name, FKey Key);
    void Forward(const FInputActionValue& Value);
    void Right(const FInputActionValue& Value);
    void Yaw(const FInputActionValue& Value);
    void Pitch(const FInputActionValue& Value);
    void SprintStart();
    void SprintEnd();
    void JumpStart();
    void JumpEnd();
    void CrouchStart();
    void CrouchEnd();
    void Quit();
    void Mute();
};
