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
    PlayerSkeletalMesh = GetSkelMeshComponent();

    if (!ensure(PlayerSkeletalMesh)) { return; }
    PlayerCapsuleComponent = dynamic_cast<UPlayerCapsuleComponent*>(GetSkelMeshComponent()->GetOwner()->FindComponentByClass<UPlayerCapsuleComponent>());

    TraceParameters = FCollisionQueryParams(TraceTag, false);
    TraceParameters.AddIgnoredComponent(Cast<UPrimitiveComponent>(PlayerSkeletalMesh));
    TraceParameters.AddIgnoredActor(Cast<AActor>(PlayerSkeletalMesh->GetOwner()));

     MainState = GetStateMachineInstanceFromName(FName(TEXT("MainState")));
}

void UMainAnimInstance::NativeUpdateAnimation(float DeltaTimeX)
{
    if (!ensure(MainState)) { return; }
    //CurrentStateName = MainState->GetCurrentStateName();
    //CurrentStateIndex = MainState->GetCurrentState();

    // switch (CurrentStateIndex)
    switch (MainState->GetCurrentState())
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

void UMainAnimInstance::AnimNotify_ResetPrevMontageKey()
{
    PrevMontageKey = -1;
}

// TODO : Play idlepivot animation if idle for more than 5 seconds
// TODO : Create Pivot Anim Nofitiy
void UMainAnimInstance::Pivot()
{
    if (!ensure(CurrentMontage)) { return; }
    if (!ensure(PlayerCapsuleComponent)) { return; }
    
    if (!Montage_IsPlaying(CurrentMontage)) {
        Montage_Play(CurrentMontage, 1);
    }
        
    if (CanMove == true) {
        MontageKey = PlayerCapsuleComponent->PivotInputKey;
        PrevMontageKey = MontageKey;   // Set to -1 when Pivot State Exit
        if (MontageKey == PrevMontageKey) { return; }
    } 
    else {
        if (IK == true)
        { 
            if (PivotKey) { LeftFootLocation = IKFootTrace(PivotKey); }
            else { RightFootLocation = IKFootTrace(PivotKey); }
        }
        return;
    }

    if (EstablishPivotFoot == false)
    { 
        if (MontageKey <= 4) { PivotKey = false; } // Left Foot
        else { PivotKey = true; } // Right Foot
        EstablishPivotFoot = true;
    }

    if (PivotKey == false) { // left foot
        switch (MontageKey) {
            case 0: // 
                CanMove = false;
                Montage_JumpToSection(FName(TEXT("PivotStepLeft0")), CurrentMontage);
                break;
            case 1: //
                CanMove = false;
                Montage_JumpToSection(FName(TEXT("PivotStepLeft1")), CurrentMontage);
                break;
            case 2: //
                CanMove = false;
                Montage_JumpToSection(FName(TEXT("PivotStepLeft2")), CurrentMontage);
                break;
            case 3: //
                CanMove = false;
                Montage_JumpToSection(FName(TEXT("PivotStepLeft3")), CurrentMontage);
                break;
            case 4: //
                CanMove = false;
                Montage_JumpToSection(FName(TEXT("PivotStepLeft4")), CurrentMontage);
                break;
            default:
                // CanMove = false;
                // Montage_JumpToSection(FName(TEXT("PivotPosNeutralR6L2")), CurrentMontage); // Play Current Pivot Pos by FName CurrentPivotPos
                return;
        }
    }
    
    if (PivotKey == true) { // right foot
        switch (MontageKey) {
            case 5: // 
                CanMove = false;
                Montage_JumpToSection(FName(TEXT("PivotStepRight5")), CurrentMontage);
                break;
            case 6: //
                CanMove = false;
                Montage_JumpToSection(FName(TEXT("PivotStepRight6")), CurrentMontage);
                break;
            case 7: //
                CanMove = false;
                Montage_JumpToSection(FName(TEXT("PivotStepRight7")), CurrentMontage);
                break;
            case 8: //
                CanMove = false;
                Montage_JumpToSection(FName(TEXT("PivotStepRight8")), CurrentMontage);
                break;
            case 9: //
                CanMove = false;
                Montage_JumpToSection(FName(TEXT("PivotStepRight9")), CurrentMontage);
                break;
            default:
                // CanMove = false;
                // Montage_JumpToSection(FName(TEXT("PivotPosNeutralR6L2")), CurrentMontage); // Play Current Pivot Pos by FName CurrentPivotPos
                return;
        }
    }
}

FVector UMainAnimInstance::IKFootTrace(bool PivotKey)
{
    if (!ensure(PlayerSkeletalMesh)) { return FVector(0, 0, 0); }
    if (!ensure(PlayerCapsuleComponent)) { return FVector(0, 0, 0); }

    FName Foot;
    if (PivotKey == false)
    { 
        Foot = FName(TEXT("foot_l"));
        LeftJointTargetLocation = PlayerSkeletalMesh->GetSocketLocation(FName(TEXT("joint_target_l")));
    }
    else
    { 
        Foot = FName(TEXT("foot_l"));
        RightJointTargetLocation = PlayerSkeletalMesh->GetSocketLocation(FName(TEXT("joint_target_r")));
    } 

    FVector FootSocketLocation = PlayerSkeletalMesh->GetSocketLocation(Foot);
    float CapsuleHalfHeight = PlayerCapsuleComponent->GetUnscaledCapsuleHalfHeight(); // TODO : CapsuleHalfHeight will be variable
    
    FVector StartTrace = FVector(FootSocketLocation.X, FootSocketLocation.Y, CapsuleHalfHeight);
    FVector EndTrace = FVector(FootSocketLocation.X, FootSocketLocation.Y, CapsuleHalfHeight - 95 - 15); // - capsule half height - trace distance;

    FHitResult HitResult(ForceInit);
    
    // if (!ensure(SubAnimInstance)) { return 0; }
    // if (!ensure(SubAnimInstance->GetWorld())) { return 0; }
    // bool HitConfirm = SubAnimInstance->GetWorld()->LineTraceSingleByChannel(
    bool HitConfirm = GetWorld()->LineTraceSingleByChannel(
            HitResult,
            StartTrace,
            EndTrace,
            ECollisionChannel::ECC_Visibility,
            TraceParameters);
    if (HitConfirm)
    {
        if (!ensure(HitResult.GetActor())) { return FVector(0, 0, 0); }
        FootSocketLocation.Z = (HitResult.Location - HitResult.TraceEnd).Size() - 15 + 13.47; // FootOffset
        return FootSocketLocation; 
    }
    
    return FVector(0, 0, 0);
}