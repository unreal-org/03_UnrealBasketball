// Cache$ Copyrights


#include "PivotAnimNotifyState.h"
#include "Components/SkeletalMeshComponent.h"

void UPivotAnimNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
    if (!ensure(MeshComp)) { return; }
    
    IK = true;  // AnimScriptInstance = MainAnimInstance;
}

void UPivotAnimNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
    // Do Nothing (for now)
}

void UPivotAnimNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    if (!ensure(MeshComp)) { return; }

    IK = false;
    CanMove = true;
}

void UPivotAnimNotifyState::GetAnimInstance()
{
    
}