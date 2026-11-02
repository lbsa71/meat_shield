#include "Presentation/MSHUD.h"
#include "Engine/World.h"
#include "Gameplay/MSCharacter.h"
#include "Gameplay/MSGameMode.h"
#include "Gameplay/MSTurret.h"
#include "Presentation/MSAudio.h"
#include "Engine/Canvas.h"
#include "Misc/CommandLine.h"
void AMSHUD::DrawHUD()
{
    Super::DrawHUD();
    if(FParse::Param(FCommandLine::Get(),TEXT("MSRealismGallery"))) return;
    auto* Mode=GetWorld()->GetAuthGameMode<AMSGameMode>();
    auto* Player=Cast<AMSCharacter>(GetOwningPawn());
    if(!Canvas || !Mode || !Player) return;
    const float W=Canvas->SizeX,H=Canvas->SizeY, S=FMath::Clamp(W/1280,0.7f,2.f);
    const FLinearColor White(.9f,.93f,.9f), Amber(1,.55f,.14f), Green(.35f,.95f,.65f);
    DrawRect(FLinearColor(.018f,.028f,.035f,.85f),24*S,24*S,370*S,100*S);
    DrawText(TEXT("MEAT SHIELD"),White,42*S,36*S,nullptr,1.6f*S);
    DrawText(TEXT("RANGE 01  /  LIVE FIRE PROTOTYPE"),Amber,42*S,74*S,nullptr,.85f*S);
    DrawRect(FLinearColor(.018f,.028f,.035f,.85f),24*S,H-116*S,460*S,90*S);
    DrawText(FString::Printf(TEXT("HEALTH  %03d"),Mode->Run.Health),White,42*S,H-100*S,nullptr,1.35f*S);
    DrawRect(FLinearColor(.3f,.12f,.1f),42*S,H-68*S,200*S,8*S);
    DrawRect(Mode->Run.Health>30?Green:Amber,42*S,H-68*S,2.f*Mode->Run.Health*S,8*S);
    DrawText(FString::Printf(TEXT("DEPLOYMENTS  %llu"),static_cast<unsigned long long>(Mode->Run.Deployments)),White,265*S,H-99*S,nullptr,.85f*S);
    DrawText(TEXT("WASD move  CTRL/C crouch  SHIFT run  SPACE jump  R retry  M sound  ESC quit"),White,42*S,H-43*S,nullptr,.65f*S);
    FString Status=TEXT("SAFE ZONE  /  CROSS THE GREEN LINE TO ADVANCE");
    FLinearColor StatusColor=Green;
    if(!AMSGameMode::IsProtected(Player->GetActorLocation()))
    {
        Status=TEXT("APPROACH THE TOWER  /  MEAT AND CONCRETE STOP FIRE"); StatusColor=White;
        if(Mode->Turret)
        {
            switch(Mode->Turret->Rules.Phase)
            {
            case MeatShield::ThreatPhase::Acquire: Status=TEXT("TARGET ACQUIRED"); StatusColor=Amber; break;
            case MeatShield::ThreatPhase::Telegraph: Status=TEXT("INCOMING FIRE  //  MOVE TO COVER"); StatusColor=Amber; break;
            case MeatShield::ThreatPhase::Burst: Status=TEXT("UNDER FIRE"); StatusColor=Amber; break;
            case MeatShield::ThreatPhase::Recover: Status=TEXT("TOWER COOLING  //  MOVE"); StatusColor=Green; break;
            default: break;
            }
        }
    }
    const float BannerWidth=600*S;
    if(Mode->IsDeathActive()) { Status=TEXT("BODY LEFT BEHIND  //  REDEPLOYING"); StatusColor=Amber; }
    DrawRect(FLinearColor(.018f,.028f,.035f,.75f),(W-BannerWidth)/2,140*S,BannerWidth,44*S);
    DrawText(Status,StatusColor,(W-BannerWidth)/2+20*S,154*S,nullptr,.82f*S);
    DrawText(FString::Printf(TEXT("BODIES  %d"),Mode->CorpseCount()),White,W-170*S,40*S,nullptr,.9f*S);
    DrawLine(W/2-6*S,H/2,W/2-2*S,H/2,White); DrawLine(W/2+2*S,H/2,W/2+6*S,H/2,White);
    DrawLine(W/2,H/2-6*S,W/2,H/2-2*S,White); DrawLine(W/2,H/2+2*S,W/2,H/2+6*S,White);
    if(GetWorld()->GetSubsystem<UMSAudioSettings>()->Muted) DrawText(TEXT("SOUND MUTED  [M]"),Amber,W-210*S,H-50*S,nullptr,.9f*S);
    if(Player->HitPulse>0)
    {
        const float A=Player->HitPulse*.3f;
        DrawRect(FLinearColor(.8f,.025f,.01f,A),0,0,W,24*S);
        DrawRect(FLinearColor(.8f,.025f,.01f,A),0,H-24*S,W,24*S);
        DrawRect(FLinearColor(.8f,.025f,.01f,A),0,0,24*S,H);
        DrawRect(FLinearColor(.8f,.025f,.01f,A),W-24*S,0,24*S,H);
        DrawText(TEXT("HIT"),Amber,W/2-12*S,H/2+24*S,nullptr,1.1f*S);
    }
}
