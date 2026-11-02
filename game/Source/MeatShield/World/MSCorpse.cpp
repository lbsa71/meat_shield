#include "World/MSCorpse.h"
#include "Presentation/MSVisuals.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
AMSCorpse::AMSCorpse()
{
    PrimaryActorTick.bCanEverTick=true; PrimaryActorTick.TickGroup=TG_PostPhysics;
    SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("DeathRecord")));
}
int AMSCorpse::Part(FName Name,FVector Position,uint8 Shape,FVector Size,float Mass,FLinearColor Color)
{
    UPrimitiveComponent* Body=nullptr;
    if(Shape==0) { auto* C=NewObject<UCapsuleComponent>(this,Name); C->InitCapsuleSize(Size.X,Size.Z); Body=C; }
    else if(Shape==1) { auto* B=NewObject<UBoxComponent>(this,Name); B->InitBoxExtent(Size); Body=B; }
    else { auto* S=NewObject<USphereComponent>(this,Name); S->InitSphereRadius(Size.X); Body=S; }
    AddInstanceComponent(Body); Body->SetupAttachment(GetRootComponent()); Body->SetRelativeLocation(Position);
    Body->SetMobility(EComponentMobility::Movable); Body->SetCollisionProfileName(TEXT("Ragdoll"));
    Body->SetCollisionResponseToChannel(ECC_Pawn,ECR_Ignore); Body->SetCollisionResponseToChannel(ECC_Visibility,ECR_Ignore);
    Body->SetCollisionResponseToChannel(ECC_Camera,ECR_Ignore); Body->SetPhysMaterialOverride(Material);
    Body->BodyInstance.PositionSolverIterationCount=8; Body->BodyInstance.VelocitySolverIterationCount=2;
    Body->RegisterComponent(); Body->SetSimulatePhysics(true); Body->SetMassOverrideInKg(NAME_None,Mass,true);
    Body->SetLinearDamping(1.8f); Body->SetAngularDamping(2.f);
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
    C->SetAngularSwing1Limit(ACM_Limited,Swing); C->SetAngularSwing2Limit(ACM_Limited,Swing); C->SetAngularTwistLimit(ACM_Limited,25);
    C->SetConstrainedComponents(Bodies[A],NAME_None,Bodies[B],NAME_None); Joints.Add(C);
}
bool AMSCorpse::Initialize(uint64 Life,FVector Velocity,FVector ShotDirection)
{
    if(LifeId || !Life) return false;
    LifeId=Life;
    Material=NewObject<UPhysicalMaterial>(this); Material->Friction=.8f; Material->Restitution=.03f;
    const FLinearColor Cloth(.14f,.22f,.17f), Pants(.12f,.14f,.12f), Dark(.035f,.045f,.04f);
    const int Pelvis=Part(TEXT("Pelvis"),{0,0,0},1,{15,18,12},16,Pants);
    const int Torso=Part(TEXT("Torso"),{0,0,26},0,{18,18,28},25,Cloth);
    const int Head=Part(TEXT("Head"),{0,0,69},2,{12,12,12},6,FLinearColor(.32f,.37f,.29f));
    Joint(Pelvis,Torso,{0,0,10},25); Joint(Torso,Head,{0,0,56},35);
    for(int Side : {-1,1})
    {
        const FString S=Side<0?TEXT("L"):TEXT("R");
        const int Thigh=Part(FName(*(S+TEXT("Thigh"))),{0,float(Side*11),-27},0,{8.5f,8.5f,23},8,Pants);
        const int Shin=Part(FName(*(S+TEXT("Shin"))),{0,float(Side*11),-64},0,{7,7,22},5,Pants);
        const int Foot=Part(FName(*(S+TEXT("Boot"))),{9,float(Side*11),-86},1,{15,7,5},2,Dark);
        Joint(Pelvis,Thigh,{0,float(Side*11),-10},45); Joint(Thigh,Shin,{0,float(Side*11),-48},45); Joint(Shin,Foot,{0,float(Side*11),-83},25);
        const int Upper=Part(FName(*(S+TEXT("UpperArm"))),{0,float(Side*28),24},0,{6,6,22},3,Cloth);
        const int Lower=Part(FName(*(S+TEXT("Forearm"))),{3,float(Side*30),-16},0,{5.5f,5.5f,22},2,Cloth);
        const int Hand=Part(FName(*(S+TEXT("Glove"))),{6,float(Side*30),-43},2,{6,6,6},1,Dark);
        Joint(Torso,Upper,{0,float(Side*25),43},70); Joint(Upper,Lower,{1,float(Side*29),2},65); Joint(Lower,Hand,{5,float(Side*30),-38},35);
    }
    InitialCenter=Center();
    const FVector StartVelocity=Velocity*.45f+ShotDirection.GetSafeNormal()*140;
    for(UPrimitiveComponent* Body : Bodies) Body->SetPhysicsLinearVelocity(StartVelocity);
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
    if(Age>=.75f) CommitPose();
}
void AMSCorpse::CommitPose()
{
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
    UE_LOG(LogTemp,Display,TEXT("MS_CORPSE_COMMIT life=%llu proxies=%d moved=%d"),LifeId,Proxies.Num(),PhysicsMoved);
}
