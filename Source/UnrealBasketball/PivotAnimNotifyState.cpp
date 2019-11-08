// Cache$ Copyrights


#include "PivotAnimNotifyState.h"
#include "Components/SkeletalMeshComponent.h"

void UPivotAnimNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
    // UE_LOG(LogTemp, Warning, TEXT("NotifyBegin."))
}

void UPivotAnimNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
    // UE_LOG(LogTemp, Warning, TEXT("NotifyTick."))
}

void UPivotAnimNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    // UE_LOG(LogTemp, Warning, TEXT("NotifyEnd."))
}