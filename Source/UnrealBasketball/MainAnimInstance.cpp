// ChoiBoi Copyrights

#include "MainAnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Math/Rotator.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"

UMainAnimInstance::UMainAnimInstance(const FObjectInitializer &ObjectInitializer)
    : Super(ObjectInitializer)
{
    // Set capsule half height and scale here with parameter
    
}

void UMainAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    PlayerSkeletalMesh = GetSkelMeshComponent();
    PelvisRotation.Add(90, 90, 180);
    PelvisTargetRotation = PelvisRotation;
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



// TODO : Pass Target Rotations by TArray to void 180 (back turned towards basket) angle
// TODO : Consider limiting too many turns in given time
// TODO : add Camera angles for these rotations
void UMainAnimInstance::SetZRotation(float ZThrow)
{
    PelvisTargetRotation.Roll += 45 * ZThrow;
}

void UMainAnimInstance::TurnBody(float DeltaTimeX)
{
    if(!ensure(PlayerSkeletalMesh)) { return; }

    LerpTime = 0;

    if(LerpTime < LerpDuration)
    {
        LerpTime += DeltaTimeX;
        PelvisRotation = FMath::Lerp(PelvisRotation, PelvisTargetRotation, LerpTime / LerpDuration);
    }
}