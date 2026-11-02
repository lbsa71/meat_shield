#include "Presentation/MSHumanAppearance.h"
#include "Presentation/MSFaceExpressionAnim.h"
#include "Presentation/MSBodyAnim.h"
#include "Presentation/MSGarmentAnim.h"
#include "Rules/AvatarRules.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMesh.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "GameFramework/Actor.h"
bool UMSHumanAppearance::Initialize(uint64 Life,USkeletalMeshComponent* ExistingBody,bool FirstPerson)
{
    auto* Owner=GetOwner();
    auto Load=[](const TCHAR* Path){return LoadObject<USkeletalMesh>(nullptr,Path);};
    auto* Mesh=Load(TEXT("/Game/MeatShield/Humans/MC_Realism_A/Body/SKM_MC_Realism_A_BodyMesh"));
    auto* Head=Load(TEXT("/Game/MeatShield/Humans/MC_Realism_A/Face/SKM_MC_Realism_A_FaceMesh"));
    auto* Clothes=Load(TEXT("/Game/MeatShield/Wardrobe/SK_FieldUniform"));
    auto* Armor=Load(TEXT("/Game/MeatShield/Wardrobe/SK_FieldVest"));
    if(!Mesh || !Head || !Clothes || !Armor) { UE_LOG(LogTemp,Error,TEXT("REQ-029 missing human wardrobe data. Assemble the MetaHuman and run tools/create_uniform_assets.py in Unreal Editor.")); return false; }
    auto NewMesh=[&](FName Name,USceneComponent* Parent,USkeletalMesh* Asset)
    {
        auto* C=NewObject<USkeletalMeshComponent>(Owner,Name); Owner->AddInstanceComponent(C); C->SetupAttachment(Parent);
        C->SetSkeletalMesh(Asset); C->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        C->VisibilityBasedAnimTickOption=EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones; return C;
    };
    Body=ExistingBody?ExistingBody:NewMesh(TEXT("HumanBody"),Owner->GetRootComponent(),Mesh);
    Body->SetSkeletalMesh(Mesh); Body->SetRelativeLocation(FVector(0,0,-90)); Body->SetRelativeRotation(FRotator(0,-90,0));
    Body->SetForcedLOD(2); Body->VisibilityBasedAnimTickOption=EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
    if(!ExistingBody) Body->RegisterComponent();
    Body->SetAnimInstanceClass(UMSBodyAnim::StaticClass());
    Face=NewMesh(TEXT("HumanFace"),Body,Head); Face->SetAnimInstanceClass(UMSFaceExpressionAnim::StaticClass()); Face->SetForcedLOD(2);
    Face->SetOwnerNoSee(FirstPerson); Face->bCastHiddenShadow=true; Face->RegisterComponent(); Face->AddTickPrerequisiteComponent(Body);
    Uniform=NewMesh(TEXT("HumanUniform"),Body,Clothes);
    Vest=NewMesh(TEXT("HumanVest"),Body,Armor);
    for(auto* Garment:{Uniform.Get(),Vest.Get()})
    {
        Garment->SetAnimInstanceClass(UMSGarmentAnim::StaticClass()); Garment->PrimaryComponentTick.TickGroup=TG_PostPhysics;
        Garment->RegisterComponent(); Garment->AddTickPrerequisiteComponent(Body);
    }
    if(Life==1)
    {
        const auto& A=Mesh->GetRefSkeleton(); const auto& B=Clothes->GetRefSkeleton(); int Wrong=0;
        for(int I=0;I<B.GetNum();++I) { int J=A.FindBoneIndex(B.GetBoneName(I)); if(J<0 || !B.GetRefBonePose()[I].Equals(A.GetRefBonePose()[J],.01f)) ++Wrong; }
        UE_LOG(LogTemp,Display,TEXT("MS_UNIFORM_BIND body_bones=%d uniform_bones=%d mismatches=%d"),A.GetNum(),B.GetNum(),Wrong);
    }
    SetLife(Life,FirstPerson); return true;
}
void UMSHumanAppearance::SetLife(uint64 Life,bool Neutral)
{
    Seed=Life;
    if(auto* Anim=Cast<UMSFaceExpressionAnim>(Face->GetAnimInstance())) Anim->FaceSeed=Neutral?0:Life;
    const auto Style=MeatShield::UniformFor(Life);
    const FLinearColor Colors[3][3]={{{.075f,.10f,.045f},{.025f,.038f,.018f},{.20f,.16f,.075f}},{{.32f,.25f,.15f},{.13f,.105f,.065f},{.46f,.38f,.24f}},{{.12f,.15f,.16f},{.035f,.05f,.065f},{.28f,.31f,.30f}}};
    auto* Mat=Uniform->CreateAndSetMaterialInstanceDynamic(0);
    if(Mat) { Mat->SetVectorParameterValue(TEXT("C0"),Colors[Style.Palette][0]); Mat->SetVectorParameterValue(TEXT("C1"),Colors[Style.Palette][1]); Mat->SetVectorParameterValue(TEXT("C2"),Colors[Style.Palette][2]); Mat->SetScalarParameterValue(TEXT("PatternSeed"),Style.Pattern); }
    Vest->SetVisibility(Style.Vest); Vest->SetHiddenInGame(!Style.Vest);
    UE_LOG(LogTemp,Display,TEXT("MS_UNIFORM_STYLE life=%llu palette=%u pattern=%.0f vest=%d"),Life,Style.Palette,Style.Pattern,Style.Vest);
}
