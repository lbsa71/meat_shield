#pragma once
#include "CoreMinimal.h"
class UStaticMeshComponent;
class USceneComponent;
class AActor;
namespace MSVisuals
{
UStaticMeshComponent* Part(AActor* Owner, USceneComponent* Parent, FName Name, FVector Location, FVector Scale,
    FLinearColor Color, bool Collision = false, const TCHAR* Shape = TEXT("Cube"));
void Color(UStaticMeshComponent* Mesh, FLinearColor Value);
}
