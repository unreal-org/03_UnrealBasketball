// ChoiBoi Copyrights

#include "MainAnimInstance.h"
#include "Animation/AnimNode_StateMachine.h"
#include "GameFramework/Actor.h"
#include "PlayerCapsuleComponent.h"
#include "Animation/AnimNotifyQueue.h"
#include "TimerManager.h"
#include "HoopzCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SplineComponent.h"

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

    BasketLocation = FVector(419, 0 , 168);
}

void UMainAnimInstance::NativeUpdateAnimation(float DeltaTimeX)
{
    if (!ensure(MainState)) { return; }
   
    // UE_LOG(LogTemp, Warning, TEXT("%i"), MainState->GetCurrentState())

    switch (MainState->GetCurrentState())
    {
        case 0: // Idle
            Idle(DeltaTimeX);
            break;
        case 1: // IdlePivot
            Pivot(DeltaTimeX);
            break;
        case 3: // Dribble
            IdleDribble(DeltaTimeX);
            break;
        case 4: // Jump (Ball)
            WhileJumped(DeltaTimeX);
            break;
        case 6: // IdleOffense
            IdleOffense(DeltaTimeX);
            break;
        default:
            return;
    }

    LeftFootLocation = IKFootTrace(0, DeltaTimeX);
    RightFootLocation = IKFootTrace(1, DeltaTimeX);

    // float Temp = (PlayerSkeletalMesh->GetSocketLocation(FName(TEXT("foot_l"))) + PlayerSkeletalMesh->GetSocketLocation(FName(TEXT("root")))).Size();
    // UE_LOG(LogTemp, Warning, TEXT("%f"), Temp)   // *** 19.44 ***

}

void UMainAnimInstance::AnimNotify_IdleOffense()
{
    HoopzCharacter->CurrentState = 6;
    FirstStep = true;
    FootPlanted = false;
    HoopzCharacter->SetCapsuleHalfHeight(90, 80);
    IKAlpha = 0.9;
}
void UMainAnimInstance::AnimNotify_IdleEntry()
{
    if (!ensure(HoopzCharacter)) { return; }
    HoopzCharacter->CurrentState = 0;
    // To Pivot
    CanMove = false;
    HoopzCharacter->CanTurn = true;
    FootPlanted = false;
    HoopzCharacter->PivotMode = false;
    HoopzCharacter->EstablishPivot = false;
    HoopzCharacter->PivotAttached = false;
    // To Offense (no ball)
    Offense = false;
    HoopzCharacter->SetCapsuleHalfHeight(90, 80);
    IKAlpha = 0.9;
}
void UMainAnimInstance::AnimNotify_SetPivot()   // TODO : lower Capsule height
{ 
    if (!ensure(HoopzCharacter)) { return; }
    HoopzCharacter->CurrentState = 1;
    NewCapsuleLocation = PlayerCapsuleComponent->GetComponentLocation();
    FRotator LookAtCapsuleRotation = UKismetMathLibrary::FindLookAtRotation(PlayerCapsuleComponent->GetComponentLocation(), HoopzCharacter->BasketLocation);
    NewCapsuleRotation.Yaw = LookAtCapsuleRotation.Yaw + HoopzCharacter->TotalRotation.Yaw;
    if (HoopzCharacter->PivotKey == false) { NewOffFootLocation = PlayerSkeletalMesh->GetSocketLocation("ik_foot_l"); } // left foot moves
    else {NewOffFootLocation = PlayerSkeletalMesh->GetSocketLocation("ik_foot_r"); }
    CanMove = false;
    HoopzCharacter->CanTurn = true;
    HoopzCharacter->PivotInputKey = -1;
    FootPlanted = false;
    FirstStep = true;
    HoopzCharacter->PivotMode = true;
    HoopzCharacter->EstablishPivot = false;
    HoopzCharacter->PivotAttached = false;
    
    Dribble = false;
    HoopzCharacter->SetCapsuleHalfHeight(80, 75);
    IKAlpha = 1;
}
void UMainAnimInstance::AnimNotify_IdleJump()
{
    if (!ensure(HoopzCharacter)) { return; }
    HoopzCharacter->CurrentState = 4;
    HoopzCharacter->TotalRotation.Yaw = 0;
    CanMove = false;
    HoopzCharacter->CanTurn = true;
    FootPlanted = false;
    HoopzCharacter->PivotMode = false;
    HoopzCharacter->EstablishPivot = false;
    HoopzCharacter->PivotAttached = false;
    Dribble = false;
    ShotPoseIndex = 0;
    HoopzCharacter->SetCapsuleHalfHeight(90, 80);
    IKAlpha = 0;
}
void UMainAnimInstance::AnimNotify_OnDribble()
{
    if (!ensure(HoopzCharacter)) { return; }
    HoopzCharacter->CurrentState = 3;
    CanMove = false;
    HoopzCharacter->CanTurn = true;
    FootPlanted = false;
    HoopzCharacter->PivotMode = false;
    HoopzCharacter->EstablishPivot = false;
    HoopzCharacter->PivotAttached = false;
    Dribble = true;
    HoopzCharacter->SetCapsuleHalfHeight(90, 80);
    IKAlpha = 0.9;
}

void UMainAnimInstance::IdleOffense(float DeltaTimeX)
{
    if (!ensure(HoopzCharacter)) { return; }

    ThrowX = HoopzCharacter->ThrowY * 65;
    ThrowY = HoopzCharacter->ThrowX * 65;

     // Turn Capsule Towards Basket
    if (HoopzCharacter) {
        FRotator CapsuleRotation = PlayerCapsuleComponent->GetComponentRotation();  
        FRotator TargetCapsuleRotation = CapsuleRotation;
        FRotator LookAtCapsuleRotation = UKismetMathLibrary::FindLookAtRotation(PlayerCapsuleComponent->GetComponentLocation(), HoopzCharacter->BasketLocation);
        TargetCapsuleRotation.Yaw = LookAtCapsuleRotation.Yaw + HoopzCharacter->TotalRotation.Yaw;
        
        CapsuleTurnTime = 0;
        if (CapsuleTurnTime < CapsuleTurnDuration)
        {
            CapsuleTurnTime += DeltaTimeX;
            CapsuleRotation = FMath::Lerp(CapsuleRotation, TargetCapsuleRotation, CapsuleTurnTime / 0.05);
            PlayerCapsuleComponent->SetWorldRotation(CapsuleRotation, false);
        }
    }
}

void UMainAnimInstance::IdleDribble(float DeltaTimeX)
{
    if (!ensure(HoopzCharacter)) { return; }

    ThrowX = HoopzCharacter->ThrowY * 65;
    ThrowY = HoopzCharacter->ThrowX * 65;

     // Turn Capsule Towards Basket
    if (HoopzCharacter) {
        FRotator CapsuleRotation = PlayerCapsuleComponent->GetComponentRotation();  
        FRotator TargetCapsuleRotation = CapsuleRotation;
        FRotator LookAtCapsuleRotation = UKismetMathLibrary::FindLookAtRotation(PlayerCapsuleComponent->GetComponentLocation(), HoopzCharacter->BasketLocation);
        TargetCapsuleRotation.Yaw = LookAtCapsuleRotation.Yaw + HoopzCharacter->TotalRotation.Yaw;
        
        CapsuleTurnTime = 0;
        if (CapsuleTurnTime < CapsuleTurnDuration)
        {
            CapsuleTurnTime += DeltaTimeX;
            CapsuleRotation = FMath::Lerp(CapsuleRotation, TargetCapsuleRotation, CapsuleTurnTime / 0.05);
            PlayerCapsuleComponent->SetWorldRotation(CapsuleRotation, false);
        }
    }
}

// TODO: Have different look modes
void UMainAnimInstance::Idle(float DeltaTimeX)
{
    if (!ensure(HoopzCharacter)) { return; }
    if (!ensure(PlayerCapsuleComponent)) { return; }

    // Turn Capsule Towards Basket
    if (HoopzCharacter) {
        FRotator CapsuleRotation = PlayerCapsuleComponent->GetComponentRotation();
        FRotator TargetCapsuleRotation = CapsuleRotation;
        FRotator LookAtCapsuleRotation = UKismetMathLibrary::FindLookAtRotation(PlayerCapsuleComponent->GetComponentLocation(), HoopzCharacter->BasketLocation);
        TargetCapsuleRotation.Yaw = LookAtCapsuleRotation.Yaw;
        
        CapsuleTurnTime = 0;
        if (CapsuleTurnTime < CapsuleTurnDuration)
        {
            CapsuleTurnTime += DeltaTimeX;
            CapsuleRotation = FMath::Lerp(CapsuleRotation, TargetCapsuleRotation, CapsuleTurnTime / 0.05);
            PlayerCapsuleComponent->SetWorldRotation(CapsuleRotation, false);
        }
    }
}

// TODO: On landing play landed animation
// TODO: Have different jumps for different modes & different look modes (basket, ball, free)
void UMainAnimInstance::WhileJumped(float DeltaTimeX)
{
    if (!ensure(HoopzCharacter)) { return; }
    if (!ensure(PlayerCapsuleComponent)) { return; }

    if (HoopzCharacter->CanChangeShot) {   // TODO: When can I stop checking ensure?
        ShotPoseIndex = HoopzCharacter->ShotKey;
        if (ShotPoseIndex != 0) {
            HoopzCharacter->CanChangeShot = false;
            HasBall = false;  // after shot
        }
    }
    
    // Turn Capsule Towards Basket - TODO : Consider only allowing partial or slowed automatic turn towards basket (adjust remaining rotation through directional input)
    if (HoopzCharacter) {
        FRotator CapsuleRotation = PlayerCapsuleComponent->GetComponentRotation();
        FRotator TargetCapsuleRotation = CapsuleRotation;
        FRotator LookAtCapsuleRotation = UKismetMathLibrary::FindLookAtRotation(PlayerCapsuleComponent->GetComponentLocation(), HoopzCharacter->BasketLocation);
        TargetCapsuleRotation.Yaw = LookAtCapsuleRotation.Yaw;
        
        CapsuleTurnTime = 0;
        if (CapsuleTurnTime < CapsuleTurnDuration)
        {
            CapsuleTurnTime += DeltaTimeX;
            CapsuleRotation = FMath::Lerp(CapsuleRotation, TargetCapsuleRotation, CapsuleTurnTime / CapsuleTurnDuration);
            PlayerCapsuleComponent->SetWorldRotation(CapsuleRotation, false);
        }
    }
}

void UMainAnimInstance::OnStepTimerExpire()
{
    CanMove = true;
    HoopzCharacter->CanTurn = true;
}

// TODO : Play idlepivot animation if idle for more than 5 seconds?
// TODO : Check for Collisions on PivotStep & PivotTurn
// TODO : Fix Multiple pivot input
void UMainAnimInstance::Pivot(float DeltaTimeX)
{
    if (!ensure(HoopzCharacter)) { return; }
    if (!ensure(PlayerCapsuleComponent)) { return; }

    // Blend Pose by Int
    if (CanMove == true) {
        // Pivot Turn
        if (HoopzCharacter->PivotTurn == true) {
            HoopzCharacter->PivotTurn = false;
            if (HoopzCharacter->PivotAttached == true) {
                // Achor Planted Foot Location
                if (FootPlanted == false) 
                {
                    if (HoopzCharacter->PivotKey == false) { // Plant Right Foot
                        PivotRightFootAnchor = PlayerSkeletalMesh->GetSocketLocation("foot_r");   // TODO : Toggle Foot IK
                        FirstStepLeftFootLocation = PlayerSkeletalMesh->GetSocketLocation("foot_l");
                    } else {  // Plant Left Foot
                        PivotLeftFootAnchor = PlayerSkeletalMesh->GetSocketLocation("foot_l");
                        FirstStepRightFootLocation = PlayerSkeletalMesh->GetSocketLocation("foot_r");
                    }
                    FootPlanted = true;
                }

                CanMove = false;
                HoopzCharacter->CanTurn = false;

                // Set New Anchors - Capsule Rotation & OffFoot
                if (HoopzCharacter->PivotKey == false) {   // Right Foot Planted
                    if (HoopzCharacter->PivotTurnRight == true) {        
                        CapsuleAnchorRotation = HoopzCharacter->PlayerRotation;
                        HoopzCharacter->PivotTurnRight = false;
                    }
                    if (HoopzCharacter->PivotTurnLeft == true) {
                        CapsuleAnchorRotation = HoopzCharacter->PlayerRotation;
                        HoopzCharacter->PivotTurnLeft = false;
                    }
                }
                else  // Left Foot Planted
                {
                    if (HoopzCharacter->PivotTurnRight == true) {;
                        CapsuleAnchorRotation = HoopzCharacter->PlayerRotation;
                        HoopzCharacter->PivotTurnRight = false;
                    }
                    if (HoopzCharacter->PivotTurnLeft == true) {
                        CapsuleAnchorRotation = HoopzCharacter->PlayerRotation;
                        HoopzCharacter->PivotTurnLeft = false;
                    }
                }

                // Turn - Set NewCapsuleRotation & Call Pivot Step w/ PrevPoseKey
                // CanMove = false;
                NewCapsuleRotation = CapsuleAnchorRotation;
                PoseKey = PrevPoseKey;
                PivotStep();
            }
            
        }

        // Pivot Step
        if (CanMove == true) {
            if (HoopzCharacter->EstablishPivot == true) {
                // Achor Planted Foot Location & Initial Capsule Location
                if (FootPlanted == false) {
                    if (HoopzCharacter->PivotKey == false) { // Plant Right Foot
                        PivotRightFootAnchor = PlayerSkeletalMesh->GetSocketLocation("foot_r");   // TODO : Toggle Foot IK
                        FirstStepLeftFootLocation = PlayerSkeletalMesh->GetSocketLocation("foot_l");
                    } else {  // Plant Left Foot
                        PivotLeftFootAnchor = PlayerSkeletalMesh->GetSocketLocation("foot_l");
                        FirstStepRightFootLocation = PlayerSkeletalMesh->GetSocketLocation("foot_r");
                    }
                    FootPlanted = true;
                }

                PoseKey = HoopzCharacter->PivotInputKey;
                if (PoseKey != PrevPoseKey) { PivotStep(); }
            }
        }
    } 
    else {
        FTimerHandle StepTimer;
	    GetWorld()->GetTimerManager().SetTimer(StepTimer, this, &UMainAnimInstance::OnStepTimerExpire, StepDelay, false);

        PivotInterp(DeltaTimeX);
        return; 
    }

    /*  TODO: Root Motion from Anim Montage for Network play?
    if (!Montage_IsPlaying(CurrentMontage)) {
        Montage_Play(CurrentMontage, 1);
        Montage_JumpToSection(FName(TEXT("PivotStepLeft0")), CurrentMontage);
        Montage_JumpToSection(FName(TEXT("PivotPosNeutralR6L2")), CurrentMontage);
    }
    */
}

void UMainAnimInstance::PivotInterp(float DeltaTimeX)
{
    FVector CapsuleLocation = PlayerCapsuleComponent->GetComponentLocation();
    FRotator CapsuleRotation = PlayerCapsuleComponent->GetComponentRotation();
    FVector FootLocation;
    FName Foot;
    
    if (HoopzCharacter->PivotKey == false) { // left foot moves
        if (FootPlanted && FirstStep) {
            FootLocation = FirstStepLeftFootLocation;
            FirstStep = false;
        } else {
            FootLocation = PlayerSkeletalMesh->GetSocketLocation("foot_l");  // TODO : Change to Previous FootLocation
        }
    } else {
        if (FootPlanted && FirstStep) {
            FootLocation = FirstStepRightFootLocation;
            FirstStep = false;
        } else {
            FootLocation = PlayerSkeletalMesh->GetSocketLocation("foot_r");
        }
    }

    NewCapsuleLocation.Z = PlayerCapsuleComponent->GetScaledCapsuleHalfHeight();

    if (PivotInterpTime < PivotInterpDuration)
    {
        PivotInterpTime += DeltaTimeX;
        CapsuleLocation = FMath::VInterpTo(CapsuleLocation, NewCapsuleLocation, DeltaTimeX, 7);
        CapsuleRotation = FMath::Lerp(CapsuleRotation, NewCapsuleRotation, PivotInterpTime / 0.15);   // speed up lerp
        PlayerCapsuleComponent->SetWorldLocation(CapsuleLocation, false);
        PlayerCapsuleComponent->SetWorldRotation(CapsuleRotation, false);

        if (HoopzCharacter->PivotKey == false) { // left foot moves
            FootLocation = FMath::VInterpTo(FootLocation, NewOffFootLocation, DeltaTimeX, 7);
            NewLeftFootLocation.X = FootLocation.X;
            NewLeftFootLocation.Y = FootLocation.Y;
        } else {
            FootLocation = FMath::VInterpTo(FootLocation, NewOffFootLocation, DeltaTimeX, 7);
            NewRightFootLocation.X = FootLocation.X;
            NewRightFootLocation.Y = FootLocation.Y;
        }
    }
        
    PivotInterpTime = 0;
}

void UMainAnimInstance::PivotStep()
{
    CanMove = false;
    //HoopzCharacter->CanTurn = false;
    PrevPoseKey = PoseKey;
    if (!ensure(PlayerCapsuleComponent)) { return; }
    if (!ensure(HoopzCharacter)) { return; }
    //if (!ensure(CapsulePivotPoints)) { return; }
    //if (!ensure(FootPivotPoints)) { return; }

    NewCapsuleRotation = HoopzCharacter->PlayerRotation;
    if (HoopzCharacter->PivotKey == false) { // left foot moves
        switch (PoseKey) {
            case 0: // Left - Forward
                // Move Capsule and Off Foot to New Location
                NewCapsuleLocation = HoopzCharacter->CapsulePivotPoints->GetWorldLocationAtSplinePoint(1);
                NewOffFootLocation = HoopzCharacter->FootPivotPoints->GetWorldLocationAtSplinePoint(1);
                break;
            case 1: // Left - Forward Left
                NewCapsuleLocation = HoopzCharacter->CapsulePivotPoints->GetWorldLocationAtSplinePoint(2);
                NewOffFootLocation = HoopzCharacter->FootPivotPoints->GetWorldLocationAtSplinePoint(2);
                break;
            case 2: // Left - Left
                NewCapsuleLocation = HoopzCharacter->CapsulePivotPoints->GetWorldLocationAtSplinePoint(3);
                NewOffFootLocation = HoopzCharacter->FootPivotPoints->GetWorldLocationAtSplinePoint(3);
                break;
            case 3: // Left - Back Left
                NewCapsuleLocation = HoopzCharacter->CapsulePivotPoints->GetWorldLocationAtSplinePoint(4);
                NewOffFootLocation = HoopzCharacter->FootPivotPoints->GetWorldLocationAtSplinePoint(4);
                break;
            case 4: // Left - Back
                NewCapsuleLocation = HoopzCharacter->CapsulePivotPoints->GetWorldLocationAtSplinePoint(5);
                NewOffFootLocation = HoopzCharacter->FootPivotPoints->GetWorldLocationAtSplinePoint(5);
                break;
            default: // Idle Pivot
                NewCapsuleLocation = HoopzCharacter->CapsulePivotPoints->GetWorldLocationAtSplinePoint(0);
                NewOffFootLocation = HoopzCharacter->FootPivotPoints->GetWorldLocationAtSplinePoint(0);
                return;   
        }
    }
    
    else { // right foot moves
        switch (PoseKey) {
            case 5: // Right - Back
                NewCapsuleLocation = HoopzCharacter->CapsulePivotPoints->GetWorldLocationAtSplinePoint(6);
                NewOffFootLocation = HoopzCharacter->FootPivotPoints->GetWorldLocationAtSplinePoint(6);
                break;
            case 6: // Right - Back Right
                NewCapsuleLocation = HoopzCharacter->CapsulePivotPoints->GetWorldLocationAtSplinePoint(7);
                NewOffFootLocation = HoopzCharacter->FootPivotPoints->GetWorldLocationAtSplinePoint(7);
                break;
            case 7: // Right - Right
                NewCapsuleLocation = HoopzCharacter->CapsulePivotPoints->GetWorldLocationAtSplinePoint(8);
                NewOffFootLocation = HoopzCharacter->FootPivotPoints->GetWorldLocationAtSplinePoint(8);
                break;
            case 8: // Right - Forward Right
                NewCapsuleLocation = HoopzCharacter->CapsulePivotPoints->GetWorldLocationAtSplinePoint(9);
                NewOffFootLocation = HoopzCharacter->FootPivotPoints->GetWorldLocationAtSplinePoint(9);
                break;
            case 9: // Right - Forward
                NewCapsuleLocation = HoopzCharacter->CapsulePivotPoints->GetWorldLocationAtSplinePoint(10);
                NewOffFootLocation = HoopzCharacter->FootPivotPoints->GetWorldLocationAtSplinePoint(10);
                break;
            default: // Idle Pivot
                NewCapsuleLocation = HoopzCharacter->CapsulePivotPoints->GetWorldLocationAtSplinePoint(11);
                NewOffFootLocation = HoopzCharacter->FootPivotPoints->GetWorldLocationAtSplinePoint(11);
                return;
        }
    }
}

// TODO : Ragdoll IK
// TODO : Toe IK
FVector UMainAnimInstance::IKFootTrace(int32 Foot, float DeltaTimeX)
{
    if (!ensure(HoopzCharacter)) { return FVector(0, 0, 0); }
    if (!ensure(PlayerSkeletalMesh)) { return FVector(0, 0, 0); }
    if (!ensure(PlayerCapsuleComponent)) { return FVector(0, 0, 0); }

    FName FootName;
    FVector FootSocketLocation;
    if (Foot == 0) {
        FootName = FName(TEXT("foot_l"));
        LeftJointTargetLocation = PlayerSkeletalMesh->GetSocketLocation(FName(TEXT("joint_target_l")));

        // return Pivot Location if Pivot Mode - else foot location
        if (FootPlanted == true)  {
            if (HoopzCharacter->PivotKey == false) {  // Left Foot Moves, Right Plants
                FootSocketLocation = NewLeftFootLocation;
            } else {
                FootSocketLocation = PivotLeftFootAnchor;
            }
        } else {
            FootSocketLocation = PlayerSkeletalMesh->GetSocketLocation(FootName);
        }

        // TODO: Foot Placement IK - Return if foot is inactive
    } else { 
        FootName = FName(TEXT("foot_r"));
        RightJointTargetLocation = PlayerSkeletalMesh->GetSocketLocation(FName(TEXT("joint_target_r")));

        if (FootPlanted == true)  {
            if (HoopzCharacter->PivotKey == true) {  // Right Foot Moves
                FootSocketLocation = NewRightFootLocation;
            } else {
                FootSocketLocation = PivotRightFootAnchor;
            }
        } else {
            FootSocketLocation = PlayerSkeletalMesh->GetSocketLocation(FootName);
        }
    }

    float CapsuleHalfHeight = PlayerCapsuleComponent->GetUnscaledCapsuleHalfHeight();
    
    FVector StartTrace = FVector(FootSocketLocation.X, FootSocketLocation.Y, CapsuleHalfHeight);
    FVector EndTrace = FVector(FootSocketLocation.X, FootSocketLocation.Y, CapsuleHalfHeight - CapsuleHalfHeight - 15); // 15 = trace distance;

    FHitResult HitResult(ForceInit);
    
    bool HitConfirm = GetWorld()->LineTraceSingleByChannel(   // Line trace for feet
            HitResult,
            StartTrace,
            EndTrace,
            ECollisionChannel::ECC_Visibility,
            TraceParameters);

    if (HitConfirm)
    {
        if (!ensure(HitResult.GetActor())) { return FVector(0, 0, 0); }

        // FootOffset Z
        FootSocketLocation.Z = (HitResult.Location - HitResult.TraceEnd).Size() - 15 + 13.47;

        // Use X & Y for Foot Rotation - TODO : Fix
        if (Foot == 0) { 
        LeftFootRotation.Roll = UKismetMathLibrary::DegAtan2(HitResult.Normal.Y, HitResult.Normal.Z);
        LeftFootRotation.Pitch = UKismetMathLibrary::DegAtan2(HitResult.Normal.X, HitResult.Normal.Z);
        } else { 
        RightFootRotation.Roll = UKismetMathLibrary::DegAtan2(HitResult.Normal.Y, HitResult.Normal.Z);
        RightFootRotation.Pitch = UKismetMathLibrary::DegAtan2(HitResult.Normal.X, HitResult.Normal.Z);
        } 

        return FootSocketLocation; 
    }
    
    return FootSocketLocation;  // else - don't offset
}

