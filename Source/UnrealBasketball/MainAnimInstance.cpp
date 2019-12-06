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
            Pivot();
            break;
        case 4: // Jump (Ball)
            WhileJumped(DeltaTimeX);
        case 6: // IdleOffense
            IdleOffense(DeltaTimeX);
            break;
        default:
            return;
    }

    LeftFootLocation = IKFootTrace(0);
    RightFootLocation = IKFootTrace(1);

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
    IKAlpha = 0.9;
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
void UMainAnimInstance::Pivot()
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
                    }
                    else
                    {
                        PivotLeftFootLocation = PlayerSkeletalMesh->GetSocketLocation("foot_l");
                        PivotAnchorLocation = PlayerCapsuleComponent->GetComponentLocation();
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

void UMainAnimInstance::PivotStep()
{
    CanMove = false;
    PrevPoseKey = PoseKey;
    if (!ensure(PlayerCapsuleComponent)) { return; }
    if (HoopzCharacter->PivotKey == false) { // left foot moves
        switch (PoseKey) {
            case 0: // Left - Forward
                // Move Capsule to New Location
            {
                FVector NewLocation = PivotAnchorLocation + PlayerCapsuleComponent->GetForwardVector() * 30;
                PlayerCapsuleComponent->SetWorldLocation(NewLocation, false);
                break;
            }
            case 1: // Left - Forward Left
            {
                FVector NewLocation = PivotAnchorLocation + PlayerCapsuleComponent->GetForwardVector() * 20 + PlayerCapsuleComponent->GetRightVector() * -20;
                PlayerCapsuleComponent->SetWorldLocation(NewLocation, false);
                break;
            }
            case 2: // Left - Left
            {
                FVector NewLocation = PivotAnchorLocation + PlayerCapsuleComponent->GetRightVector() * -30;
                PlayerCapsuleComponent->SetWorldLocation(NewLocation, false);
                break;
            }
            case 3: // Left - Back Left
            {
                FVector NewLocation = PivotAnchorLocation + PlayerCapsuleComponent->GetForwardVector() * -20 + PlayerCapsuleComponent->GetRightVector() * -20;
                PlayerCapsuleComponent->SetWorldLocation(NewLocation, false);
                break;
            }
            case 4: // Left - Back
            {
                FVector NewLocation = PivotAnchorLocation + PlayerCapsuleComponent->GetForwardVector() * -30;
                PlayerCapsuleComponent->SetWorldLocation(NewLocation, false);
                break;
            }
            default: // Idle Pivot
                PlayerCapsuleComponent->SetWorldLocation(PivotAnchorLocation, false);
                return;   
        }
    }
    
    else { // right foot moves
        switch (PoseKey) {
            case 5: // Right - Back
            {
                FVector NewLocation = PivotAnchorLocation + PlayerCapsuleComponent->GetForwardVector() * -30;
                PlayerCapsuleComponent->SetWorldLocation(NewLocation, false);
                break;
            }
            case 6: // Right - Back Right
            {
                FVector NewLocation = PivotAnchorLocation + PlayerCapsuleComponent->GetForwardVector() * -20 + PlayerCapsuleComponent->GetRightVector() * 20;
                PlayerCapsuleComponent->SetWorldLocation(NewLocation, false);
                break;
            }
            case 7: // Right - Right
            {
                FVector NewLocation = PivotAnchorLocation + PlayerCapsuleComponent->GetRightVector() * 30;
                PlayerCapsuleComponent->SetWorldLocation(NewLocation, false);
                break;
            }
            case 8: // Right - Forward Right
            {
                FVector NewLocation = PivotAnchorLocation + PlayerCapsuleComponent->GetForwardVector() * 20 + PlayerCapsuleComponent->GetRightVector() * 20;
                PlayerCapsuleComponent->SetWorldLocation(NewLocation, false);
                break;
            }
            case 9: // Right - Forward
            {
                FVector NewLocation = PivotAnchorLocation + PlayerCapsuleComponent->GetForwardVector() * 30;
                PlayerCapsuleComponent->SetWorldLocation(NewLocation, false);
                break;
            }
            default: // Idle Pivot
                PlayerCapsuleComponent->SetWorldLocation(PivotAnchorLocation, false);
                return;
        }
    }
}

// TODO : Ragdoll IK
// TODO : Toe IK
FVector UMainAnimInstance::IKFootTrace(int32 Foot)
{
    if (!ensure(PlayerSkeletalMesh)) { return FVector(0, 0, 0); }
    if (!ensure(PlayerCapsuleComponent)) { return FVector(0, 0, 0); }

    FName FootName;
    if (Foot == 0) {
        FootName = FName(TEXT("foot_l"));
        LeftJointTargetLocation = PlayerSkeletalMesh->GetSocketLocation(FName(TEXT("joint_target_l")));

        // return Pivot Location if Pivot Mode
        if (FootPlanted == true && HoopzCharacter->PivotKey == true) { return PivotLeftFootLocation; }

        // TODO: Foot Placement - Return if foot is inactive
    } else { 
        FootName = FName(TEXT("foot_r"));
        RightJointTargetLocation = PlayerSkeletalMesh->GetSocketLocation(FName(TEXT("joint_target_r")));
        if (FootPlanted == true && HoopzCharacter->PivotKey == false) { return PivotRightFootLocation; } 
    } 

    FVector FootSocketLocation = PlayerSkeletalMesh->GetSocketLocation(FootName);
    float CapsuleHalfHeight = PlayerCapsuleComponent->GetUnscaledCapsuleHalfHeight();
    
    FVector StartTrace = FVector(FootSocketLocation.X, FootSocketLocation.Y, CapsuleHalfHeight);
    FVector EndTrace = FVector(FootSocketLocation.X, FootSocketLocation.Y, CapsuleHalfHeight - CapsuleHalfHeight - 5); // 15 = trace distance;

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
        FootSocketLocation.Z = (HitResult.Location - HitResult.TraceEnd).Size() - 5 + 13.47;

        // Use X & Y for Foot Rotation
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

