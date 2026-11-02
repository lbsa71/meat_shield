#include "World/MSMeatField.h"
#include "World/MSCorpse.h"
#include "Engine/World.h"
#include "EngineUtils.h"
void MSMeatField::Build(UWorld* World)
{
    for(TActorIterator<AMSMeatPile> It(World);It;++It) return;
    World->SpawnActor<AMSMeatPile>();
}
AMSMeatPile::AMSMeatPile() { PrimaryActorTick.bCanEverTick=true; }
void AMSMeatPile::BeginPlay()
{
    Super::BeginPlay(); FRandomStream Random(71031);
    for(int I=0;I<42;++I)
    {
        const FVector P(-1050+(I%3-1)*160+Random.FRandRange(-5,5),((I/3)%2-.5f)*160+Random.FRandRange(-5,5),450+(I/6)*220);
        auto* Body=GetWorld()->SpawnActor<AMSCorpse>(P,FRotator(Random.FRandRange(-15,15),Random.FRandRange(-180,180),Random.FRandRange(-15,15)));
        Body->Tags.Add(TEXT("PreDowned")); Bodies.Add(Body);
        Body->InitializeDowned((uint64(1)<<63)+I+1);
    }
}
void AMSMeatPile::Tick(float Delta)
{
    Super::Tick(Delta);
    for(AMSCorpse* Body : Bodies) if(!Body->Stable) return;
    SetActorTickEnabled(false);
}
