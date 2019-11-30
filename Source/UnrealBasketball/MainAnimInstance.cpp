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
   
    // UE_LOG(LogTemp, Warning, TEXT("%i"), MainState->GetCurrentState())

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
    IKAlpha = 0.25;
}
void UMainAnimInstance::AnimNotify_SetBasketLocation()
{
    if (!ensure(HoopzCharacter)) { return; }
    HoopzCharacter->PivotMode = false;
    IKAlpha = 0.9;
}
void UMainAnimInstance::AnimNotify_SetPivot()
{ 
    if (!ensure(HoopzCharacter)) { return; }
    HoopzCharacter->PivotMode = true;
    IKAlpha = 0.85;
}
void UMainAnimInstance::AnimNotify_PivotToJumpTransition()
{
    PivotPoseIndex = 0;
    ShotPoseIndex = 0;
    IKAlpha = 0.25;
}

void UMainAnimInstance::IdleOffense(float DeltaTimeX)
{
    ThrowX = HoopzCharacter->ThrowY * 100;
    ThrowY = HoopzCharacter->ThrowX * 100;

    UE_LOG(LogTemp, Warning, TEXT("%f : %f"), ThrowX, ThrowY)

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

// TODO: Check for falling - loop falling animation - character Falling();
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

    // Blend Pose by Int
    if (CanMove == true) {
        // To disable concurrent turn & step

        if (CanMove == true) {
            PoseKey = HoopzCharacter->PivotInputKey;
            if (HoopzCharacter->EstablishPivot == true) {
                PivotStep();
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
    if (HoopzCharacter->PivotKey == false) { // left foot moves
        switch (PoseKey) {
            case 0: // 
                PivotPoseIndex = 17;
                break;
            case 1: //
                PivotPoseIndex = 18;
                break;
            case 2: //
                PivotPoseIndex = 19;
                break;
            case 3: //
                PivotPoseIndex = 20;
                break;
            case 4: //
                PivotPoseIndex = 21;
                break;
            default:
                PivotPoseIndex = 0;
                return;   
        }
    }
    
    else { // right foot moves
        switch (PoseKey) {
            case 5: // 
                PivotPoseIndex = 22;
                break;
            case 6: //
                PivotPoseIndex = 23;
                break;
            case 7: //
                PivotPoseIndex = 24;
                break;
            case 8: //
                PivotPoseIndex = 25;
                break;
            case 9: //
                PivotPoseIndex = 26;
                break;
            default:
                PivotPoseIndex = 0;
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
    
    bool HitConfirm = GetWorld()->LineTraceSingleByChannel(   // Line trace for feet
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

