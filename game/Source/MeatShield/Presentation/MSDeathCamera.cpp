#include "Presentation/MSDeathCamera.h"
#include "World/MSCorpse.h"
#include "Camera/CameraComponent.h"
#include "Engine/World.h"
void AMSDeathCamera::BeginDeath(FTransform Eye,FVector Forward,AMSCorpse* Body,FVector ReturnEye,AActor* Avatar)
{
    Corpse=Body; Start=Eye.GetLocation(); StartRotation=Eye.GetRotation(); Home=ReturnEye;
    Zoom=Body->GetActorLocation()-Forward*300+FVector(0,0,190);
    FHitResult Hit; FCollisionQueryParams Q(SCENE_QUERY_STAT(DeathCamera),false,Body);
    Q.AddIgnoredActor(Avatar);
    if(GetWorld()->SweepSingleByChannel(Hit,Body->Center()+FVector(0,0,40),Zoom,FQuat::Identity,ECC_Camera,FCollisionShape::MakeSphere(12),Q))
        Zoom=Hit.Location+Hit.ImpactNormal*16;
    SetActorTransform(Eye); GetCameraComponent()->SetFieldOfView(96);
}
void AMSDeathCamera::UpdateDeath(MeatShield::DeathPhase Phase,float Progress)
{
    if(!Corpse) return;
    const float A=FMath::SmoothStep(0.f,1.f,Progress);
    FVector Position=Zoom;
    if(Phase==MeatShield::DeathPhase::PullBack) Position=FMath::Lerp(Start,Zoom,A);
    if(Phase==MeatShield::DeathPhase::Return) Position=FMath::Lerp(Zoom,Home,A)+FVector(0,0,FMath::Sin(A*PI)*180);
    const FQuat Look=(Corpse->Center()+FVector(0,0,15)-Position).Rotation().Quaternion();
    FQuat Rotation=Look;
    if(Phase==MeatShield::DeathPhase::PullBack) Rotation=FQuat::Slerp(StartRotation,Look,A);
    if(Phase==MeatShield::DeathPhase::Return) Rotation=FQuat::Slerp(Look,FQuat::Identity,A);
    SetActorLocationAndRotation(Position,Rotation);
}
