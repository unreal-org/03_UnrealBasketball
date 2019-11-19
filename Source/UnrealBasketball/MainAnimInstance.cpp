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
   
    switch (MainState->GetCurrentState())
    {
        case 0: // Idle
            break;
        case 1: // IdlePivot
            Pivot(DeltaTimeX);
            break;
        case 4: // Jump (Ball)
            ShotSelection(DeltaTimeX);
        default:
            return;
    }

    LeftFootLocation = IKFootTrace(0);
    RightFootLocation = IKFootTrace(1);

    if (ensure(HoopzCharacter)) { BasketLocation = HoopzCharacter->BasketLocation; }

    // float Temp = (PlayerSkeletalMesh->GetSocketLocation(FName(TEXT("foot_l"))) + PlayerSkeletalMesh->GetSocketLocation(FName(TEXT("root")))).Size();
    // UE_LOG(LogTemp, Warning, TEXT("%f"), Temp)   // *** 27.64 ***

}

void UMainAnimInstance::AnimNotify_SetBasketLocation() { if (ensure(HoopzCharacter)) BasketLocation = HoopzCharacter->BasketLocation; }
void UMainAnimInstance::AnimNotify_ResetPrevMontageKey() {  }
void UMainAnimInstance::AnimNotify_SetPivot()
{ 
    if (!ensure(HoopzCharacter)) { return; }
    HoopzCharacter->PivotMode = true;
    // set movementmode
}
void UMainAnimInstance::AnimNotify_PivotToJumpTransition()
{
    PoseIndex = 0;
    // adjust IKFoot blend alpha
}

void UMainAnimInstance::OnStepTimerExpire()
{
    CanMove = true;
}

// TODO : Play idlepivot animation if idle for more than 5 seconds?
void UMainAnimInstance::Pivot(float DeltaTimeX)
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
                PoseIndex = 0;
                return;   
        }
    }
    
    else { // right foot moves
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
                PoseIndex = 0;
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

void UMainAnimInstance::ShotSelection(float DeltaTimeX)
{
    //UE_LOG(LogTemp, Warning, TEXT("%i"), MainState->GetCurrentState())
}