#pragma once
#include "CoreMinimal.h"
#include "Animation/PoseSnapshot.h"
struct FMSPuzzlePose { FTransform Actor,Mesh; FPoseSnapshot Pose; };
// Versioned filesystem boundary. Never generates fallback puzzle state.
namespace MSPuzzlePoseStore {
    bool Read(TArray<FMSPuzzlePose>& Poses,FString& Error);
    bool Write(const TArray<FMSPuzzlePose>& Poses,FString& Error);
}
