// Cache$ Copyrights


#include "SubAnimInstance.h"

USubAnimInstance::USubAnimInstance(const FObjectInitializer &ObjectInitializer)
    : Super(ObjectInitializer)
{
    // Set capsule half height and scale here with parameter
    
}

void USubAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    // PlayerSkeletalMesh = GetSkelMeshComponent();

    // if(!ensure(PlayerSkeletalMesh)) { return; }
    // PelvisRotation.Add(90, 90, 90);
    // TargetRotation = PelvisRotation;
    // CapsuleHalfHeight = 1.0f;
	// CapsuleScale = 1.0f;
    //if(!ensure(PlayerSkeletalMesh)) { return; }
}

void USubAnimInstance::NativeUpdateAnimation(float DeltaTimeX)
{
    Super::NativeUpdateAnimation(DeltaTimeX);

    
}

// void UMainAnimInstance::CalculateTargetFootPosition(FVector MoveDirection)
// {
//     // Pivot - Looking Straight - Get current rates
//     if (Pivot) // && Both feet planted?
//     {
//         if(!ensure(PlayerSkeletalMesh)) { return; }
//         FVector TargetFootLocation = MoveDirection * MaxReach + PlayerSkeletalMesh->GetComponentLocation();
//         // determine which foot is closer to direction
//         if ((TargetFootLocation - PlayerSkeletalMesh->GetSocketLocation(RightFoot)).Size() <= (TargetFootLocation - PlayerSkeletalMesh->GetSocketLocation(LeftFoot)).Size())
//         {
//             RightFootFree = true;
//             RightFootLocation = TargetFootLocation;
//             return;
//         }
//         else
//         {
//             RightFootFree = false;
//             LeftFootLocation = TargetFootLocation;
//             return;
//         }
        
//             // move foot to targetlocation - if within XY constraints - otherwise move max reach
//     }
// }