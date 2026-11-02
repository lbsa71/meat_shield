#include "Gameplay/MSCharacter.h"
#include "Engine/World.h"
#include "Gameplay/MSGameMode.h"
#include "Presentation/MSAudio.h"
#include "Presentation/MSVisuals.h"
#include "Presentation/MSHumanAppearance.h"
#include "Presentation/MSBodyAnim.h"
#include "Components/SkeletalMeshComponent.h"
#include "Misc/CommandLine.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "InputModifiers.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/KismetSystemLibrary.h"
AMSCharacter::AMSCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
    GetCapsuleComponent()->InitCapsuleSize(34,90);
    // CharacterMovement restores the class-default mesh offset after crouching.
    GetMesh()->SetRelativeLocation(FVector(0,0,-90)); GetMesh()->SetRelativeRotation(FRotator(0,-90,0));
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility,ECR_Block);
    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
    Camera->SetupAttachment(GetCapsuleComponent()); Camera->SetRelativeLocation(FVector(0,0,65));
    Camera->bUsePawnControlRotation = true; Camera->FieldOfView = 96;
    GetCharacterMovement()->MaxWalkSpeed = 450; GetCharacterMovement()->JumpZVelocity = 500;
    GetCharacterMovement()->AirControl = .4f; GetCharacterMovement()->MaxStepHeight = 45;
    GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch=true;
    GetCharacterMovement()->SetCrouchedHalfHeight(50);
    GetCharacterMovement()->MaxWalkSpeedCrouched=220;
    Audio = CreateDefaultSubobject<UMSAudio>(TEXT("FootstepsAndImpact"));
}
UInputAction* AMSCharacter::Axis(FName Name, FKey Positive, FKey Negative)
{
    auto* Action = NewObject<UInputAction>(this, Name); Action->ValueType = EInputActionValueType::Axis1D;
    Mapping->MapKey(Action, Positive);
    if (Negative.IsValid()) Mapping->MapKey(Action, Negative).Modifiers.Add(NewObject<UInputModifierNegate>(this));
    Actions.Add(Action); return Action;
}
UInputAction* AMSCharacter::Button(FName Name, FKey Key)
{
    auto* Action = NewObject<UInputAction>(this, Name); Action->ValueType = EInputActionValueType::Boolean;
    Mapping->MapKey(Action, Key); Actions.Add(Action); return Action;
}
void AMSCharacter::SetupPlayerInputComponent(UInputComponent* Component)
{
    Super::SetupPlayerInputComponent(Component);
    Mapping = NewObject<UInputMappingContext>(this);
    auto* Input = CastChecked<UEnhancedInputComponent>(Component);
    Input->BindAction(Axis(TEXT("MoveForward"),EKeys::W,EKeys::S),ETriggerEvent::Triggered,this,&AMSCharacter::Forward);
    Input->BindAction(Axis(TEXT("MoveRight"),EKeys::D,EKeys::A),ETriggerEvent::Triggered,this,&AMSCharacter::Right);
    Input->BindAction(Axis(TEXT("LookYaw"),EKeys::MouseX,FKey()),ETriggerEvent::Triggered,this,&AMSCharacter::Yaw);
    Input->BindAction(Axis(TEXT("LookPitch"),EKeys::MouseY,FKey()),ETriggerEvent::Triggered,this,&AMSCharacter::Pitch);
    auto* Sprint = Button(TEXT("Sprint"),EKeys::LeftShift);
    Input->BindAction(Sprint,ETriggerEvent::Started,this,&AMSCharacter::SprintStart);
    Input->BindAction(Sprint,ETriggerEvent::Completed,this,&AMSCharacter::SprintEnd);
    auto* JumpAction = Button(TEXT("Jump"),EKeys::SpaceBar);
    Input->BindAction(JumpAction,ETriggerEvent::Started,this,&AMSCharacter::JumpStart);
    Input->BindAction(JumpAction,ETriggerEvent::Completed,this,&AMSCharacter::JumpEnd);
    auto* Duck=Button(TEXT("Crouch"),EKeys::LeftControl); Mapping->MapKey(Duck,EKeys::C);
    Input->BindAction(Duck,ETriggerEvent::Started,this,&AMSCharacter::CrouchStart);
    Input->BindAction(Duck,ETriggerEvent::Completed,this,&AMSCharacter::CrouchEnd);
    Input->BindAction(Button(TEXT("Retry"),EKeys::R),ETriggerEvent::Started,this,&AMSCharacter::Retry);
    Input->BindAction(Button(TEXT("Quit"),EKeys::Escape),ETriggerEvent::Started,this,&AMSCharacter::Quit);
    Input->BindAction(Button(TEXT("Mute"),EKeys::M),ETriggerEvent::Started,this,&AMSCharacter::Mute);
    auto* PC = Cast<APlayerController>(GetController());
    if (PC && PC->GetLocalPlayer())
        if (auto* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
            Subsystem->AddMappingContext(Mapping,0);
}
void AMSCharacter::BeginPlay()
{
    Super::BeginPlay();
    if(FParse::Param(FCommandLine::Get(),TEXT("MSRealism")))
    {
        Human=NewObject<UMSHumanAppearance>(this); AddInstanceComponent(Human); Human->RegisterComponent();
        const auto* Mode=GetWorld()->GetAuthGameMode<AMSGameMode>();
        if(!Human->Initialize(Mode?Mode->Run.Life:1,GetMesh(),true)) Human=nullptr;
        else CacheInitialMeshOffset(GetMesh()->GetRelativeLocation(),GetMesh()->GetRelativeRotation());
    }
    else
    {
    const FLinearColor Sleeve(.12f,.19f,.16f), Glove(.045f,.055f,.05f);
    LeftArm = MSVisuals::Part(this,Camera,TEXT("LeftSleeve"),{30,-27,-32},{.40f,.13f,.14f},Sleeve,false,TEXT("Sphere"));
    RightArm = MSVisuals::Part(this,Camera,TEXT("RightSleeve"),{30,27,-32},{.40f,.13f,.14f},Sleeve,false,TEXT("Sphere"));
    MSVisuals::Part(this,LeftArm,TEXT("LeftGlove"),{55,0,0},{.38f,1.1f,1.1f},Glove,false,TEXT("Sphere"));
    MSVisuals::Part(this,RightArm,TEXT("RightGlove"),{55,0,0},{.38f,1.1f,1.1f},Glove,false,TEXT("Sphere"));
    }
    if (auto* PC=Cast<APlayerController>(GetController()))
    { PC->bShowMouseCursor=false; PC->SetInputMode(FInputModeGameOnly()); PC->PlayerCameraManager->ViewPitchMin=-75; PC->PlayerCameraManager->ViewPitchMax=75; }
}
void AMSCharacter::Tick(float Delta)
{
    Super::Tick(Delta);
    HitPulse=FMath::Max(0.f,HitPulse-Delta*2.8f);
    const float Speed=GetVelocity().Size2D();
    const float Movement=FMath::Clamp(Speed/450,0.f,1.5f);
    Stride += Delta * (Sprinting?13.f:9.f) * FMath::Min(Movement,1.f);
    if(Human) if(auto* Anim=Cast<UMSBodyAnim>(GetMesh()->GetAnimInstance())) { Anim->Phase=Stride; Anim->Movement=Movement; Anim->Crouched=bIsCrouched; }
    const bool Ground=GetCharacterMovement()->IsMovingOnGround();
    const float Bob=Ground ? FMath::Sin(Stride*2)*1.25f*Movement : 0;
    Camera->SetRelativeLocation(FVector(Human?10:0,0,FMath::FInterpTo(Camera->GetRelativeLocation().Z,EyeOffset()+Bob,Delta,12)));
    Camera->SetRelativeRotation(FRotator(HitPulse*FMath::Sin(HitPulse*19)*2,0,HitPulse*2));
    Camera->SetFieldOfView(FMath::FInterpTo(Camera->FieldOfView,Sprinting&&!bIsCrouched&&Speed>40?102.f:96.f,Delta,6));
    if (LeftArm && RightArm)
    {
        const float Swing=FMath::Sin(Stride)*Movement;
        LeftArm->SetRelativeLocation(FVector(25+Swing*5,-27,-32+Swing*5));
        RightArm->SetRelativeLocation(FVector(25-Swing*5,27,-32-Swing*5));
        LeftArm->SetRelativeRotation(FRotator(Swing*12,-8,-8));
        RightArm->SetRelativeRotation(FRotator(-Swing*12,8,8));
    }
    StepClock-=Delta;
    if (Ground && Speed>80 && StepClock<=0) { Audio->Play(MSound::Step,GetActorLocation()); StepClock=bIsCrouched?.6f:Sprinting?.28f:.43f; }
}
void AMSCharacter::Forward(const FInputActionValue& V) { AddMovementInput(GetActorForwardVector(),V.Get<float>()); }
void AMSCharacter::Right(const FInputActionValue& V) { AddMovementInput(GetActorRightVector(),V.Get<float>()); }
void AMSCharacter::Yaw(const FInputActionValue& V) { AddControllerYawInput(V.Get<float>()*.85f); }
void AMSCharacter::Pitch(const FInputActionValue& V) { AddControllerPitchInput(-V.Get<float>()*.85f); }
void AMSCharacter::SprintStart() { Sprinting=true; GetCharacterMovement()->MaxWalkSpeed=800; }
void AMSCharacter::SprintEnd() { Sprinting=false; GetCharacterMovement()->MaxWalkSpeed=450; }
void AMSCharacter::JumpStart() { Jump(); }
void AMSCharacter::JumpEnd() { StopJumping(); }
void AMSCharacter::CrouchStart() { Crouch(); }
void AMSCharacter::CrouchEnd() { UnCrouch(); }
void AMSCharacter::Impact() { HitPulse=1; Audio->Play(MSound::Impact,GetActorLocation()); }
void AMSCharacter::Retry() { if (auto* Mode=GetWorld()->GetAuthGameMode<AMSGameMode>()) Mode->Retry(this); }
void AMSCharacter::Mute() { auto* Settings=GetWorld()->GetSubsystem<UMSAudioSettings>(); Settings->Muted=!Settings->Muted; }
void AMSCharacter::Quit() { UKismetSystemLibrary::QuitGame(this,Cast<APlayerController>(GetController()),EQuitPreference::Quit,false); }
void AMSCharacter::RefreshAvatar(uint64 Life) { if(Human) Human->SetLife(Life,true); }
