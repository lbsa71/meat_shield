#include "Presentation/MSAudio.h"
#include "Engine/World.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundWaveProcedural.h"
#include "GameFramework/Actor.h"
UMSAudio::UMSAudio() { PrimaryComponentTick.bCanEverTick = true; }
void UMSAudio::Play(MSound Kind, FVector Location, bool Spatial)
{
    if (GetWorld()->GetSubsystem<UMSAudioSettings>()->Muted) return;
    constexpr int Rate = 24000;
    const float Duration = Kind == MSound::Shot ? .22f : Kind == MSound::Warning ? .32f : .18f;
    TArray<int16> PCM; PCM.SetNumZeroed(FMath::CeilToInt(Rate * Duration));
    uint32 Noise = 4291;
    for (int I = 0; I < PCM.Num(); ++I)
    {
        const float T = float(I) / Rate;
        Noise = 1664525u * Noise + 1013904223u;
        const float N = (float((Noise >> 8) & 65535) / 32767.5f) - 1;
        float V = 0;
        switch (Kind)
        {
        case MSound::Shot: V = (N * .7f + FMath::Sin(T * 2 * PI * 95) * .3f) * FMath::Exp(-T * 32); break;
        case MSound::Impact: V = (N * .35f + FMath::Sin(T * 2 * PI * (130 - T * 250)) * .65f) * FMath::Exp(-T * 27); break;
        case MSound::Step: V = (N * .3f + FMath::Sin(T * 2 * PI * 65) * .7f) * FMath::Exp(-T * 50); break;
        case MSound::Warning: V = FMath::Sin(T * 2 * PI * 920) * (FMath::Sin(T * 2 * PI * 12) > 0 ? .6f : 0) * FMath::Min(T * 80, 1.f); break;
        default: V = FMath::Sin(T * 2 * PI * (440 + 1600 * T)) * FMath::Sin(PI * T / Duration) * .5f; break;
        }
        const float Gain = Kind == MSound::Step ? .24f : .5f;
        PCM[I] = int16(FMath::Clamp(V * Gain, -.85f, .85f) * 32767);
    }
    if (Voices.Num() < 8)
    {
        auto* Voice = NewObject<UAudioComponent>(GetOwner());
        Voice->bAutoActivate = false;
        Voice->RegisterComponent();
        Voices.Add(Voice); Remaining.Add(0);
    }
    const int Index = Next++ % Voices.Num();
    UAudioComponent* Voice = Voices[Index].Get(); Voice->Stop();
    auto* Wave = NewObject<USoundWaveProcedural>(Voice);
    Wave->SetSampleRate(Rate); Wave->NumChannels = 1; Wave->Duration = Duration; Wave->bLooping = false;
    Wave->QueueAudio(reinterpret_cast<const uint8*>(PCM.GetData()), PCM.Num() * sizeof(int16));
    Voice->bAllowSpatialization = Spatial;
    Voice->bOverrideAttenuation = Spatial;
    Voice->AttenuationOverrides.bAttenuate = Spatial;
    Voice->AttenuationOverrides.AttenuationShapeExtents = FVector(400);
    Voice->AttenuationOverrides.FalloffDistance = 6000;
    Voice->SetWorldLocation(Location); Voice->SetSound(Wave); Voice->Play();
    Remaining[Index] = Duration + .1f; ++Played;
}
void UMSAudio::TickComponent(float Delta, ELevelTick Type, FActorComponentTickFunction* Tick)
{
    Super::TickComponent(Delta, Type, Tick);
    for (int I = 0; I < Voices.Num(); ++I)
        if (Remaining[I] > 0 && (Remaining[I] -= Delta) <= 0) { Voices[I]->Stop(); Voices[I]->SetSound(nullptr); }
}
