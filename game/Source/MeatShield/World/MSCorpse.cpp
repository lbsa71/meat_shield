#include "World/MSCorpse.h"
#include "Presentation/MSVisuals.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Engine/World.h"
AMSCorpse::AMSCorpse()
{
    PrimaryActorTick.bCanEverTick=true; PrimaryActorTick.TickGroup=TG_PostPhysics;
    SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("DeathRecord")));
}
int AMSCorpse::Part(FName Name,FVector Position,uint8 Shape,FVector Size,float Mass,FLinearColor Color,FRotator Rotation)
{
    UPrimitiveComponent* Body=nullptr;
    if(Shape==0) { auto* C=NewObject<UCapsuleComponent>(this,Name); C->InitCapsuleSize(Size.X,Size.Z); Body=C; }
    else if(Shape==1) { auto* B=NewObject<UBoxComponent>(this,Name); B->InitBoxExtent(Size); Body=B; }
    else { auto* S=NewObject<USphereComponent>(this,Name); S->InitSphereRadius(Size.X); Body=S; }
    AddInstanceComponent(Body); Body->SetupAttachment(GetRootComponent()); Body->SetRelativeLocation(Position);
    Body->SetRelativeRotation(Rotation);
    Body->SetMobility(EComponentMobility::Movable); Body->SetCollisionProfileName(TEXT("Ragdoll"));
    Body->SetCollisionResponseToChannel(ECC_Pawn,ECR_Ignore); Body->SetCollisionResponseToChannel(ECC_Visibility,ECR_Block);
    // Active rigs are cosmetic physics, not support authority. Suppress their
    // mutual/self contacts; WorldStatic and committed WorldDynamic still block.
    Body->SetCollisionResponseToChannel(ECC_PhysicsBody,ECR_Ignore);
    Body->SetCollisionResponseToChannel(ECC_Camera,ECR_Ignore); Body->SetPhysMaterialOverride(Material);
    Body->BodyInstance.PositionSolverIterationCount=8; Body->BodyInstance.VelocitySolverIterationCount=2;
    Body->RegisterComponent(); Body->SetSimulatePhysics(true); Body->SetMassOverrideInKg(NAME_None,Mass,true);
    Body->SetLinearDamping(.8f); Body->SetAngularDamping(1.2f);
    const int Index=Bodies.Add(Body); Dimensions.Add(Size); Shapes.Add(Shape);
    auto Visual=[&](FName Suffix,FVector P,FVector Scale,const TCHAR* Mesh)
    { MSVisuals::Part(this,Body,FName(*(Name.ToString()+Suffix.ToString())),P,Scale,Color,false,Mesh); };
    if(Shape==0)
    {
        Visual(TEXT("Tube"),FVector::ZeroVector,FVector(Size.X*2,Size.X*2,(Size.Z-Size.X)*2)/100,TEXT("Cylinder"));
        Visual(TEXT("Top"),FVector(0,0,Size.Z-Size.X),FVector(Size.X*2/100),TEXT("Sphere"));
        Visual(TEXT("Bottom"),FVector(0,0,-Size.Z+Size.X),FVector(Size.X*2/100),TEXT("Sphere"));
    }
    else Visual(TEXT("Skin"),FVector::ZeroVector,Shape==1?Size*2/100:FVector(Size.X*2/100),Shape==1?TEXT("Cube"):TEXT("Sphere"));
    return Index;
}
void AMSCorpse::Joint(int A,int B,FVector Position,float Swing)
{
    auto* C=NewObject<UPhysicsConstraintComponent>(this,FName(*FString::Printf(TEXT("Joint_%02d"),Joints.Num())));
    AddInstanceComponent(C); C->SetupAttachment(GetRootComponent()); C->SetRelativeLocation(Position); C->RegisterComponent();
    C->SetDisableCollision(true); C->SetLinearXLimit(LCM_Locked,0); C->SetLinearYLimit(LCM_Locked,0); C->SetLinearZLimit(LCM_Locked,0);
    C->SetAngularSwing1Limit(ACM_Limited,FMath::Min(Swing,85.f)); C->SetAngularSwing2Limit(ACM_Limited,FMath::Min(Swing,85.f)); C->SetAngularTwistLimit(ACM_Free,0);
    C->SetConstrainedComponents(Bodies[A],NAME_None,Bodies[B],NAME_None); Joints.Add(C);
}
bool AMSCorpse::Initialize(uint64 Life,FVector Velocity,FVector ShotDirection,bool Crouched)
{
    if(LifeId || !Life) return false;
    LifeId=Life;
    Material=NewObject<UPhysicalMaterial>(this); Material->Friction=.8f; Material->Restitution=.03f;
    const FLinearColor Cloth(.14f,.22f,.17f), Pants(.12f,.14f,.12f), Dark(.035f,.045f,.04f);
    const int Pelvis=Part(TEXT("Pelvis"),{0,0,0},1,{15,18,12},16,Pants);
    const int Torso=Part(TEXT("Torso"),{0,0,26},0,{18,18,28},25,Cloth);
    const int Head=Part(TEXT("Head"),{0,0,69},2,{12,12,12},6,FLinearColor(.32f,.37f,.29f));
    Joint(Pelvis,Torso,{0,0,10},60); Joint(Torso,Head,{0,0,56},60);
    for(int Side : {-1,1})
    {
        const FString S=Side<0?TEXT("L"):TEXT("R");
        auto Align=[](FVector V){return FQuat::FindBetweenNormals(FVector::UpVector,V.GetSafeNormal()).Rotator();};
        const int Thigh=Part(FName(*(S+TEXT("Thigh"))),{Crouched?18.f:0.f,float(Side*11),Crouched?-6.5f:-27.f},0,{8.5f,8.5f,23},8,Pants,Crouched?Align({36,0,-3}):FRotator::ZeroRotator);
        // Leave clearance between the folded shin and pelvis in the spawn pose.
        const int Shin=Part(FName(*(S+TEXT("Shin"))),{Crouched?18.f:0.f,float(Side*11),Crouched?-21.f:-64.f},0,{7,7,22},5,Pants,Crouched?Align({-36,0,-14}):FRotator::ZeroRotator);
        const int Foot=Part(FName(*(S+TEXT("Boot"))),{Crouched?-3.f:9.f,float(Side*11),Crouched?-31.f:-86.f},1,{15,7,5},2,Dark);
        Joint(Pelvis,Thigh,{0,float(Side*11),Crouched?-5.f:-10.f},85); Joint(Thigh,Shin,{Crouched?36.f:0.f,float(Side*11),Crouched?-11.f:-48.f},100); Joint(Shin,Foot,{0,float(Side*11),Crouched?-28.f:-83.f},45);
        const int Upper=Part(FName(*(S+TEXT("UpperArm"))),{Crouched?8.f:0.f,float(Side*28),24},0,{6,6,22},3,Cloth,Crouched?Align({18,0,-35}):FRotator::ZeroRotator);
        const int Lower=Part(FName(*(S+TEXT("Forearm"))),{Crouched?32.f:3.f,float(Side*30),Crouched?5.f:-16.f},0,{5.5f,5.5f,22},2,Cloth,Crouched?Align({28,0,-6}):FRotator::ZeroRotator);
        const int Hand=Part(FName(*(S+TEXT("Glove"))),{Crouched?53.f:6.f,float(Side*30),Crouched?2.f:-43.f},2,{6,6,6},1,Dark);
        Joint(Torso,Upper,{0,float(Side*25),43},100); Joint(Upper,Lower,{Crouched?18.f:1.f,float(Side*29),Crouched?8.f:2.f},110); Joint(Lower,Hand,{Crouched?50.f:5.f,float(Side*30),Crouched?3.f:-38.f},70);
    }
    InitialCenter=Center();
    const FVector StartVelocity=Velocity*.45f+ShotDirection.GetSafeNormal()*140;
    const FVector Spin=FVector::CrossProduct(FVector::UpVector,ShotDirection.GetSafeNormal()).GetSafeNormal()*180;
    for(UPrimitiveComponent* Body : Bodies)
    {
        Body->SetPhysicsLinearVelocity(StartVelocity+FVector::CrossProduct(Spin*(PI/180),Body->GetComponentLocation()-GetActorLocation()));
        Body->SetPhysicsAngularVelocityInDegrees(Spin);
    }
    Bodies[Torso]->AddImpulseAtLocation(ShotDirection.GetSafeNormal()*1800,Center()+FVector(0,0,25));
    UE_LOG(LogTemp,Display,TEXT("MS_CORPSE_SPAWN life=%llu parts=%d"),LifeId,Bodies.Num());
    return true;
}
FVector AMSCorpse::Center() const { return Bodies.Num()>1?Bodies[1]->GetComponentLocation():GetActorLocation(); }
TArray<FTransform> AMSCorpse::Pose() const
{
    TArray<FTransform> Result; for(UPrimitiveComponent* Body : Bodies) Result.Add(Body->GetComponentTransform()); return Result;
}
void AMSCorpse::Tick(float Delta)
{
    Super::Tick(Delta); if(!LifeId || Stable) return;
    Age+=Delta; PhysicsMoved|=FVector::Distance(Center(),InitialCenter)>10;
    MeatShield::SettleSample Sample;
    for(UPrimitiveComponent* Body : Bodies)
    {
        Sample.LinearCmS=FMath::Max(Sample.LinearCmS,double(Body->GetPhysicsLinearVelocity().Size()));
        Sample.AngularDegS=FMath::Max(Sample.AngularDegS,double(Body->GetPhysicsAngularVelocityInDegrees().Size()));
    }
    Sample.Collapsed=FMath::Abs(Bodies[1]->GetUpVector().Z)<.35;
    FHitResult Support; FCollisionObjectQueryParams Objects; Objects.AddObjectTypesToQuery(ECC_WorldStatic); Objects.AddObjectTypesToQuery(ECC_WorldDynamic);
    FCollisionQueryParams Query(SCENE_QUERY_STAT(CorpseSupport),false,this);
    const FVector P=Center();
    if(GetWorld()->LineTraceSingleByObjectType(Support,P,P-FVector(0,0,200),Objects,Query))
    {
        const float Gap=P.Z-Support.ImpactPoint.Z-Bodies[1]->Bounds.BoxExtent.Z;
        Sample.Supported=Gap>=-8 && Gap<=18;
    }
    if(Settling.Step(Delta,Sample).Commit) CommitPose();
    else if(Age>=NextSettleLog)
    {
        UE_LOG(LogTemp,Display,TEXT("MS_CORPSE_WAIT life=%llu age=%.1f lin=%.1f angular=%.1f up=%.2f z=%.1f support=%d quiet=%.2f"),LifeId,Age,Sample.LinearCmS,Sample.AngularDegS,FMath::Abs(Bodies[1]->GetUpVector().Z),P.Z,Sample.Supported,Settling.Quiet);
        NextSettleLog=Age+2;
    }
}
void AMSCorpse::CommitPose()
{
    CommittedAt=Age;
    for(int I=0;I<Bodies.Num();++I)
    {
        UPrimitiveComponent* Body=Bodies[I]; const FTransform Pose=Body->GetComponentTransform();
        UPrimitiveComponent* Proxy=nullptr; const FVector Size=Dimensions[I];
        const FName Name(*FString::Printf(TEXT("Committed_%02d"),I));
        if(Shapes[I]==0) { auto* C=NewObject<UCapsuleComponent>(this,Name); C->InitCapsuleSize(Size.X,Size.Z); Proxy=C; }
        else if(Shapes[I]==1) { auto* B=NewObject<UBoxComponent>(this,Name); B->InitBoxExtent(Size); Proxy=B; }
        else { auto* S=NewObject<USphereComponent>(this,Name); S->InitSphereRadius(Size.X); Proxy=S; }
        AddInstanceComponent(Proxy); Proxy->SetupAttachment(GetRootComponent()); Proxy->SetWorldTransform(Pose);
        Proxy->SetMobility(EComponentMobility::Movable); Proxy->SetCollisionProfileName(TEXT("BlockAllDynamic"));
        Proxy->SetCollisionResponseToChannel(ECC_Camera,ECR_Ignore); Proxy->RegisterComponent(); Proxies.Add(Proxy);
        Body->SetSimulatePhysics(false); Body->SetWorldTransform(Pose); Body->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }
    for(UPhysicsConstraintComponent* C : Joints) C->DestroyComponent(); Joints.Empty();
    Stable=true; SetActorTickEnabled(false);
    UE_LOG(LogTemp,Display,TEXT("MS_CORPSE_COMMIT life=%llu proxies=%d moved=%d age=%.2f torso_z=%.1f torso_up=%.2f"),LifeId,Proxies.Num(),PhysicsMoved,Age,Center().Z,FMath::Abs(Bodies[1]->GetUpVector().Z));
}
