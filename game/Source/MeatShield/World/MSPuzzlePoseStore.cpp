#include "World/MSPuzzlePoseStore.h"
#include "Rules/PuzzleRules.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonSerializer.h"
namespace {
FString SourcePath() { return FPaths::ProjectContentDir()/TEXT("MeatShield/Data/puzzle-01.json"); }
bool ParseTransform(const FString& Text,FTransform& Out) { return Out.InitFromString(Text) && Out.IsValid(); }
}
bool MSPuzzlePoseStore::Read(TArray<FMSPuzzlePose>& Poses,FString& Error)
{
    FString Text; TSharedPtr<FJsonObject> Root;
    if(!FFileHelper::LoadFileToString(Text,*SourcePath()) || !FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Text),Root) || !Root) { Error=TEXT("Missing or invalid Content/MeatShield/Data/puzzle-01.json. Run the MSPuzzleBake recipe and stage its validated output."); return false; }
    double Version=0,Seed=0; const TArray<TSharedPtr<FJsonValue>>* Rows=nullptr;
    if(!Root->TryGetNumberField(TEXT("version"),Version) || Version!=MeatShield::Puzzle::Version || !Root->TryGetNumberField(TEXT("seed"),Seed) || Seed!=MeatShield::Puzzle::Seed || !Root->TryGetArrayField(TEXT("bodies"),Rows) || Rows->Num()!=18) { Error=TEXT("Puzzle pose recipe/version/seed/count mismatch; rebake recipe 1."); return false; }
    TArray<FMSPuzzlePose> Loaded;
    for(const auto& V:*Rows) {
        const auto O=V->AsObject(); FMSPuzzlePose P; FString A,M; const TArray<TSharedPtr<FJsonValue>>* Bones=nullptr;
        if(!O || !O->TryGetStringField(TEXT("actor"),A) || !O->TryGetStringField(TEXT("mesh"),M) || !ParseTransform(A,P.Actor) || !ParseTransform(M,P.Mesh) || !O->TryGetArrayField(TEXT("bones"),Bones) || Bones->Num()<24) { Error=TEXT("Invalid puzzle body transform or bone array."); return false; }
        for(const auto& B:*Bones) { const auto O2=B->AsObject(); FString Name,T; FTransform Transform;
            if(!O2 || !O2->TryGetStringField(TEXT("name"),Name) || Name.IsEmpty() || !O2->TryGetStringField(TEXT("transform"),T) || !ParseTransform(T,Transform)) { Error=TEXT("Invalid puzzle bone pose."); return false; }
            P.Pose.BoneNames.Add(FName(*Name)); P.Pose.LocalTransforms.Add(Transform);
        }
        P.Pose.bIsValid=true; Loaded.Add(P);
    }
    Poses=MoveTemp(Loaded); return true;
}
bool MSPuzzlePoseStore::Write(const TArray<FMSPuzzlePose>& Poses,FString& Error)
{
    auto Root=MakeShared<FJsonObject>(); Root->SetNumberField(TEXT("version"),MeatShield::Puzzle::Version); Root->SetNumberField(TEXT("seed"),MeatShield::Puzzle::Seed);
    TArray<TSharedPtr<FJsonValue>> Rows;
    for(const auto& P:Poses) { auto O=MakeShared<FJsonObject>(); O->SetStringField(TEXT("actor"),P.Actor.ToString()); O->SetStringField(TEXT("mesh"),P.Mesh.ToString());
        TArray<TSharedPtr<FJsonValue>> Bones;
        for(int I=0;I<P.Pose.BoneNames.Num();++I) { auto B=MakeShared<FJsonObject>(); B->SetStringField(TEXT("name"),P.Pose.BoneNames[I].ToString()); B->SetStringField(TEXT("transform"),P.Pose.LocalTransforms[I].ToString()); Bones.Add(MakeShared<FJsonValueObject>(B)); }
        O->SetArrayField(TEXT("bones"),Bones); Rows.Add(MakeShared<FJsonValueObject>(O));
    }
    Root->SetArrayField(TEXT("bodies"),Rows); FString Text; FJsonSerializer::Serialize(Root,TJsonWriterFactory<>::Create(&Text));
    const FString Path=FPaths::ProjectSavedDir()/TEXT("puzzle-01.json");
    if(!FFileHelper::SaveStringToFile(Text,*Path)) { Error=TEXT("Could not write Saved/puzzle-01.json"); return false; }
    UE_LOG(LogTemp,Display,TEXT("MS_PUZZLE_BAKED path=%s bodies=%d"),*Path,Poses.Num()); return true;
}
