#include "Presentation/MSVisuals.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/Actor.h"
#include "Materials/MaterialInstanceDynamic.h"
UStaticMeshComponent* MSVisuals::Part(AActor* Owner, USceneComponent* Parent, FName Name, FVector Location,
    FVector Scale, FLinearColor Tint, bool Collision, const TCHAR* Shape)
{
    auto* Mesh = NewObject<UStaticMeshComponent>(Owner, Name);
    Owner->AddInstanceComponent(Mesh);
    Mesh->SetupAttachment(Parent);
    Mesh->SetMobility(EComponentMobility::Movable);
    const FString Path = FString::Printf(TEXT("/Engine/BasicShapes/%s.%s"), Shape, Shape);
    Mesh->SetStaticMesh(LoadObject<UStaticMesh>(nullptr, *Path));
    Mesh->SetRelativeLocation(Location);
    Mesh->SetRelativeScale3D(Scale);
    Mesh->SetCollisionProfileName(Collision ? TEXT("BlockAll") : TEXT("NoCollision"));
    Mesh->RegisterComponent();
    auto* Material = LoadObject<UMaterialInterface>(nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
    Mesh->SetMaterial(0, UMaterialInstanceDynamic::Create(Material, Owner));
    Color(Mesh, Tint);
    return Mesh;
}
void MSVisuals::Color(UStaticMeshComponent* Mesh, FLinearColor Value)
{
    if (auto* Material = Cast<UMaterialInstanceDynamic>(Mesh->GetMaterial(0)))
        Material->SetVectorParameterValue(TEXT("Color"), Value);
}
