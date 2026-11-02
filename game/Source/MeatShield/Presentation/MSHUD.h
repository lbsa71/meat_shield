#pragma once
#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MSHUD.generated.h"
UCLASS()
class AMSHUD : public AHUD
{
    GENERATED_BODY()
public:
    virtual void DrawHUD() override;
};
