#include "World/MSYard.h"
#include "World/MSPuzzle.h"
#include "World/MSMeatField.h"
#include "Engine/World.h"
#include "Presentation/MSVisuals.h"
#include "Components/SceneComponent.h"
#include "Components/TextRenderComponent.h"
#include "Engine/DirectionalLight.h"
#include "Engine/SkyLight.h"
#include "Engine/ExponentialHeightFog.h"
#include "Components/SkyAtmosphereComponent.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/SkyLightComponent.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "Presentation/MSRealismEnvironment.h"
#include "Misc/CommandLine.h"
AMSYard::AMSYard() { SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("RecipeRoot"))); }
void AMSYard::BeginPlay()
{
    Super::BeginPlay();
    const bool Puzzle=MSPuzzle::Enabled();
    const FLinearColor Concrete(.24f,.27f,.28f), Dark(.07f,.1f,.12f), Orange(.9f,.32f,.04f);
    int N = 0;
    auto Box = [&](FVector P, FVector S, FLinearColor C, bool Collision = true)
    { return MSVisuals::Part(this, GetRootComponent(), FName(*FString::Printf(TEXT("Yard_%03d"), N++)), P, S, C, Collision); };
    if(Puzzle) MSPuzzle::BuildTerrain(this); else Box({0,0,-30},{84,28,.6f},Concrete);
    MSMeatField::Build(GetWorld());
    Box({0,-1420,250},{84,.4f,5},Dark); Box({0,1420,250},{84,.4f,5},Dark);
    Box({-4200,0,250},{.4f,28,5},Dark); Box({4200,0,250},{.4f,28,5},Dark);
    for (int X=-3800; X<=3800; X+=400)
    {
        Box({float(X),-1380,310},{.7f,.9f,6.2f},Concrete); Box({float(X),1380,310},{.7f,.9f,6.2f},Concrete);
        Box({float(X),-1150,2},{1.4f,.10f,.03f},Orange,false); Box({float(X),1150,2},{1.4f,.10f,.03f},Orange,false);
    }
    if(!Puzzle) for (int X=-2200; X<1200; X+=350) Box({float(X),0,1},{1.2f,.06f,.02f},FLinearColor(.65f,.68f,.62f),false);
    if(!Puzzle) for (const FVector P : {FVector(-1000,-500,100), FVector(200,500,100), FVector(900,-500,100)})
    {
        Box(P,{3.6f,3.5f,2},Concrete);
        Box(P+FVector(0,0,104),{3.8f,3.7f,.12f},Orange,false);
        Box(P+FVector(-181,0,20),{.02f,2.8f,.12f},Dark,false);
    }
    Box({-1900,0,1},{.14f,24,.03f},FLinearColor(.14f,.8f,.55f),false);
    Box({1150,0,1},{.18f,24,.03f},Orange,false);
    Box({2200,0,200},{8,18,4},Dark); Box({2180,0,430},{8.5f,19,.6f},Concrete);
    for (int Y=-800; Y<=800; Y+=200) Box({1770,float(Y),200},{.14f,.2f,3.8f},Orange,false);
    GetWorld()->SpawnActor<ASkyAtmosphere>();
    auto* Sun = GetWorld()->SpawnActor<ADirectionalLight>(FVector(0,0,1500),FRotator(-48,-30,0));
    CastChecked<UDirectionalLightComponent>(Sun->GetLightComponent())->bAtmosphereSunLight=true;
    Sun->GetLightComponent()->SetIntensity(4.f);
    auto* Sky = GetWorld()->SpawnActor<ASkyLight>();
    Sky->GetLightComponent()->SetIntensity(1.1f);
    Sky->GetLightComponent()->SetLightColor(FLinearColor(.65f,.78f,1));
    if(FParse::Param(FCommandLine::Get(),TEXT("MSRealism"))) MSRealismEnvironment::Build(this,Sun,Sky);
    auto* Fog = GetWorld()->SpawnActor<AExponentialHeightFog>();
    Fog->GetComponent()->SetFogDensity(.012f);
    Fog->GetComponent()->SetFogInscatteringColor(FLinearColor(.36f,.46f,.58f));
    auto Sign = [&](FVector P, const TCHAR* Text, float Size, FColor Color)
    {
        auto* T = NewObject<UTextRenderComponent>(this); AddInstanceComponent(T); T->SetupAttachment(GetRootComponent());
        T->SetRelativeLocation(P); T->SetRelativeRotation(FRotator(0,180,0)); T->SetText(FText::FromString(Text));
        T->SetWorldSize(Size); T->SetHorizontalAlignment(EHTA_Center); T->SetTextRenderColor(Color); T->RegisterComponent();
    };
    Sign({1760,0,630},TEXT("MEAT SHIELD  /  RANGE 01"),64,FColor(225,232,222));
    Sign({-1870,0,170},TEXT("LIVE FIRE BEYOND GREEN LINE"),28,FColor(120,230,180));
    Sign({1755,0,520},TEXT("AUTOMATED DEFENSE // SEEK COVER"),25,FColor(255,155,45));
}
