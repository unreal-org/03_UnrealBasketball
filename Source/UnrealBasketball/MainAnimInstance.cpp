// ChoiBoi Copyrights

#include "MainAnimInstance.h"
#include "Animation/AnimNode_StateMachine.h"
#include "GameFramework/Actor.h"

UMainAnimInstance::UMainAnimInstance(const FObjectInitializer &ObjectInitializer)
    : Super(ObjectInitializer)
{}

void UMainAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();
    
    if (!ensure(GetSkelMeshComponent())) { return; }
    PlayerCapsuleComponent = dynamic_cast<UPlayerCapsuleComponent*>(GetSkelMeshComponent()->GetOwner()->FindComponentByClass<UPlayerCapsuleComponent>());
    MainState = GetStateMachineInstanceFromName(FName(TEXT("MainState")));
}

void UMainAnimInstance::NativeUpdateAnimation(float DeltaTimeX)
{
    switch (CurrentStateIndex)
    {
        // case 0: // Idle - play blend
        //     break;
        case 1: // IdlePivot
            Pivot();
            break;
        default:
            return;
    }
}

void UMainAnimInstance::AnimNotify_ChangeStateInfo()
{
    if (!ensure(MainState)) { return; }
    CurrentStateName = MainState->GetCurrentStateName();
    CurrentStateIndex = MainState->GetCurrentState();
}

// TODO : Play idlepivot animation if idle for more than 5 seconds
// TODO : Blend between pose animations
void UMainAnimInstance::Pivot()
{
    if (!ensure(CurrentMontage)) { return; }
    /*
    if (Anim_Notify signals end of montage) {
        MontageKey = CapsuleComponent->InputKey
    }
    
    if (PivotFoot == 0 && PivotInputKey > 4) {
        // return to neutralpos
    }
    if (PivotFoot == 1 && PivotInputKey <= 4) {
        // return to neutral pos
    }
    
    Montage_JumpToSection(FName SectionName, CurrentMontage);
    */
}

/*
if skelmeshcomponent has ball
pointer to movementcomponent->hasball = true;

if (player has ball) {
    has ball = true;  // trigger trasition state to IdlePivot   ***Also updates HoopzPlayerMovementComponent states***
    pivotfoot established = false;  // true after first step
    pivotaxis = 0; (8  pivot axes per foot)
}
in pivot state
    loop idlepivot animation
    10 motions (5 for each foot) to establish pivot foot
    pivot foot and capsule will be moved to 'predetermined locations' according to directional input

    input commands for turning pivotaxis

*/