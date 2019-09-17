// ChoiBoi Copyrights

#include "MainAnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Math/Rotator.h"
#include "Engine/World.h"

UMainAnimInstance::UMainAnimInstance(const FObjectInitializer &ObjectInitializer)
    : Super(ObjectInitializer)
{
    // Set capsule half height and scale here with parameter
    
}

void UMainAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    PlayerSkeletalMesh = GetSkelMeshComponent();

    if(!ensure(PlayerSkeletalMesh)) { return; }
    PelvisRotation.Add(90, 90, 90);
    TargetRotation = PelvisRotation;
    // CapsuleHalfHeight = 1.0f;
	// CapsuleScale = 1.0f;
    //if(!ensure(PlayerSkeletalMesh)) { return; }
}

void UMainAnimInstance::NativeUpdateAnimation(float DeltaTimeX)
{
    Super::NativeUpdateAnimation(DeltaTimeX);

    TurnBody(DeltaTimeX);
}

///////////////////////// Target Foot Position Calculator /////////////////////////////
// float UMainAnimInstance::IKFootTrace(FName Foot, float CapsuleLocationZ)
// {
//     if(!ensure(PlayerSkeletalMesh)) { return; }
//     FVector FootLocation = PlayerSkeletalMesh->GetSocketLocation(Foot);
//     FootLocation.Z = CapsuleLocationZ;

//     CapsuleLocationZ -= CapsuleHalfHeight;
//     FVector CapsuleLocation = FVector(FootLocation.X, FootLocation.Y, CapsuleLocationZ);

//     FHitResult HitResult;
//     FCollisionQueryParams TraceParameters(false);
//     if (GetWorld()->LineTraceSingleByChannel(
//             HitResult,
//             FootLocation,
//             CapsuleLocation,
//             TraceParameters))
//     {
//         FVector FootOffset = CapsuleLocation - HitResult.GetComponent().GetComponentLocation();
//         return FootOffset.Size() / CapsuleScale;
//     }

//     return 0; // No Hit = No Offset
// }

void UMainAnimInstance::CalculateTargetFootPosition(FVector MoveDirection)
{
    // Pivot - Looking Straight - Get current rates
    if (Pivot) // && Both feet planted?
    {
        if(!ensure(PlayerSkeletalMesh)) { return; }
        FVector TargetFootLocation = MoveDirection * MaxReach + PlayerSkeletalMesh->GetComponentLocation();
        // determine which foot is closer to direction
        if ((TargetFootLocation - PlayerSkeletalMesh->GetSocketLocation(RightFoot)).Size() <= (TargetFootLocation - PlayerSkeletalMesh->GetSocketLocation(LeftFoot)).Size())
        {
            RightFootFree = true;
            RightFootLocation = TargetFootLocation;
            return;
        }
        else
        {
            RightFootFree = false;
            LeftFootLocation = TargetFootLocation;
            return;
        }
        
            // move foot to targetlocation - if within XY constraints - otherwise move max reach
    }
}

void UMainAnimInstance::SetZRotation(float ZThrow)
{
    ZRotation = ZThrow;
    TargetRotation.Yaw += 45 * ZRotation;
    ZRotation = 0;
}

void UMainAnimInstance::TurnBody(float DeltaTimeX)
{
    if(!ensure(PlayerSkeletalMesh)) { return; }
    
    // Rotate body by 45 degrees - unless post-up = true
    LerpTime = 0;

    if(LerpTime < LerpDuration)
    {
        LerpTime += DeltaTimeX;
        PelvisRotation = FMath::Lerp(PelvisRotation, TargetRotation, LerpTime / LerpDuration);
    }
}