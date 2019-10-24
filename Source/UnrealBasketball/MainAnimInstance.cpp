// ChoiBoi Copyrights

#include "MainAnimInstance.h"
#include "Animation/AnimNode_StateMachine.h"

UMainAnimInstance::UMainAnimInstance(const FObjectInitializer &ObjectInitializer)
    : Super(ObjectInitializer)
{}

void UMainAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    //MainState = 
}

void UMainAnimInstance::NativeUpdateAnimation(float DeltaTimeX)
{

}

/*

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