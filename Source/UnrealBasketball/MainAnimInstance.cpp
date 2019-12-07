// ChoiBoi Copyrights

#include "MainAnimInstance.h"
#include "Animation/AnimNode_StateMachine.h"
#include "GameFramework/Actor.h"
#include "PlayerCapsuleComponent.h"
#include "Animation/AnimNotifyQueue.h"
#include "TimerManager.h"
#include "HoopzCharacter.h"
#include "Kismet/KismetMathLibrary.h"

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
   
    // UE_LOG(LogTemp, Warning, TEXT("%s"), *PlayerSkeletalMesh->GetSocketLocation(FName(TEXT("foot_l"))).ToString())

    switch (MainState->GetCurrentState())
    {
        case 0: // Idle
            Idle(DeltaTimeX);
            break;
        case 1: // IdlePivot
            Pivot(DeltaTimeX);
            break;
        case 4: // Jump (Ball)
            WhileJumped(DeltaTimeX);
        case 6: // IdleOffense
            IdleOffense(DeltaTimeX);
            break;
        default:
            return;
    }

    LeftFootLocation = IKFootTrace(0, DeltaTimeX);
    RightFootLocation = IKFootTrace(1, DeltaTimeX);

    // float Temp = (PlayerSkeletalMesh->GetSocketLocation(FName(TEXT("foot_l"))) + PlayerSkeletalMesh->GetSocketLocation(FName(TEXT("root")))).Size();
    // UE_LOG(LogTemp, Warning, TEXT("%f"), Temp)   // *** 27.64 ***

}

void UMainAnimInstance::AnimNotify_IdleOffense() 
{
    FootPlanted = false;
    IKAlpha = 0.9;
}
void UMainAnimInstance::AnimNotify_SetBasketLocation()
{
    if (!ensure(HoopzCharacter)) { return; }
    HoopzCharacter->PivotMode = false;
    FootPlanted = false;
    IKAlpha = 0.9;
}
void UMainAnimInstance::AnimNotify_SetPivot()
{ 
    if (!ensure(HoopzCharacter)) { return; }
    HoopzCharacter->PivotMode = true;
    FootPlanted = false;
    // lower Capsule height
    IKAlpha = 1;
}
void UMainAnimInstance::AnimNotify_PivotToJumpTransition()
{
    FootPlanted = false;
    PivotPoseIndex = 0;
    ShotPoseIndex = 0;
    IKAlpha = 0.25;
}

void UMainAnimInstance::IdleOffense(float DeltaTimeX)
{
    ThrowX = HoopzCharacter->ThrowY * 65;
    ThrowY = HoopzCharacter->ThrowX * 65;

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
            CapsuleRotation = FMath::Lerp(CapsuleRotation, TargetCapsuleRotation, CapsuleTurnTime / CapsuleTurnDuration);
            PlayerCapsuleComponent->SetWorldRotation(CapsuleRotation, false);
        }
    }
}

void UMainAnimInstance::OnStepTimerExpire()
{
    CanMove = true;
}

// TODO : Play idlepivot animation if idle for more than 5 seconds?
void UMainAnimInstance::Pivot(float DeltaTimeX)
{
    if (!ensure(HoopzCharacter)) { return; }
    if (!ensure(PlayerCapsuleComponent)) { return; }

    // Blend Pose by Int
    if (CanMove == true) {
        // To disable concurrent turn & step

        if (CanMove == true) {
            PoseKey = HoopzCharacter->PivotInputKey;
            if (HoopzCharacter->EstablishPivot == true) {
                // Achor Planted Foot Location & Initial Capsule Location
                if (FootPlanted == false) 
                {
                    if (HoopzCharacter->PivotKey == false) // Plant Right Foot
                    {
                        PivotRightFootLocation = PlayerSkeletalMesh->GetSocketLocation("foot_r");   // TODO : Toggle Foot IK
                        PivotAnchorLocation = PlayerCapsuleComponent->GetComponentLocation();
                        OffFootLocation = PlayerSkeletalMesh->GetSocketLocation("foot_l");
                        //NewLeftFootLocation = PlayerSkeletalMesh->GetSocketLocation("foot_l");
                    }
                    else
                    {
                        PivotLeftFootLocation = PlayerSkeletalMesh->GetSocketLocation("foot_l");
                        PivotAnchorLocation = PlayerCapsuleComponent->GetComponentLocation();
                        OffFootLocation = PlayerSkeletalMesh->GetSocketLocation("foot_r");
                        //NewRightFootLocation = PlayerSkeletalMesh->GetSocketLocation("foot_r");
                    }
                    FootPlanted = true;
                }

                if (PoseKey != PrevPoseKey) { PivotStep(); }
            }
        }
    } 
    else {
        FTimerHandle StepTimer;
	    GetWorld()->GetTimerManager().SetTimer(StepTimer, this, &UMainAnimInstance::OnStepTimerExpire, StepDelay, false);
        PivotInterp(DeltaTimeX, NewStepLocation);
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

void UMainAnimInstance::PivotInterp(float DeltaTimeX, FVector NewLocation)
{
    FVector CapsuleLocation = PlayerCapsuleComponent->GetComponentLocation();
    FVector FootLocation;
    FName Foot;
    if (HoopzCharacter->PivotKey == false) { // left foot moves
        FootLocation = PlayerSkeletalMesh->GetSocketLocation("foot_l");
    } else {
        FootLocation = PlayerSkeletalMesh->GetSocketLocation("foot_r");
    }

    if (CapsuleInterpTime < CapsuleInterpDuration)
    {
        CapsuleInterpTime += DeltaTimeX;
        CapsuleLocation = FMath::VInterpTo(CapsuleLocation, NewLocation, DeltaTimeX, 7);
        PlayerCapsuleComponent->SetWorldLocation(CapsuleLocation, false);

        // if (HoopzCharacter->PivotKey == false) { // left foot moves
        //     FootLocation = FMath::VInterpTo(FootLocation, NewFootLocation, DeltaTimeX, 7);
        //     NewLeftFootLocation.X = FootLocation.X;
        //     NewLeftFootLocation.Y = FootLocation.Y;
        // } else {
        //     FootLocation = FMath::VInterpTo(FootLocation, NewFootLocation, DeltaTimeX, 7);
        //     NewRightFootLocation.X = FootLocation.X;
        //     NewRightFootLocation.Y = FootLocation.Y;
    }
        
    CapsuleInterpTime = 0;
}

void UMainAnimInstance::PivotStep()
{
    CanMove = false;
    PrevPoseKey = PoseKey;
    if (!ensure(PlayerCapsuleComponent)) { return; }
    if (!ensure(HoopzCharacter)) { return; }
    if (HoopzCharacter->PivotKey == false) { // left foot moves
        switch (PoseKey) {
            case 0: // Left - Forward
            {
                // Move Capsule and Off Foot to New Location
                NewStepLocation = PivotAnchorLocation + PlayerCapsuleComponent->GetForwardVector() * 35;
                NewFootLocation = OffFootLocation + PlayerCapsuleComponent->GetForwardVector() * 70;
                break;
            }
            case 1: // Left - Forward Left
            {
                NewStepLocation = PivotAnchorLocation + PlayerCapsuleComponent->GetForwardVector() * 25 + PlayerCapsuleComponent->GetRightVector() * -25;
                NewFootLocation = OffFootLocation + PlayerCapsuleComponent->GetForwardVector() * 50 + PlayerCapsuleComponent->GetRightVector() * -50;
                break;
            }
            case 2: // Left - Left
            {
                NewStepLocation = PivotAnchorLocation + PlayerCapsuleComponent->GetRightVector() * -35;
                NewFootLocation = OffFootLocation + PlayerCapsuleComponent->GetRightVector() * -70;
                break;
            }
            case 3: // Left - Back Left
            {
                NewStepLocation = PivotAnchorLocation + PlayerCapsuleComponent->GetForwardVector() * -25 + PlayerCapsuleComponent->GetRightVector() * -25;
                NewFootLocation = OffFootLocation + PlayerCapsuleComponent->GetForwardVector() * -50 + PlayerCapsuleComponent->GetRightVector() * -50;
                break;
            }
            case 4: // Left - Back
            {
                NewStepLocation = PivotAnchorLocation + PlayerCapsuleComponent->GetForwardVector() * -35;
                NewFootLocation = OffFootLocation + PlayerCapsuleComponent->GetForwardVector() * -70;
                break;
            }
            default: // Idle Pivot
                NewStepLocation = PivotAnchorLocation;
                NewFootLocation = OffFootLocation;
                return;   
        }
    }
    
    else { // right foot moves
        switch (PoseKey) {
            case 5: // Right - Back
            {
                NewStepLocation = PivotAnchorLocation + PlayerCapsuleComponent->GetForwardVector() * -35;
                NewFootLocation = OffFootLocation + PlayerCapsuleComponent->GetForwardVector() * -70;
                break;
            }
            case 6: // Right - Back Right
            {
                NewStepLocation = PivotAnchorLocation + PlayerCapsuleComponent->GetForwardVector() * -25 + PlayerCapsuleComponent->GetRightVector() * 25;
                NewFootLocation = OffFootLocation + PlayerCapsuleComponent->GetForwardVector() * -50 + PlayerCapsuleComponent->GetRightVector() * 50;
                break;
            }
            case 7: // Right - Right
            {
                NewStepLocation = PivotAnchorLocation + PlayerCapsuleComponent->GetRightVector() * 35;
                NewFootLocation = OffFootLocation + PlayerCapsuleComponent->GetRightVector() * 70;
                break;
            }
            case 8: // Right - Forward Right
            {
                NewStepLocation = PivotAnchorLocation + PlayerCapsuleComponent->GetForwardVector() * 25 + PlayerCapsuleComponent->GetRightVector() * 25;
                NewFootLocation = OffFootLocation + PlayerCapsuleComponent->GetForwardVector() * 50 + PlayerCapsuleComponent->GetRightVector() * 50;
                break;
            }
            case 9: // Right - Forward
            {
                NewStepLocation = PivotAnchorLocation + PlayerCapsuleComponent->GetForwardVector() * 35;
                NewFootLocation = OffFootLocation + PlayerCapsuleComponent->GetForwardVector() * 70;
                break;
            }
            default: // Idle Pivot
                NewStepLocation = PivotAnchorLocation;
                NewFootLocation = OffFootLocation;
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
            if (HoopzCharacter->PivotKey == true) {
                FootSocketLocation = PivotLeftFootLocation;
            } else {
                FVector FootLocation = PlayerSkeletalMesh->GetSocketLocation(FootName);
                if (PivotTime < PivotDuration) {
                    PivotTime += DeltaTimeX;
                    FootSocketLocation = FMath::VInterpTo(FootLocation, NewFootLocation, DeltaTimeX, 7);
                }
                PivotTime = 0;
            }
        } else {
            FootSocketLocation = PlayerSkeletalMesh->GetSocketLocation(FootName);
        }

        // TODO: Foot Placement - Return if foot is inactive
    } else { 
        FootName = FName(TEXT("foot_r"));
        RightJointTargetLocation = PlayerSkeletalMesh->GetSocketLocation(FName(TEXT("joint_target_r")));

        if (FootPlanted == true)  {
            if (HoopzCharacter->PivotKey == false) {
                FootSocketLocation = PivotRightFootLocation;
            } else {
                FVector FootLocation = PlayerSkeletalMesh->GetSocketLocation(FootName);
                if (PivotTime < PivotDuration) {
                    PivotTime += DeltaTimeX;
                    FootSocketLocation = FMath::VInterpTo(FootLocation, NewFootLocation, DeltaTimeX, 7);
                }
                PivotTime = 0;
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

