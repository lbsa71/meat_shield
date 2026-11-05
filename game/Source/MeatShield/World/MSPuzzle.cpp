#include "World/MSPuzzle.h"
#include "Rules/PuzzleRules.h"
#include "Misc/CommandLine.h"
#include "ProceduralMeshComponent.h"
#include "Materials/MaterialInterface.h"
#include "GameFramework/Actor.h"
bool MSPuzzle::Enabled() { return FParse::Param(FCommandLine::Get(),TEXT("MSPuzzle")); }
void MSPuzzle::BuildTerrain(AActor* Owner)
{
    auto* Surface=NewObject<UProceduralMeshComponent>(Owner,TEXT("PuzzleTerrain")); Owner->AddInstanceComponent(Surface);
    Surface->SetupAttachment(Owner->GetRootComponent()); Surface->bUseComplexAsSimpleCollision=true;
    Surface->bUseAsyncCooking=false; Surface->SetCollisionProfileName(TEXT("BlockAll")); Surface->SetCollisionObjectType(ECC_WorldStatic);
    TArray<FVector> Vertices,Normals; TArray<int32> Triangles; TArray<FVector2D> UV;
    constexpr int NX=168,NY=56;
    for(int Y=0;Y<=NY;++Y) for(int X=0;X<=NX;++X) {
        const double PX=-4200+X*50,PY=-1400+Y*50;
        Vertices.Add(FVector(PX,PY,MeatShield::Puzzle::Height(PX,PY))); UV.Add(FVector2D(PX/100,PY/100));
        Normals.Add(FVector(MeatShield::Puzzle::Height(PX-1,PY)-MeatShield::Puzzle::Height(PX+1,PY),MeatShield::Puzzle::Height(PX,PY-1)-MeatShield::Puzzle::Height(PX,PY+1),2).GetSafeNormal());
        if(X<NX && Y<NY) { const int A=Y*(NX+1)+X,B=A+1,C=A+NX+1,D=C+1; Triangles.Append({A,C,B,B,C,D}); }
    }
    Surface->CreateMeshSection_LinearColor(0,Vertices,Triangles,Normals,UV,TArray<FLinearColor>(),TArray<FProcMeshTangent>(),true);
    Surface->SetMaterial(0,LoadObject<UMaterialInterface>(nullptr,TEXT("/Game/MeatShield/Environment/M_RangeSoil")));
    Surface->RegisterComponent();
    UE_LOG(LogTemp,Display,TEXT("MS_PUZZLE_TERRAIN version=%u seed=%u vertices=%d triangles=%d"),MeatShield::Puzzle::Version,MeatShield::Puzzle::Seed,Vertices.Num(),Triangles.Num()/3);
}
