#include "Presentation/MSRealismEnvironment.h"
#include "World/MSPuzzle.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/SkyLightComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/DirectionalLight.h"
#include "Engine/SkyLight.h"
#include "Engine/PostProcessVolume.h"
#include "Engine/World.h"
#include "PCGComponent.h"
#include "PCGGraph.h"
#include "GameFramework/Actor.h"
void MSRealismEnvironment::Build(AActor* Yard,ADirectionalLight* Sun,ASkyLight* Sky)
{
    // REQ-028: authored safe lane/collision remain authoritative. This is dressing.
    auto* Terrain=LoadObject<UStaticMesh>(nullptr,TEXT("/Game/MeatShield/Environment/SM_RangeSurface"));
    auto* Graph=LoadObject<UPCGGraph>(nullptr,TEXT("/Game/MeatShield/Environment/PCG_EdgeGravel"));
    if(!Terrain || !Graph) { UE_LOG(LogTemp,Error,TEXT("REQ-028 missing environment bake; run tools/create_realism_assets.py in editor.")); return; }
    if(!MSPuzzle::Enabled()) {
    auto* Surface=NewObject<UStaticMeshComponent>(Yard,TEXT("ProceduralGround")); Yard->AddInstanceComponent(Surface);
    Surface->SetupAttachment(Yard->GetRootComponent()); Surface->SetStaticMesh(Terrain);
    Surface->SetCollisionEnabled(ECollisionEnabled::NoCollision); Surface->RegisterComponent();
    }
    for(int Side:{-1,1})
    {
        auto* Edge=Yard->GetWorld()->SpawnActor<AActor>(FVector(0,Side*1250,0),FRotator::ZeroRotator);
        auto* Root=NewObject<UBoxComponent>(Edge); Root->InitBoxExtent(FVector(3750,65,30)); Root->SetCollisionEnabled(ECollisionEnabled::NoCollision); Edge->AddInstanceComponent(Root); Edge->SetRootComponent(Root); Root->RegisterComponent();
        Root->SetWorldLocation(FVector(0,Side*1250,0));
        auto* PCG=NewObject<UPCGComponent>(Edge); Edge->AddInstanceComponent(PCG);
        PCG->Seed=71031+Side; PCG->GenerationTrigger=EPCGComponentGenerationTrigger::GenerateOnDemand;
        PCG->RegisterComponent(); PCG->SetGraph(Graph); PCG->Generate(true);
    }
    auto* Light=CastChecked<UDirectionalLightComponent>(Sun->GetLightComponent());
    Light->SetIntensity(5.5f); Light->SetLightColor(FLinearColor(1,.89f,.74f));
    Light->LightSourceAngle=2; Light->ContactShadowLength=.08f;
    Light->DynamicShadowDistanceMovableLight=12000;
    Sky->GetLightComponent()->SetMobility(EComponentMobility::Movable);
    Sky->GetLightComponent()->SetRealTimeCaptureEnabled(true);
    Sky->GetLightComponent()->SetIntensity(1.1f);
    auto* Post=Yard->GetWorld()->SpawnActor<APostProcessVolume>(); Post->bUnbound=true;
    Post->Settings.bOverride_AmbientOcclusionIntensity=true; Post->Settings.AmbientOcclusionIntensity=.85f;
    Post->Settings.bOverride_AmbientOcclusionRadius=true; Post->Settings.AmbientOcclusionRadius=45;
    UE_LOG(LogTemp,Display,TEXT("MS_ENVIRONMENT_READY seed=71031 terrain=geometryscript gravel=pcg collision=cosmetic lighting=contact_shadows_ssao"));
}
