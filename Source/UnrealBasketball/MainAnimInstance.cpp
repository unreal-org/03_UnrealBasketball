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
    //PelvisMotion = FRotator(0, 90, -90);

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
        case 2: // PostUp
            IdlePostUp(DeltaTimeX);
            CapsuleTargetLerp(DeltaTimeX, .05);
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

    // Set Limb Scale
    ArmScale = FVector(1.1, 1, 1);
    LegScale = FVector(1.3, 1, 1);

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
    HoopzCharacter->SetCapsuleHalfHeight(100, 85);
    RightLegInterpTo = PlayerSkeletalMesh->GetSocketLocation("foot_r");
    LeftLegInterpTo = PlayerSkeletalMesh->GetSocketLocation("foot_l");
    IKAlpha = 0.95;
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
    if (HoopzCharacter->PivotKey == false) {   // left foot moves
        FirstStepLeftFootLocation = PlayerSkeletalMesh->GetSocketLocation("foot_l");
        NewOffFootLocation = PlayerSkeletalMesh->GetSocketLocation("ik_foot_l");
    } else {
        FirstStepRightFootLocation = PlayerSkeletalMesh->GetSocketLocation("foot_r");
        NewOffFootLocation = PlayerSkeletalMesh->GetSocketLocation("ik_foot_r");
    }

    // Reset Pivot
    CanMove = false;
    HoopzCharacter->CanTurn = true;
    HoopzCharacter->PivotInputKey = -1;
    FootPlanted = false;
    FirstStep = true;
    HoopzCharacter->PivotMode = true;
    HoopzCharacter->EstablishPivot = false;
    HoopzCharacter->PivotAttached = false;
    FVector ComponentLocation = PlayerCapsuleComponent->GetComponentLocation();
    ComponentLocation.Z = 0;
    HoopzCharacter->PivotPoint->SetActorLocation(ComponentLocation, false);
	HoopzCharacter->PivotPoint->SetActorRotation(PlayerCapsuleComponent->GetComponentRotation(), ETeleportType::None);

    // Reset Dribble transition variable
    Dribble = false;

    // Jump
    LeftFootJump = false;
    RightFootJump = false;

    // Post Pose Key & Mods
    PostPoseIndex = -1;
    PostUpSpeedMod = 1;
    PostUpInterpMod = 0;
    PostUpReverse = 1;

    // Reset Stance
    HoopzCharacter->SetCapsuleHalfHeight(95, 85);
    IKAlpha = 1;
    LeftIKAlpha = 1;
    RightIKAlpha = 1;
}

void UMainAnimInstance::AnimNotify_PostUpEntry()
{
    if (!ensure(HoopzCharacter)) { return; }
    HoopzCharacter->CurrentState = 2;

     // Reset Pivot
    CanMove = false;
    HoopzCharacter->CanTurn = true;
    FootPlanted = false;
    HoopzCharacter->PivotMode = false;
    HoopzCharacter->EstablishPivot = false;
    HoopzCharacter->PivotAttached = false;

    // PostUp Mods
    PostUpSpeedMod = 2;
    PostUpInterpMod = 2.5;
    PostUpReverse = -1;

    // Jump
    LeftFootJump = false;
    RightFootJump = false;

    // Reset Stance
    HoopzCharacter->SetCapsuleHalfHeight(95, 85);
    IKAlpha = 0.95;
    LeftIKAlpha = 0.9;
    RightIKAlpha = 0.9;
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

    // Post Pose Key
    PostPoseIndex = -1;
    PostUpSpeedMod = 1;
    PostUpInterpMod = 0;
    PostUpReverse = 1;

    // Jump
    LeftFootJump = false;
    RightFootJump = false;

    // Reset Stance
    HoopzCharacter->SetCapsuleHalfHeight(100, 85);
    RightLegInterpTo = PlayerSkeletalMesh->GetSocketLocation("foot_r");
    LeftLegInterpTo = PlayerSkeletalMesh->GetSocketLocation("foot_l");
    IKAlpha = 0.95;
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

    // Reset Pose Keys
    PostPoseIndex = -1;

    // Reset Stance
    HoopzCharacter->SetCapsuleHalfHeight(100, 85);
    RightLegInterpTo = PlayerSkeletalMesh->GetSocketLocation("foot_r");
    LeftLegInterpTo = PlayerSkeletalMesh->GetSocketLocation("foot_l");
    IKAlpha = 0.9;
    LeftIKAlpha = 0.5;
    RightIKAlpha = 0.5;
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

void UMainAnimInstance::Locomotion(float DeltaTimeX)
{
    if (!ensure(HoopzCharacter)) { return; }
    if (!ensure(PlayerSkeletalMesh)) { return; }

    // Main Locomotion Control Value
    FVector MotionDirection;
    MotionDirection.X = HoopzCharacter->GetVelocity().X; 
    MotionDirection.Y = HoopzCharacter->GetVelocity().Y;
    MotionDirection.Z = 0;

    FVector RightFoot = PlayerSkeletalMesh->GetSocketLocation("foot_r");
    FVector LeftFoot = PlayerSkeletalMesh->GetSocketLocation("foot_l");

    float CapsuleVelocity = HoopzCharacter->GetVelocity().Size();

    // If CapsuleVelocity = 0, Reset Cycle to 0
    if (CapsuleVelocity <= 0.1) {     /////////////////////////// Velocity Hits 0 on direction change - TODO : Use another variable? ////////////////////////////////////
        LeftInterpSpeed = 5;
        RightInterpSpeed = 5;
        PelvisHeight.Z = 0;
        if (InitialPose == true) {
            RightLegInterpTo = PlayerSkeletalMesh->GetSocketLocation("foot_r");
            LeftLegInterpTo = PlayerSkeletalMesh->GetSocketLocation("foot_l");
            InitialPose = false;
        }

        if (HoopzCharacter->LocomotionTurn == true) {
            RightLegInterpTo = PlayerSkeletalMesh->GetSocketLocation("ik_foot_r");
            LeftLegInterpTo = PlayerSkeletalMesh->GetSocketLocation("ik_foot_l");
            HoopzCharacter->LocomotionTurn = false;
        }
        // Maintain Natural Foot Positions - TODO : FIX
        // if ((RightFoot - PlayerSkeletalMesh->GetSocketLocation("ik_foot_r")).Size() >= MaxStepOffset) {
        //     RightInterpSpeed = 10;
        //     RightLegInterpTo = PlayerSkeletalMesh->GetSocketLocation("ik_foot_r");
        // }
        // if ((LeftFoot - PlayerSkeletalMesh->GetSocketLocation("ik_foot_l")).Size() >= MaxStepOffset) {
        //     LeftInterpSpeed = 10;
        //     LeftLegInterpTo = PlayerSkeletalMesh->GetSocketLocation("ik_foot_l");
        // }

        LeftFootJump = false;
        RightFootJump = false;
        InitialStep = true;
        
    }
    else {
        // Start Cycle with Foot that is closer to direction (unless Foot is at or close to MaxReach) & Set FName Foot
        if (InitialStep == true) {
            InitialStep = false;
            FVector NewDirection = PlayerCapsuleComponent->GetComponentLocation() + MotionDirection;
            if ((NewDirection - RightFoot).Size() <= (NewDirection - LeftFoot).Size()) {  // Right Foot Closer
                FootKey = true;  // Move Right Foot First
                if ((RightFoot - PlayerCapsuleComponent->GetComponentLocation()).Size() >= MaxReach) {  // if foot already extended
                    FootKey = false;  // Move Left Foot
                }
            } else  {   // Vice Versa
                FootKey = false; 
                if ((LeftFoot - PlayerCapsuleComponent->GetComponentLocation()).Size() >= MaxReach) {
                    FootKey = true;
                }
            }
            MotionTime = 0;
            PointSet1 = false;
            PointSet2 = false;
            PointSet3 = false;
            PointSet4 = false;
        }

        // TODO : FIX FIRST STEP LAG

        // Increase Speed of interpolation & Range of Motion for upperbody (based on Velocity)
        float MotionSpeed = CapsuleVelocity / 75 * PostUpSpeedMod;    // 100 = Max Move Speed - TODO : Soft Code
        MotionTime += DeltaTimeX * (1 + MotionSpeed); 
        float InterpWave = UKismetMathLibrary::Sin(UKismetMathLibrary::GetPI() * MotionTime);
        float HeightWave = UKismetMathLibrary::Cos(UKismetMathLibrary::GetPI() * MotionTime * 2);
        MotionWave = MotionSpeed * InterpWave;
        float CycleTime = abs(UKismetMathLibrary::GenericPercent_FloatFloat(MotionTime, 1.f));
        PelvisMotion = FRotator(-HoopzCharacter->ForwardThrow * 4 * MotionSpeed, 0, HoopzCharacter->RightThrow * 4 * MotionSpeed);

        // MotionWave also controls range of motion in the pelvis, spine, & upperbody
        // Sine wave linked to Chained Rotations - Multiply by FRotator
        // Add Offset Interpolation Points to increase range of motion & avoid Leg collisions
        if (FootKey == false) {   // Left Foot moves
            PelvisMotion += MotionWave * FRotator(2, -3, 1);
            PelvisHeight.Z = HeightWave * MotionSpeed * 2 - 1;
            Spine2Motion = -MotionWave * FRotator(2, 2, 1);
            RightFootJump = true;
            
            if (CycleTime < 0.2) {   
                if (PointSet1 == false) {
                    LeftLegInterpTo = PlayerSkeletalMesh->GetSocketLocation("foot_target_l1") - MotionDirection / (4.5 + PostUpInterpMod) + MotionSpeed * FVector(0, 0, 15 / PostUpSpeedMod);
                    PointSet1 = true;
                    LeftInterpSpeed = 5;
                }
            } else if (0.2 <= CycleTime && CycleTime < 0.5) {
                if (PointSet2 == false) {
                    LeftLegInterpTo = PlayerSkeletalMesh->GetSocketLocation("foot_target_l1") + MotionDirection / (1.5 + PostUpInterpMod) - MotionSpeed * FVector(0, 0, 5 / PostUpSpeedMod);
                    PointSet2 = true;
                    LeftInterpSpeed = 10;
                }
            } else if (0.5 <= CycleTime && CycleTime < 0.8) {
                if (PointSet3 == false) {
                    LeftLegInterpTo = PlayerSkeletalMesh->GetSocketLocation("foot_target_l1") + MotionDirection / (2.5 + PostUpInterpMod) + MotionSpeed * FVector(0, 0, .10 / PostUpSpeedMod);
                    PointSet3 = true;
                    LeftInterpSpeed = 15;
                }
            } else if (0.8 <= CycleTime && CycleTime < 0.95) {
                if (PointSet4 == false) {
                    LeftLegInterpTo = PlayerSkeletalMesh->GetSocketLocation("foot_target_l1") + MotionDirection / (3.5 + PostUpInterpMod) - MotionSpeed * FVector(0, 0, 5 / PostUpSpeedMod);
                    PointSet4 = true;
                    LeftInterpSpeed = 20;
                }
            } else if (0.95 <= CycleTime) {   
                FootKey = true;
                RightFootJump = false;
                MotionTime = 0;
                PointSet1 = false;
                PointSet2 = false;
                PointSet3 = false;
                PointSet4 = false;
                LeftLegInterpTo = PlayerSkeletalMesh->GetSocketLocation("foot_l");
                LeftLegInterpTo.Z = 0;
                LeftIKAlpha = 1;
            }
        }
        else {   // Right Foot moves
            PelvisMotion += MotionWave * FRotator(1, 3, -1);
            PelvisHeight.Z = HeightWave * MotionSpeed * 2 - 1;
            Spine2Motion = -MotionWave * FRotator(1, -2, -2);
            LeftFootJump = true;

            if (CycleTime < 0.2) {
                if (PointSet1 == false) {
                    RightLegInterpTo = PlayerSkeletalMesh->GetSocketLocation("foot_target_r1") - MotionDirection / (4.5 + PostUpInterpMod) + MotionSpeed * FVector(0, 0, 15 / PostUpSpeedMod);
                    PointSet1 = true;
                    RightInterpSpeed = 5;
                }
            } else if (0.2 <= CycleTime && CycleTime < 0.5) {
                if (PointSet2 == false) {
                    RightLegInterpTo = PlayerSkeletalMesh->GetSocketLocation("foot_target_r1") + MotionDirection / (1.5 + PostUpInterpMod) - MotionSpeed * FVector(0, 0, 5 / PostUpSpeedMod);
                    PointSet2 = true;
                    RightInterpSpeed = 10;
                }
            } else if (0.5 <= CycleTime && CycleTime < 0.8) {
                if (PointSet3 == false) {
                    RightLegInterpTo = PlayerSkeletalMesh->GetSocketLocation("foot_target_r1") + MotionDirection / (2.5 + PostUpInterpMod) + MotionSpeed * FVector(0, 0, 10 / PostUpSpeedMod);
                    PointSet3 = true;
                    RightInterpSpeed = 15;
                }
            } else if (0.8 <= CycleTime && CycleTime < 0.95) {
                if (PointSet4 == false) {
                    RightLegInterpTo = PlayerSkeletalMesh->GetSocketLocation("foot_target_r1") + MotionDirection / (3.5 + PostUpInterpMod) - MotionSpeed * FVector(0, 0, 5 / PostUpSpeedMod);
                    PointSet4 = true;
                    RightInterpSpeed = 20;
                }
            } else if (0.95 <= CycleTime) {
                FootKey = false;
                LeftFootJump = false;
                MotionTime = 0;
                PointSet1 = false;
                PointSet2 = false;
                PointSet3 = false;
                PointSet4 = false;
                RightLegInterpTo = PlayerSkeletalMesh->GetSocketLocation("foot_r");
                RightLegInterpTo.Z = 0;
                RightIKAlpha = 1;
            }
        }
    }

    if (PivotInterpTime < PivotInterpDuration) {
        PivotInterpTime += DeltaTimeX;
        FVector LeftFootCurrent = PlayerSkeletalMesh->GetSocketLocation("foot_l");
        FVector RightFootCurrent = PlayerSkeletalMesh->GetSocketLocation("foot_r");
        if (InitialStep == false) {
            if (FootKey == false) {  // left foot moves
                LeftFootCurrent = FMath::VInterpTo(LeftFootCurrent, LeftLegInterpTo, DeltaTimeX, LeftInterpSpeed);
                LeftLegTarget = LeftFootCurrent;
                RightLegTarget = RightLegInterpTo;
            } else {   // right foot moves
                RightFootCurrent = FMath::VInterpTo(RightFootCurrent, RightLegInterpTo, DeltaTimeX, RightInterpSpeed);
                RightLegTarget = RightFootCurrent;
                LeftLegTarget = LeftLegInterpTo;
            }
        } else {
            LeftFootCurrent = FMath::VInterpTo(LeftFootCurrent, LeftLegInterpTo, DeltaTimeX, LeftInterpSpeed);
            LeftLegTarget = LeftFootCurrent;
            RightFootCurrent = FMath::VInterpTo(RightFootCurrent, RightLegInterpTo, DeltaTimeX, RightInterpSpeed);
            RightLegTarget = RightFootCurrent;
        }
    }
        
    PivotInterpTime = 0;
}


///////////////////////////// Idle State - 0 //////////////////////////////////
// TODO: Have free look mode?
// TODO: Procedural Walk
void UMainAnimInstance::Idle(float DeltaTimeX)   
{
    Locomotion(DeltaTimeX);
}


///////////////////////////// Idle State - 0 //////////////////////////////////
void UMainAnimInstance::IdlePostUp(float DeltaTimeX)
{
    Locomotion(DeltaTimeX);
}


///////////////////////////// Pivot State - 1 //////////////////////////////////
// TODO : Check for Collisions on PivotStep & PivotTurn
// TODO : Fix Multiple pivot input
// TODO : Find Non-Reproducible Bug with OffFootLocation on first step pivot(Right Foot)
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

    // TODO : Consider moving Pivot Interp out here.

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
    Locomotion(DeltaTimeX);
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
        if (LeftFootJump == true) { ShotPoseIndex = 1; }
		else if (RightFootJump == true) { ShotPoseIndex = 2; }
		else if (RightFootJump == false && LeftFootJump == false) { ShotPoseIndex = 0; }

        if (HoopzCharacter->ShotSelect == true) {
            ShotPoseIndex = HoopzCharacter->ShotKey;
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
    if (!ensure(MainState)) { return FVector(0, 0, 0); }

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
            if (MainState->GetCurrentState() == 0 || MainState->GetCurrentState() == 2 || MainState->GetCurrentState() == 3) {
                if (FootKey == false) {
                    FootSocketLocation = LeftLegTarget;
                    return FootSocketLocation;
                }
                else { FootSocketLocation = LeftLegInterpTo; }
            } else {
                FootSocketLocation = PlayerSkeletalMesh->GetSocketLocation(FootName);
            }
        }

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
            if (MainState->GetCurrentState() == 0 || MainState->GetCurrentState() == 2 || MainState->GetCurrentState() == 3) {
                if (FootKey == true) {
                    FootSocketLocation = RightLegTarget;
                    return FootSocketLocation;
                }
                else { FootSocketLocation = RightLegInterpTo; }
            } else {
                FootSocketLocation = PlayerSkeletalMesh->GetSocketLocation(FootName);
            }
        }
    }

    float CapsuleHalfHeight = PlayerCapsuleComponent->GetUnscaledCapsuleHalfHeight();
    FVector StartTrace = FVector(FootSocketLocation.X, FootSocketLocation.Y, CapsuleHalfHeight);
    FVector EndTrace = FVector(FootSocketLocation.X, FootSocketLocation.Y, CapsuleHalfHeight - CapsuleHalfHeight - 30); // 30 = trace distance;

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
        FootSocketLocation.Z = (HitResult.Location - HitResult.TraceEnd).Size() - 30 + 13.5 * LegScale.X;
        
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

