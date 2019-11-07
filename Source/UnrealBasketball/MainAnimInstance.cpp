// ChoiBoi Copyrights

#include "MainAnimInstance.h"
#include "Animation/AnimNode_StateMachine.h"
#include "GameFramework/Actor.h"
#include "PlayerCapsuleComponent.h"
#include "Animation/AnimNotifyQueue.h"

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

    // NotifyQueue.AnimNotifies[0].GetNotify()->NotifyStateClass;

    // UE_LOG(LogTemp, Warning, TEXT("%i"), NotifyQueue.AnimNotifies.Num())
    // UE_LOG(LogTemp, Warning, TEXT("%i"), CurrentStateIndex)
}

void UMainAnimInstance::AnimNotify_ResetPrevMontageKey()
{
    //PrevMontageKey = -1;
    PrevPoseKey = -1;
}

// TODO : Play idlepivot animation if idle for more than 5 seconds
// TODO : Create Pivot Anim Nofitiy
void UMainAnimInstance::Pivot()
{
    if (!ensure(CurrentMontage)) { return; }
    if (!ensure(PlayerCapsuleComponent)) { return; }

    // Blend Pose by Int
    if (CanMove == true) {  // CanMove will be changed by PivotAnimNotifyState broadcast
        PoseKey = PlayerCapsuleComponent->PivotInputKey;
        if (PoseKey == PrevPoseKey) { return; }
        PrevPoseKey = PoseKey;   // Set to -1 when Pivot State Exit
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
        if (PoseKey <= 4) { PivotKey = false; } // Left Foot
        else { PivotKey = true; } // Right Foot
        EstablishPivotFoot = true;
    }

    if (PivotKey == false) { // left foot
        switch (PoseKey) {
            case 0: // 
                CanMove = false;
                PoseIndex = 17;
                break;
            case 1: //
                CanMove = false;
                PoseIndex = 18;
                break;
            case 2: //
                CanMove = false;
                PoseIndex = 19;
                break;
            case 3: //
                CanMove = false;
                PoseIndex = 20;
                break;
            case 4: //
                CanMove = false;
                PoseIndex = 21;
                break;
            default:
                // CanMove = false;
                // Montage_JumpToSection(FName(TEXT("PivotPosNeutralR6L2")), CurrentMontage); // Play Current Pivot Pos by FName CurrentPivotPos
                return;
        }
    }
    
    if (PivotKey == true) { // right foot
        switch (PoseKey) {
            case 5: // 
                CanMove = false;
                PoseIndex = 22;
                break;
            case 6: //
                CanMove = false;
                PoseIndex = 23;
                break;
            case 7: //
                CanMove = false;
                PoseIndex = 24;
                break;
            case 8: //
                CanMove = false;
                PoseIndex = 25;
                break;
            case 9: //
                CanMove = false;
                PoseIndex = 26;
                break;
            default:
                // CanMove = false;
                // PoseIndex =  // Play Current Pivot Pos by FName CurrentPivotPos
                return;
        }
    }

    /*  TODO: Root Motion from Anim Montage for Network play
    if (!Montage_IsPlaying(CurrentMontage)) {
        Montage_Play(CurrentMontage, 1);
        Montage_JumpToSection(FName(TEXT("PivotStepLeft0")), CurrentMontage);
        Montage_JumpToSection(FName(TEXT("PivotPosNeutralR6L2")), CurrentMontage);
    }
        
    if (CanMove == true) {  // CanMove will be taken from PivotAnimNotifyState
        MontageKey = PlayerCapsuleComponent->PivotInputKey;
        if (MontageKey == PrevMontageKey) { return; }
        PrevMontageKey = MontageKey;   // Set to -1 when Pivot State Exit
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
    */
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