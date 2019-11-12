// ChoiBoi Copyrights

#include "MainAnimInstance.h"
#include "Animation/AnimNode_StateMachine.h"
#include "GameFramework/Actor.h"
#include "PlayerCapsuleComponent.h"
#include "Animation/AnimNotifyQueue.h"
#include "TimerManager.h"
#include "HoopzCharacter.h"

UMainAnimInstance::UMainAnimInstance(const FObjectInitializer &ObjectInitializer)
    : Super(ObjectInitializer)
{}

void UMainAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();
    HoopzCharacter = dynamic_cast<AHoopzCharacter*>(GetSkelMeshComponent()->GetOwner());
    
    if (!ensure(HoopzCharacter)) { return; }
    PlayerSkeletalMesh = HoopzCharacter->GetMesh();
    PlayerCapsuleComponent = HoopzCharacter->FindComponentByClass<UCapsuleComponent>();

    TraceParameters = FCollisionQueryParams(TraceTag, false);
    TraceParameters.AddIgnoredComponent(Cast<UPrimitiveComponent>(PlayerSkeletalMesh));
    TraceParameters.AddIgnoredActor(Cast<AActor>(PlayerSkeletalMesh->GetOwner()));

    MainState = GetStateMachineInstanceFromName(FName(TEXT("MainState")));

    LeftFootLocation = PlayerSkeletalMesh->GetSocketLocation(FName(TEXT("foot_l")));
    RightFootLocation = PlayerSkeletalMesh->GetSocketLocation(FName(TEXT("foot_r")));
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
            Pivot(DeltaTimeX);
            break;
        default:
            return;
    }

    LeftFootLocation = IKFootTrace(0);
    RightFootLocation = IKFootTrace(1);

    if (ensure(HoopzCharacter)) { BasketLocation = HoopzCharacter->BasketLocation;; }
    
    
    // NotifyQueue.AnimNotifies[0].GetNotify()->NotifyStateClass;

    // UE_LOG(LogTemp, Warning, TEXT("%i"), NotifyQueue.AnimNotifies.Num())
    //UE_LOG(LogTemp, Warning, TEXT("%s"), *BasketLocation.ToString())
}

void UMainAnimInstance::AnimNotify_SetBasketLocation() { if (ensure(HoopzCharacter)) BasketLocation = HoopzCharacter->BasketLocation; }
void UMainAnimInstance::AnimNotify_ResetPrevMontageKey() { PrevPoseKey = -1; }
void UMainAnimInstance::AnimNotify_SetPivot()
{ 
    if (!ensure(HoopzCharacter)) { return; }
    HoopzCharacter->PivotMode = true;
    // set movementmode
}

void UMainAnimInstance::OnTimerExpire()
{
    CanMove = true;
    Notified = false;
    //UE_LOG(LogTemp, Warning, TEXT("Can pivot."))
}

// TODO : Play idlepivot animation if idle for more than 5 seconds
// TODO : Create Pivot Anim Nofitiy
void UMainAnimInstance::Pivot(float DeltaTimeX)
{
    //if (!ensure(CurrentMontage)) { return; }
    if (!ensure(PlayerCapsuleComponent)) { return; }
    if (!ensure(HoopzCharacter)) { return; }

    // Blend Pose by Int
    if (CanMove == true) {  // CanMove will be changed by PivotAnimNotifyState broadcast?
        //PoseKey = PlayerCapsuleComponent->PivotInputKey;
        PoseKey = HoopzCharacter->PivotInputKey;
        if (PoseKey == PrevPoseKey) { return; }
        PrevPoseKey = PoseKey;   // Set to -1 when Pivot State Exit
    } 
    else {
        FTimerHandle Timer;
	    GetWorld()->GetTimerManager().SetTimer(Timer, this, &UMainAnimInstance::OnTimerExpire, PivotDelay, false);

        // if (NotifyQueue.AnimNotifies.Num() > 0)
        // { 
        //     Notified = true;
        //     UE_LOG(LogTemp, Warning, TEXT("Notified."))
        //     // if (PivotKey) { LeftFootLocation = IKFootTrace(PivotKey); }
        //     // else { RightFootLocation = IKFootTrace(PivotKey); }
        // } else {
        //     if (Notified == true) {
                
        //     }
        // }
        return; 
    }
    //UE_LOG(LogTemp, Warning, TEXT("Pivot 2."))

    if (EstablishPivotFoot == false)
    { 
        if (PoseKey <= 4) { PivotKey = false; } // Left Foot
        else { PivotKey = true; } // Right Foot
        EstablishPivotFoot = true;
    }
    //UE_LOG(LogTemp, Warning, TEXT("Pivot Foot Established."))

    if (PivotKey == false) { // left foot
        // PoseIndex = 1;
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
                CanMove = false;
                PoseIndex = 0; // Play Current Pivot Pos by FName CurrentPivotPos
                return;
        }
    }
    
    else if (PivotKey == true) { // right foot
        // PoseIndex = 0;
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
                CanMove = false;
                PoseIndex = 0; // Play Current Pivot Pos by FName CurrentPivotPos
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
    */
}

// TODO : Rotate Foot to Match ground
FVector UMainAnimInstance::IKFootTrace(int32 Foot)
{
    if (!ensure(PlayerSkeletalMesh)) { return FVector(0, 0, 0); }
    if (!ensure(PlayerCapsuleComponent)) { return FVector(0, 0, 0); }

    FName FootName;
    if (Foot == 0) { 
        FootName = FName(TEXT("foot_l"));
        LeftJointTargetLocation = PlayerSkeletalMesh->GetSocketLocation(FName(TEXT("joint_target_l")));
    } else { 
        FootName = FName(TEXT("foot_r"));
        RightJointTargetLocation = PlayerSkeletalMesh->GetSocketLocation(FName(TEXT("joint_target_r")));
    } 

    FVector FootSocketLocation = PlayerSkeletalMesh->GetSocketLocation(FootName);
    float CapsuleHalfHeight = PlayerCapsuleComponent->GetUnscaledCapsuleHalfHeight(); // TODO : CapsuleHalfHeight will be variable
    
    FVector StartTrace = FVector(FootSocketLocation.X, FootSocketLocation.Y, CapsuleHalfHeight);
    FVector EndTrace = FVector(FootSocketLocation.X, FootSocketLocation.Y, CapsuleHalfHeight - CapsuleHalfHeight - 15); // - capsule half height - trace distance;

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