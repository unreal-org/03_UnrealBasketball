// ChoiBoi Copyrights

#include "MainAnimInstance.h"
#include "Animation/AnimNode_StateMachine.h"
#include "GameFramework/Actor.h"
#include "PlayerCapsuleComponent.h"

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
    //UE_LOG(LogTemp, Warning, TEXT("%i"), CurrentStateIndex)
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
    if (!ensure(PlayerCapsuleComponent)) { return; }
    
    // if (AnimNotifyState signals end of montage) {
    //     MontageKey = CapsuleComponent->InputKey
    //     PivotFoot = CapsuleComponent->PivotFoot
    // }
    
    // UE_LOG(LogTemp, Warning, TEXT("Pivot called."))

    if (PlayerCapsuleComponent->EstablishPivotFoot == false) {
        //if (!Montage_IsPlaying(CurrentMontage)) {
            //Montage_Play(CurrentMontage, 1);
            //Montage_JumpToSection(FName(TEXT("PivotPosNeutralR6L2")), CurrentMontage);
        //}
        //UE_LOG(LogTemp, Warning, TEXT("%s"), *Montage_GetCurrentSection(CurrentMontage).ToString())

        // Montage_JumpToSection(FName(TEXT("PivotPosNeutralR6L2")), CurrentMontage);
        // UE_LOG(LogTemp, Warning, TEXT("%s"), *Montage_GetCurrentSection(CurrentMontage).ToString())  

        return;
    }
    else {
        MontageKey = PlayerCapsuleComponent->PivotInputKey;
        PivotKey = PlayerCapsuleComponent->PivotFoot;
        UE_LOG(LogTemp, Warning, TEXT("Pivot Foot Established."))
    }
    // UE_LOG(LogTemp, Warning, TEXT("MontageKey : %i"), MontageKey)
    if (PivotKey == false) { // left foot
        switch (MontageKey) {
            case 0: // 
                Montage_JumpToSection(FName(TEXT("PivotStepLeft0")), CurrentMontage);
                break;
            case 1: //
                Montage_JumpToSection(FName(TEXT("PivotStepLeft1")), CurrentMontage);
                break;
            case 2: //
                Montage_JumpToSection(FName(TEXT("PivotStepLeft2")), CurrentMontage);
                break;
            case 3: //
                Montage_JumpToSection(FName(TEXT("PivotStepLeft3")), CurrentMontage);
                break;
            case 4: //
                Montage_JumpToSection(FName(TEXT("PivotStepLeft4")), CurrentMontage);
                break;
            default:
                Montage_JumpToSection(FName(TEXT("PivotPosNeutralR6L2")), CurrentMontage); // defaultpivotpos
                return;
        }
    }
    
    if (PivotKey == true) { // right foot
        switch (MontageKey) {
            case 5: // 
                Montage_JumpToSection(FName(TEXT("PivotStepRight5")), CurrentMontage);
                break;
            case 6: //
                Montage_JumpToSection(FName(TEXT("PivotStepRight6")), CurrentMontage);
                break;
            case 7: //
                Montage_JumpToSection(FName(TEXT("PivotStepRight7")), CurrentMontage);
                break;
            case 8: //
                Montage_JumpToSection(FName(TEXT("PivotStepRight8")), CurrentMontage);
                break;
            case 9: //
                Montage_JumpToSection(FName(TEXT("PivotStepRight9")), CurrentMontage);
                break;
            default:
                Montage_JumpToSection(FName(TEXT("PivotPosNeutralR6L2")), CurrentMontage); // defaultpivotpos
                return;
        }
    }
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