#include "Gameplay/MSTurret.h"
#include "Engine/World.h"
#include "World/MSCorpse.h"
#include "Gameplay/MSCharacter.h"
#include "Gameplay/MSGameMode.h"
#include "Presentation/MSVisuals.h"
#include "Presentation/MSAudio.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "Kismet/GameplayStatics.h"
FHitResult AMSTurret::ResolveShot(FVector Start,FVector End,AMSCharacter* Target)
{
    FHitResult Hit; FCollisionQueryParams Query(SCENE_QUERY_STAT(TurretShot),false,this);
    GetWorld()->LineTraceSingleByChannel(Hit,Start,End,ECC_Visibility,Query);
    if(auto* Body=Cast<AMSCorpse>(Hit.GetActor())) Body->ApplyBulletImpact(Hit.GetComponent(),Hit.ImpactPoint,End-Start);
    if(Target && Hit.GetActor()==Target && !AMSGameMode::IsProtected(Target->GetActorLocation()))
    { ++Hits; auto* Mode=GetWorld()->GetAuthGameMode<AMSGameMode>(); Mode->ApplyTurretHit(Target,Mode->Run.Life,(End-Start).GetSafeNormal(),Hit.ImpactPoint); }
    return Hit;
}
AMSTurret::AMSTurret()
{
    PrimaryActorTick.bCanEverTick=true;
    SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("TowerRoot")));
    Aim=CreateDefaultSubobject<USceneComponent>(TEXT("AimingHead")); Aim->SetupAttachment(GetRootComponent()); Aim->SetRelativeLocation(FVector(0,0,360));
    Barrel=CreateDefaultSubobject<USceneComponent>(TEXT("BarrelAssembly")); Barrel->SetupAttachment(Aim); Barrel->SetRelativeLocation(FVector(100,0,0));
    Muzzle=CreateDefaultSubobject<USceneComponent>(TEXT("Muzzle")); Muzzle->SetupAttachment(Aim); Muzzle->SetRelativeLocation(FVector(290,0,0));
    Light=CreateDefaultSubobject<UPointLightComponent>(TEXT("MuzzleLight")); Light->SetupAttachment(Muzzle); Light->SetLightColor(FLinearColor(1,.4f,.08f)); Light->SetAttenuationRadius(600); Light->SetIntensity(0);
    Audio=CreateDefaultSubobject<UMSAudio>(TEXT("TurretEffects"));
}
void AMSTurret::BeginPlay()
{
    Super::BeginPlay();
    const FLinearColor Armor(.14f,.2f,.18f), Metal(.045f,.06f,.07f), Orange(.85f,.25f,.015f);
    MSVisuals::Part(this,GetRootComponent(),TEXT("Foundation"),{0,0,28},{4.5f,4.5f,.56f},Metal,true);
    MSVisuals::Part(this,GetRootComponent(),TEXT("Pedestal"),{0,0,170},{1.7f,1.7f,2.8f},Armor,true,TEXT("Cylinder"));
    MSVisuals::Part(this,GetRootComponent(),TEXT("Collar"),{0,0,310},{2.3f,2.3f,.3f},Orange,true,TEXT("Cylinder"));
    MSVisuals::Part(this,Aim,TEXT("ArmoredHead"),{0,0,0},{2.5f,2.1f,1.3f},Armor,true);
    MSVisuals::Part(this,Aim,TEXT("HeadTrim"),{0,0,67},{2.6f,2.2f,.12f},Orange);
    Sensor=MSVisuals::Part(this,Aim,TEXT("Sensor"),{130,0,50},{.25f,.55f,.25f},Orange);
    for(int I=0;I<6;++I)
    {
        const float Angle=I*PI/3;
        auto* B=MSVisuals::Part(this,Barrel,FName(*FString::Printf(TEXT("Barrel%d"),I)),{75,FMath::Cos(Angle)*25,FMath::Sin(Angle)*25},{.14f,.14f,2},Metal,false,TEXT("Cylinder"));
        B->SetRelativeRotation(FRotator(90,0,0));
    }
    auto* Ring=MSVisuals::Part(this,Barrel,TEXT("MuzzleHub"),{160,0,0},{.25f,.25f,.22f},Metal,false,TEXT("Cylinder")); Ring->SetRelativeRotation(FRotator(90,0,0));
    Flash=MSVisuals::Part(this,Muzzle,TEXT("Flash"),{0,0,0},{.7f,.45f,.45f},FLinearColor(1,.7f,.08f),false,TEXT("Sphere")); Flash->SetVisibility(false);
    Tracer=MSVisuals::Part(this,GetRootComponent(),TEXT("Tracer"),{0,0,0},{1,.025f,.025f},FLinearColor(1,.8f,.22f)); Tracer->SetVisibility(false);
}
void AMSTurret::Tick(float Delta)
{
    Super::Tick(Delta);
    auto* Player=Cast<AMSCharacter>(UGameplayStatics::GetPlayerPawn(this,0));
    if(!Player) return;
    const FVector Target=Player->TargetPoint();
    const float Distance=FVector::Distance(Aim->GetComponentLocation(),Target);
    const FRotator Wanted=(Target-Aim->GetComponentLocation()).Rotation();
    Aim->SetWorldRotation(FMath::RInterpTo(Aim->GetComponentRotation(),Wanted,Delta,5));
    FHitResult Hit; FCollisionQueryParams Query(SCENE_QUERY_STAT(TurretSight),false,this);
    GetWorld()->LineTraceSingleByChannel(Hit,Muzzle->GetComponentLocation(),Target,ECC_Visibility,Query);
    HasSight=Hit.GetActor()==Player;
    const auto Out=Rules.Step(Delta,HasSight,Distance,AMSGameMode::IsProtected(Player->GetActorLocation()));
    if(Out.Warn) Audio->Play(MSound::Warning,GetActorLocation(),true);
    if(Out.Fire) Fire(Player);
    Recoil=FMath::FInterpTo(Recoil,0,Delta,18);
    Spin+=Delta*(Rules.Phase==MeatShield::ThreatPhase::Burst?1400:50);
    Barrel->SetRelativeLocation(FVector(100-Recoil,0,0)); Barrel->SetRelativeRotation(FRotator(0,0,Spin));
    FlashTime=FMath::Max(0.f,FlashTime-Delta); Flash->SetVisibility(FlashTime>0); Tracer->SetVisibility(FlashTime>0);
    Light->SetIntensity(FlashTime>0?18000:0);
    MSVisuals::Color(Sensor,Rules.Phase==MeatShield::ThreatPhase::Telegraph?FLinearColor(1,.02f,.005f):FLinearColor(.9f,.3f,.025f));
}
void AMSTurret::Fire(AMSCharacter* Target)
{
    const FVector Start=Muzzle->GetComponentLocation(), End=Target->TargetPoint()+Target->GetActorRightVector()*float((Shots%3-1)*16)+FVector(0,0,float((Shots%2)*12-6));
    const FHitResult Hit=ResolveShot(Start,End,Target);
    const FVector Impact=Hit.bBlockingHit?Hit.ImpactPoint:End;
    Tracer->SetWorldLocation((Start+Impact)*.5f); Tracer->SetWorldRotation((Impact-Start).Rotation());
    Tracer->SetWorldScale3D(FVector(FVector::Distance(Start,Impact)/100,.008f,.008f));
    Recoil=18; FlashTime=.04f; ++Shots;
    Audio->Play(MSound::Shot,Start,true);
}
