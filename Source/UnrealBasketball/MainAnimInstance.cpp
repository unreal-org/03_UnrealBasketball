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
    
    UE_LOG(LogTemp, Warning, TEXT("%i"), CanMove)
}

void UMainAnimInstance::AnimNotify_SetBasketLocation() { if (ensure(HoopzCharacter)) BasketLocation = HoopzCharacter->BasketLocation; }
void UMainAnimInstance::AnimNotify_ResetPrevMontageKey() { PrevPoseKey = -1; }
void UMainAnimInstance::AnimNotify_SetPivot()
{ 
    if (!ensure(HoopzCharacter)) { return; }
    HoopzCharacter->PivotMode = true;
    HoopzCharacter->PivotPos = 0;
    PivotPos = 0;
    // set movementmode
}

void UMainAnimInstance::OnTimerExpire()
{
    CanMove = true;
    HoopzCharacter->CanTurn = true;
    //Notified = false;
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
        PivotPos = HoopzCharacter->PivotPos;
        if (PivotPos != PrevPivotPos) {
            PrevPivotPos = PivotPos;
            if (EstablishPivotFoot == false) {
                EstablishPivotFoot = true;
                if (PivotPos > 0) { PivotKey = true; }
                else { PivotKey = false; }
            }

            PivotTurn();
        }

        if (CanMove == true) {
            PoseKey = HoopzCharacter->PivotInputKey;
            if (PoseKey != PrevPoseKey) {
                PrevPoseKey = PoseKey;   // Set to -1 when Pivot State Exit
                if (EstablishPivotFoot == false) {
                    EstablishPivotFoot = true;
                    if (PoseKey <= 4) { PivotKey = false; } // Left Foot
                    else { PivotKey = true; } // Right Foot    
                }

                PivotStep();
            }
        }
    } 
    else {
        FTimerHandle Timer;
	    GetWorld()->GetTimerManager().SetTimer(Timer, this, &UMainAnimInstance::OnTimerExpire, PivotDelay, false);
        return; 
    }

    /*  TODO: Root Motion from Anim Montage for Network play
    if (!Montage_IsPlaying(CurrentMontage)) {
        Montage_Play(CurrentMontage, 1);
        Montage_JumpToSection(FName(TEXT("PivotStepLeft0")), CurrentMontage);
        Montage_JumpToSection(FName(TEXT("PivotPosNeutralR6L2")), CurrentMontage);
    }
    */
}

void UMainAnimInstance::PivotTurn()
{
    if (PivotKey == false) {  // left foot
        CanMove = false;
        int TempIndex = abs(PivotPos % 8); // Play Current Pivot Pos
        switch (TempIndex) {
            case 0: // 
                PoseIndex = 0;
                DefaultPivotPos = PoseIndex;
                break;
            case 1: //
                PoseIndex = 4;
                DefaultPivotPos = PoseIndex;
                break;
            case 2: //
                PoseIndex = 3;
                DefaultPivotPos = PoseIndex;
                break;
            case 3: //
                PoseIndex = 9;
                DefaultPivotPos = PoseIndex;
                break;
            case 4: //
                PoseIndex = 8;
                DefaultPivotPos = PoseIndex;
                break;
            case 5: //
                PoseIndex = 7;
                DefaultPivotPos = PoseIndex;
                break;
            case 6: //
                PoseIndex = 6;
                DefaultPivotPos = PoseIndex;
                break;
            case 7: //
                PoseIndex = 5;
                DefaultPivotPos = PoseIndex;
                break;
            default:
                return;
        }
        return;
    }
    else if (PivotKey == true) { // right foot
        CanMove = false;
        int TempIndex = abs(PivotPos % 8); // Play Current Pivot Pos
        switch (TempIndex) {
            case 0: // 
                PoseIndex = 0;
                DefaultPivotPos = PoseIndex;
                break;
            case 1: //
                PoseIndex = 16;
                DefaultPivotPos = PoseIndex;
                break;
            case 2: //
                PoseIndex = 10;
                DefaultPivotPos = PoseIndex;
                break;
            case 3: //
                PoseIndex = 11;
                DefaultPivotPos = PoseIndex;
                break;
            case 4: //
                PoseIndex = 12;
                DefaultPivotPos = PoseIndex;
                break;
            case 5: //
                PoseIndex = 13;
                DefaultPivotPos = PoseIndex;
                break;
            case 6: //
                PoseIndex = 14;
                DefaultPivotPos = PoseIndex;
                break;
            case 7: //
                PoseIndex = 15;
                DefaultPivotPos = PoseIndex;
                break;
            default:
                return;
        }
        return;
    }
}

void UMainAnimInstance::PivotStep()
{
    if (PivotKey == false) { // left foot
        CanMove = false;
        switch (PoseKey) {
            case 0: // 
                PoseIndex = 17;
                break;
            case 1: //
                PoseIndex = 18;
                break;
            case 2: //
                PoseIndex = 19;
                break;
            case 3: //
                PoseIndex = 20;
                break;
            case 4: //
                PoseIndex = 21;
                break;
            default:
                PoseIndex = DefaultPivotPos;
                return;   
        }
    }
    
    else if (PivotKey == true) { // right foot
        CanMove = false;
        switch (PoseKey) {
            case 5: // 
                PoseIndex = 22;
                break;
            case 6: //
                PoseIndex = 23;
                break;
            case 7: //
                PoseIndex = 24;
                break;
            case 8: //
                PoseIndex = 25;
                break;
            case 9: //
                PoseIndex = 26;
                break;
            default:
                PoseIndex = DefaultPivotPos;
                return;
        }
    }
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