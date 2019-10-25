// ChoiBoi Copyrights

#include "MainAnimInstance.h"
#include "Animation/AnimNode_StateMachine.h"

UMainAnimInstance::UMainAnimInstance(const FObjectInitializer &ObjectInitializer)
    : Super(ObjectInitializer)
{}

void UMainAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    
}

void UMainAnimInstance::NativeUpdateAnimation(float DeltaTimeX)
{
    /*
    swtich (CurrentStateIndex)
    {
        case 0: // Idle
            break;
        case 1: // IdlePivot
            Pivot();
            break;
        default:
            return;
    }
    */
}

void UMainAnimInstance::AnimNotify_ChangeToIdlePivot()
{
    CurrentStateName = FName(TEXT("IdlePivot"));
}

void UMainAnimInstance::Pivot()
{
    /*
    if not already in position
        PlayPivotAnimation(PlayerCapsuleComponent->PivotFoot, PlayerCapsuleComponent->PivotPos);
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