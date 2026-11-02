#include "World/MSMeatField.h"
#include "World/MSCorpse.h"
#include "Engine/World.h"
#include "EngineUtils.h"
void MSMeatField::Build(UWorld* World)
{
    for(TActorIterator<AMSCorpse> It(World);It;++It) if(It->ActorHasTag(TEXT("PreDowned"))) return;
    FRandomStream Random(71031); uint64 Id=uint64(1)<<63;
    for(int X=0;X<9;++X) for(int Y : {-1050,-750,-450,450,750,1050})
    {
        FVector P(-1750+350*X+Random.FRandRange(-25,25),Y+Random.FRandRange(-25,25),18);
        bool Clear=true;
        for(FVector Cover : {FVector(-1000,-500,0),FVector(200,500,0),FVector(900,-500,0)})
            if(FMath::Abs(P.X-Cover.X)<300 && FMath::Abs(P.Y-Cover.Y)<300) Clear=false;
        if(!Clear) continue;
        auto* Body=World->SpawnActor<AMSCorpse>(P,FRotator(90,Random.FRandRange(-180,180),0));
        Body->InitializeDowned(++Id);
    }
}
