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

// TODO : Do Scratch work on proxy

// Constructors
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

// Tick
void UMainAnimInstance::NativeUpdateAnimation(float DeltaTimeX)
{
    if (!ensure(MainState)) { return; }
   
    // UE_LOG(LogTemp, Warning, TEXT("%i"), MainState->GetCurrentState())

    switch (MainState->GetCurrentState())
    {
        case 0: // Idle
            Idle(DeltaTimeX);
            CapsuleTargetLerp(DeltaTimeX, .05);
            break;
        case 1: // IdlePivot
            Pivot(DeltaTimeX);
            break;
        case 3: // Dribble
            IdleDribble(DeltaTimeX);
            CapsuleTargetLerp(DeltaTimeX, .05);
            break;
        case 4: // Jump (Ball)
            WhileJumped(DeltaTimeX);
            CapsuleTargetLerp(DeltaTimeX, CapsuleTurnDuration);
            break;
        case 6: // IdleOffense
            IdleOffense(DeltaTimeX);
            CapsuleTargetLerp(DeltaTimeX, .05);
            break;
        default:
            return;
    }

    LeftFootLocation = IKFootTrace(0, DeltaTimeX);   // TODO: Disable IK with Procedural cycles
    RightFootLocation = IKFootTrace(1, DeltaTimeX);
}


/////////////////////////// Transition Events //////////////////////////////
// TODO: Trigger during transition 
void UMainAnimInstance::AnimNotify_IdleEntry()
{
    if (!ensure(HoopzCharacter)) { return; }
    HoopzCharacter->CurrentState = 0;

    // Reset OffFootLocation for IK
    if (HoopzCharacter->PivotKey == false) { NewOffFootLocation = PlayerSkeletalMesh->GetSocketLocation("ik_foot_l"); } // left foot moves
    else { NewOffFootLocation = PlayerSkeletalMesh->GetSocketLocation("ik_foot_r"); }

    // For Transition to Pivot - needed?
    CanMove = false;
    HoopzCharacter->CanTurn = true;
    FootPlanted = false;
    HoopzCharacter->PivotMode = false;
    HoopzCharacter->EstablishPivot = false;
    HoopzCharacter->PivotAttached = false;

    // For Transition to Offense (no ball)
    Offense = false;

    // Reset Stance
    HoopzCharacter->SetCapsuleHalfHeight(85, 75);
    IKAlpha = 0.9;
    LeftIKAlpha = 0.9;
    RightIKAlpha = 0.9;
}

void UMainAnimInstance::AnimNotify_SetPivot()   
{ 
    if (!ensure(HoopzCharacter)) { return; }
    HoopzCharacter->CurrentState = 1;

    // Reset Character Locations & Rotations
    NewCapsuleLocation = PlayerCapsuleComponent->GetComponentLocation();
    FRotator LookAtCapsuleRotation = UKismetMathLibrary::FindLookAtRotation(PlayerCapsuleComponent->GetComponentLocation(), HoopzCharacter->BasketLocation);
    NewCapsuleRotation.Yaw = LookAtCapsuleRotation.Yaw + HoopzCharacter->TotalRotation.Yaw;
    if (HoopzCharacter->PivotKey == false) { NewOffFootLocation = PlayerSkeletalMesh->GetSocketLocation("ik_foot_l"); } // left foot moves
    else {NewOffFootLocation = PlayerSkeletalMesh->GetSocketLocation("ik_foot_r"); }

    // Reset Pivot
    CanMove = false;
    HoopzCharacter->CanTurn = true;
    HoopzCharacter->PivotInputKey = -1;
    FootPlanted = false;
    FirstStep = true;
    HoopzCharacter->PivotMode = true;
    HoopzCharacter->EstablishPivot = false;
    HoopzCharacter->PivotAttached = false;

    // Reset Dribble transition variable
    Dribble = false;

    // Reset Stance
    HoopzCharacter->SetCapsuleHalfHeight(80, 75);
    IKAlpha = 1;
    LeftIKAlpha = 1;
    RightIKAlpha = 1;
}

void UMainAnimInstance::AnimNotify_OnDribble()
{
    if (!ensure(HoopzCharacter)) { return; }
    HoopzCharacter->CurrentState = 3;

    // Reset OffFootLocation for IK
    if (HoopzCharacter->PivotKey == false) { NewOffFootLocation = PlayerSkeletalMesh->GetSocketLocation("ik_foot_l"); } // left foot moves
    else {NewOffFootLocation = PlayerSkeletalMesh->GetSocketLocation("ik_foot_r"); }

    // Reset Pivot
    CanMove = false;
    HoopzCharacter->CanTurn = true;
    FootPlanted = false;
    HoopzCharacter->PivotMode = false;
    HoopzCharacter->EstablishPivot = false;
    HoopzCharacter->PivotAttached = false;

    // Reset Dribble transition variable
    Dribble = true;

    // Reset Stance
    HoopzCharacter->SetCapsuleHalfHeight(90, 80);
    IKAlpha = 0.9;
    LeftIKAlpha = 0.9;
    RightIKAlpha = 0.9;
}

void UMainAnimInstance::AnimNotify_IdleJump()
{
    if (!ensure(HoopzCharacter)) { return; }
    HoopzCharacter->CurrentState = 4;

    // Reset TotalRotation
    HoopzCharacter->TotalRotation.Yaw = 0;
    
    // Reset Pivot
    CanMove = false;
    HoopzCharacter->CanTurn = true;
    FootPlanted = false;
    HoopzCharacter->PivotMode = false;
    HoopzCharacter->EstablishPivot = false;
    HoopzCharacter->PivotAttached = false;

    // Reset Dribble transition variable
    Dribble = false;

    // Reset Shot Pose Key
    ShotPoseIndex = 0;

    // Reset Stance
    HoopzCharacter->SetCapsuleHalfHeight(90, 80);
    IKAlpha = 0;
    LeftIKAlpha = 0;
    RightIKAlpha = 0;
}

void UMainAnimInstance::AnimNotify_IdleOffense()
{
    if (!ensure(HoopzCharacter)) { return; }
    HoopzCharacter->CurrentState = 6;

    // For IK 
    FirstStep = true;
    FootPlanted = false;

    // Reset Stance
    HoopzCharacter->SetCapsuleHalfHeight(90, 80);
    IKAlpha = 0.9;
    LeftIKAlpha = 0.9;
    RightIKAlpha = 0.9;
}


/////////////////////////////// Helper Functions /////////////////////////////
void UMainAnimInstance::CapsuleTargetLerp(float DeltaTimeX, float TurnDuration)   
{
    if (!ensure(HoopzCharacter)) { return; }
    if (!ensure(PlayerCapsuleComponent)) { return; }
    // Turn Capsule Towards Basket
    // TODO : Allow for turn duration to be a character stat
    // TODO : Allow for multiple targets (Ball, Basket, Opponent) 
    if (HoopzCharacter) {
        FRotator CapsuleRotation = PlayerCapsuleComponent->GetComponentRotation();
        FRotator TargetCapsuleRotation = CapsuleRotation;
        FRotator LookAtCapsuleRotation = UKismetMathLibrary::FindLookAtRotation(PlayerCapsuleComponent->GetComponentLocation(), HoopzCharacter->BasketLocation);
        TargetCapsuleRotation.Yaw = LookAtCapsuleRotation.Yaw + HoopzCharacter->TotalRotation.Yaw;
        
        CapsuleTurnTime = 0;
        if (CapsuleTurnTime < CapsuleTurnDuration)
        {
            CapsuleTurnTime += DeltaTimeX;
            CapsuleRotation = FMath::Lerp(CapsuleRotation, TargetCapsuleRotation, CapsuleTurnTime / TurnDuration);
            PlayerCapsuleComponent->SetWorldRotation(CapsuleRotation, false);
        }
    }
}


///////////////////////////// Idle State - 0 //////////////////////////////////
// TODO: Have free look mode?
// TODO: Procedural Walk
void UMainAnimInstance::Idle(float DeltaTimeX)   
{
    if (!ensure(HoopzCharacter)) { return; }
    if (!ensure(PlayerSkeletalMesh)) { return; }

    // Main Locomotion Control Value
    FVector MotionDirection;
    MotionDirection.X = HoopzCharacter->GetVelocity().X; //HoopzCharacter->ForwardThrow.X + HoopzCharacter->RightThrow.X;  // * Capsule Velocity
    MotionDirection.Y = HoopzCharacter->GetVelocity().Y; //HoopzCharacter->ForwardThrow.Y + HoopzCharacter->RightThrow.Y;  // * Capsule Velocity
    MotionDirection.Z = 0;

    float CapsuleVelocity = HoopzCharacter->GetVelocity().Size();

    // If CapsuleVelocity = 0, Reset Cycle to 0
    if (CapsuleVelocity <= 0.1) {
        MotionTime = 0;
        MotionWave = 0;
        RightLegInterpTo = PlayerSkeletalMesh->GetSocketLocation("foot_r");
        LeftLegInterpTo = PlayerSkeletalMesh->GetSocketLocation("foot_l");
    }
    else {
        // Start Cycle with Foot that is closer to direction (unless Foot is at or close to MaxReach)
        // Set FName Foot & Disable IK for that foot

        // Start Cycle
        // if (CycleTime >= 1) {
        //     StartTime = GetWorld()->GetTimeSeconds();
        //     // Change Foot 
        //     return;
        // }

        // *** MAKE MOTION SPEED THE TIME VARIABLE FOR MOTION FREQUENCY ***

        // Rotate towards MotionDirection
        // Increase Speed of interpolation & Range of Motion for upperbody (based on Velocity)
        float MotionSpeed = CapsuleVelocity / 100;    // 100 = Max Move Speed - TODO : Soft Code
        MotionTime += DeltaTimeX * (1 + MotionSpeed); 
        
        // MotionFrequency also controls range of motion in the pelvis, spine, & upperbody
        // Sine wave linked to Chained Rotations - Multiply by FRotator
        InterpWave = UKismetMathLibrary::Sin(UKismetMathLibrary::GetPI() * MotionTime);
        MotionWave = CapsuleVelocity / 100 * InterpWave;
        float CycleTime = abs(UKismetMathLibrary::GenericPercent_FloatFloat(MotionTime, 1.f));

        // Add Offset Interpolation Points to increase range of motion & avoid Leg collisions
        // Adjust IK here
        if (FootKey == false) {   // Left Foot moves
            RightInterpSpeed = 100;
            if (CycleTime < 0.2) {
                if (PointSet1 == false) {
                    LeftLegInterpTo = PlayerSkeletalMesh->GetSocketLocation("foot_target_l1");// - MotionDirection;
                    PointSet1 = true;
                    LeftInterpSpeed = 5;
                }
            } else if (0.2 <= CycleTime && CycleTime < 0.4) {
                if (PointSet2 == false) {
                    LeftLegInterpTo = PlayerSkeletalMesh->GetSocketLocation("foot_target_l2") + MotionDirection;
                    PointSet2 = true;
                    LeftInterpSpeed = 10;
                }
            } else if (0.4 <= CycleTime && CycleTime < 0.7) {
                if (PointSet3 == false) {
                    LeftLegInterpTo = PlayerSkeletalMesh->GetSocketLocation("foot_target_l3") + MotionDirection / 2;
                    PointSet3 = true;
                    LeftInterpSpeed = 20;
                }
            } else if (0.7 <= CycleTime && CycleTime < 0.98) {
                if (PointSet4 == false) {
                    LeftLegInterpTo = PlayerSkeletalMesh->GetSocketLocation("foot_target_l4") + MotionDirection / 3;
                    PointSet4 = true;
                    LeftInterpSpeed = 40;
                }
            } else if (0.98 <= CycleTime) {   
                FootKey = true;
                MotionTime = 0;
                PointSet1 = false;
                PointSet2 = false;
                PointSet3 = false;
                PointSet4 = false;
                LeftInterpSpeed = 100;
                RightInterpSpeed = 25;
            }
        }
        else {   // Right Foot moves
            LeftInterpSpeed = 100;
            if (CycleTime < 0.2) {
                if (PointSet1 == false) {
                    RightLegInterpTo = PlayerSkeletalMesh->GetSocketLocation("foot_target_r1");// - MotionDirection;
                    PointSet1 = true;
                    RightInterpSpeed = 5;
                }
            } else if (0.2 <= CycleTime && CycleTime < 0.4) {
                if (PointSet2 == false) {
                    RightLegInterpTo = PlayerSkeletalMesh->GetSocketLocation("foot_target_r2") + MotionDirection;
                    PointSet2 = true;
                    RightInterpSpeed = 10;
                }
            } else if (0.4 <= CycleTime && CycleTime < 0.7) {
                if (PointSet3 == false) {
                    RightLegInterpTo = PlayerSkeletalMesh->GetSocketLocation("foot_target_r3") + MotionDirection / 2;
                    PointSet3 = true;
                    RightInterpSpeed = 20;
                }
            } else if (0.7 <= CycleTime && CycleTime < 0.98) {
                if (PointSet4 == false) {
                    RightLegInterpTo = PlayerSkeletalMesh->GetSocketLocation("foot_target_r4") + MotionDirection / 3;
                    PointSet4 = true;
                    RightInterpSpeed = 40;
                }
            } else if (0.98 <= CycleTime) {
                FootKey = false;
                MotionTime = 0;
                PointSet1 = false;
                PointSet2 = false;
                PointSet3 = false;
                PointSet4 = false;
                LeftInterpSpeed = 25;
                RightInterpSpeed = 100;
            }
        }

        InterpSpeed = 7;
    }

    // Apply Wave to Base Rotators - if MotionWave < 0, Set IK = 1 & stop motion
    // LeftLegBase = MotionWave * FRotator(0, 45, 0);
    // RightLegBase = MotionWave * FRotator(0, -45, 0);

    if (PivotInterpTime < PivotInterpDuration) {
        PivotInterpTime += DeltaTimeX;
        FVector LeftFootCurrent = PlayerSkeletalMesh->GetSocketLocation("foot_l");
        LeftFootCurrent = FMath::VInterpTo(LeftFootCurrent, LeftLegInterpTo, DeltaTimeX, LeftInterpSpeed);
        LeftLegTarget.X = LeftFootCurrent.X;
        LeftLegTarget.Y = LeftFootCurrent.Y;
        FVector RightFootCurrent = PlayerSkeletalMesh->GetSocketLocation("foot_r");
        RightFootCurrent = FMath::VInterpTo(RightFootCurrent, RightLegInterpTo, DeltaTimeX, RightInterpSpeed);
        RightLegTarget.X = RightFootCurrent.X;
        RightLegTarget.Y = RightFootCurrent.Y;
    }
        
    PivotInterpTime = 0;

    // UE_LOG(LogTemp, Warning, TEXT("Motion Time: %f"), MotionTime)
    // UE_LOG(LogTemp, Warning, TEXT("Motion Wave: %f"), MotionWave)
    // UE_LOG(LogTemp, Warning, TEXT("Motion Direction: %s"), *MotionDirection.ToString())
    
}

///////////////////////////// Pivot State - 1 //////////////////////////////////
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
                    if (HoopzCharacter->PivotKey == false) { // Plant Right Foot - Left Foot Moves
                        PivotRightFootAnchor = PlayerSkeletalMesh->GetSocketLocation("foot_r");   // TODO : Toggle Foot IK
                        FirstStepLeftFootLocation = PlayerSkeletalMesh->GetSocketLocation("foot_l");
                    } else {  // Plant Left Foot
                        PivotLeftFootAnchor = PlayerSkeletalMesh->GetSocketLocation("foot_l");
                        FirstStepRightFootLocation = PlayerSkeletalMesh->GetSocketLocation("foot_r");
                    }
                    FootPlanted = true;
                }

                // CanMove = false;
                HoopzCharacter->CanTurn = false;

                // Set New Anchors - Capsule Rotation & OffFoot
                if (HoopzCharacter->PivotKey == false) {   // Right Foot Planted
                    if (HoopzCharacter->PivotTurnRight == true) {        
                        CapsuleAnchorRotation = HoopzCharacter->PivotPointRotation;
                        HoopzCharacter->PivotTurnRight = false;
                    }
                    if (HoopzCharacter->PivotTurnLeft == true) {
                        CapsuleAnchorRotation = HoopzCharacter->PivotPointRotation;
                        HoopzCharacter->PivotTurnLeft = false;
                    }
                }
                else  // Left Foot Planted
                {
                    if (HoopzCharacter->PivotTurnRight == true) {;
                        CapsuleAnchorRotation = HoopzCharacter->PivotPointRotation;
                        HoopzCharacter->PivotTurnRight = false;
                    }
                    if (HoopzCharacter->PivotTurnLeft == true) {
                        CapsuleAnchorRotation = HoopzCharacter->PivotPointRotation;
                        HoopzCharacter->PivotTurnLeft = false;
                    }
                }

                // Turn - Set NewCapsuleRotation & Call Pivot Step w/ PrevPoseKey
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

void UMainAnimInstance::OnStepTimerExpire()
{
    CanMove = true;
    HoopzCharacter->CanTurn = true;
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
        CapsuleLocation = FMath::VInterpTo(CapsuleLocation, NewCapsuleLocation, DeltaTimeX, 5);
        CapsuleRotation = FMath::Lerp(CapsuleRotation, NewCapsuleRotation, PivotInterpTime / 0.15);   // speed up lerp
        PlayerCapsuleComponent->SetWorldLocation(CapsuleLocation, false);
        PlayerCapsuleComponent->SetWorldRotation(CapsuleRotation, false);

        if (HoopzCharacter->PivotKey == false) { // left foot moves
            FootLocation = FMath::VInterpTo(FootLocation, NewOffFootLocation, DeltaTimeX, 5);
            NewLeftFootLocation.X = FootLocation.X;
            NewLeftFootLocation.Y = FootLocation.Y;
        } else {
            FootLocation = FMath::VInterpTo(FootLocation, NewOffFootLocation, DeltaTimeX, 5);
            NewRightFootLocation.X = FootLocation.X;
            NewRightFootLocation.Y = FootLocation.Y;
        }
    }
        
    PivotInterpTime = 0;
}

void UMainAnimInstance::PivotStep()
{
    CanMove = false;
    PrevPoseKey = PoseKey;
    if (!ensure(PlayerCapsuleComponent)) { return; }
    if (!ensure(HoopzCharacter)) { return; }
    
    // Move Capsule and Off Foot to New Location
    NewCapsuleRotation = HoopzCharacter->PivotPointRotation;
    if (HoopzCharacter->PivotKey == false) { // left foot moves
        switch (PoseKey) {
            case 0: // Left - Forward
                
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


///////////////////////////// Dribble State - 3 //////////////////////////////////
// TODO: Implement Dribble animation - Right & Left Shoulder Trigger
void UMainAnimInstance::IdleDribble(float DeltaTimeX)    
{
    if (!ensure(HoopzCharacter)) { return; }

    // ThrowX = HoopzCharacter->ThrowY * 65;
    // ThrowY = HoopzCharacter->ThrowX * 65;
}


///////////////////////////// Jump State - 4 //////////////////////////////////
// TODO: On landing play landed animation
// TODO: Have different jumps for Offball offense/defense & different look targets(basket, ball, free) / modes
// TODO: Rotate Lower arm back to charge shot by holding shot button
void UMainAnimInstance::WhileJumped(float DeltaTimeX)     
{
    if (!ensure(HoopzCharacter)) { return; }
    if (!ensure(PlayerCapsuleComponent)) { return; }

    if (HoopzCharacter->CanChangeShot) {   // TODO: When can I stop checking ensure?
        ShotPoseIndex = HoopzCharacter->ShotKey;
        if (ShotPoseIndex != 0) {
            HoopzCharacter->CanChangeShot = false;  // TODO: Consider allowing shot change
            HasBall = false;  // after shot
        }
    }
    
}


///////////////////////////// Offense State - 6 //////////////////////////////////
// Todo: Snap to closest SET 45 degree angle? (free look mode)
void UMainAnimInstance::IdleOffense(float DeltaTimeX)   
{
    if (!ensure(HoopzCharacter)) { return; }

    // ThrowX = HoopzCharacter->ThrowY * 65;
    // ThrowY = HoopzCharacter->ThrowX * 65;

}


//////////////////////////////////////// IK Foot Placement /////////////////////////////////////
// TODO : Move to SubAnimInstance on Refactor
// TODO : Ragdoll IK
// TODO : Toe IK
FVector UMainAnimInstance::IKFootTrace(int32 Foot, float DeltaTimeX)
{
    if (!ensure(HoopzCharacter)) { return FVector(0, 0, 0); }
    if (!ensure(PlayerSkeletalMesh)) { return FVector(0, 0, 0); }
    if (!ensure(PlayerCapsuleComponent)) { return FVector(0, 0, 0); }

    /*
    if (Motion == true) {    // Locomotion
        if (Foot == 0) {     // Left Foot
            FootSocketLocation = LeftFootInterpPoint;
        } else {
            FootSocketLocation = RightFootInterpPoint;
        }
    }
    */

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
            if (FootKey == false) {
                FootSocketLocation = LeftLegTarget;
            } else { 
                FootSocketLocation = LeftLegInterpTo;
            }
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
            if (FootKey == true) {
                FootSocketLocation = RightLegTarget;
            } else { 
                FootSocketLocation = RightLegInterpTo;
            }
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
        // if (Foot == 0) { 
        // LeftFootRotation.Roll = UKismetMathLibrary::DegAtan2(HitResult.Normal.Y, HitResult.Normal.Z);
        // LeftFootRotation.Pitch = UKismetMathLibrary::DegAtan2(HitResult.Normal.X, HitResult.Normal.Z);
        // } else { 
        // RightFootRotation.Roll = UKismetMathLibrary::DegAtan2(HitResult.Normal.Y, HitResult.Normal.Z);
        // RightFootRotation.Pitch = UKismetMathLibrary::DegAtan2(HitResult.Normal.X, HitResult.Normal.Z);
        // } 

        return FootSocketLocation; 
    }
    
    return FootSocketLocation;  // else - don't offset
}

