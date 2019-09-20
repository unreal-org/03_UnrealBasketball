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

    RightFoot = FName(TEXT("foot_r"));
    LeftFoot = FName(TEXT("foot_l"));
    Pelvis = FName(TEXT("pelvis"));
    //Root = PlayerSkeletalMesh->GetBoneName(0);
    
    // TargetRightFootLocation = PlayerSkeletalMesh->GetBoneLocation(RightFoot, EBoneSpaces::WorldSpace);
    // TargetLeftFootLocation = PlayerSkeletalMesh->GetBoneLocation(LeftFoot, EBoneSpaces::WorldSpace);
    //RootLocation = PlayerSkeletalMesh->GetSocketLocation(Root);
    
}

void UMainAnimInstance::NativeUpdateAnimation(float DeltaTimeX)
{
    Super::NativeUpdateAnimation(DeltaTimeX);

    TurnBody(DeltaTimeX);
    SetFeet();
}

///////////////////////// Target Foot Position Calculator /////////////////////////////
float UMainAnimInstance::IKFootTrace(FName Foot)
{
    if(!ensure(PlayerSkeletalMesh)) { return 0; }
    FVector SkeletalMeshLocation = PlayerSkeletalMesh->GetComponentLocation();
    FVector FootSocketLocation = PlayerSkeletalMesh->GetSocketLocation(Foot);
    FootSocketLocation.Z = SkeletalMeshLocation.Z;
    SkeletalMeshLocation = FootSocketLocation;
    SkeletalMeshLocation.Z -= CapsuleHalfHeight;

    FHitResult HitResult;
    FCollisionQueryParams TraceParameters(false);
    if (GetWorld()->LineTraceSingleByChannel(
            HitResult,
            FootSocketLocation,
            SkeletalMeshLocation,
            ECollisionChannel::ECC_WorldStatic,
            TraceParameters))
    {
        FVector FootOffset = SkeletalMeshLocation - HitResult.GetComponent()->GetComponentLocation();
        return FootOffset.Size(); // /CapsuleScale;
    }
    
    return 0;
}

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

void UMainAnimInstance::SetFeet()
{
    TargetRightFootLocation = FVector(0, 20, 10);
    TargetLeftFootLocation = FVector(0, -20, 10);
    
    TargetRightFootLocation.Z -= IKFootTrace(RightFoot);
    TargetLeftFootLocation.Z -= IKFootTrace(LeftFoot);

    RightFootLocation = TargetRightFootLocation;
    LeftFootLocation = TargetLeftFootLocation;
    // UE_LOG(LogTemp, Warning, TEXT("Right %s. Left %s. Root "), *RightFootSocketLocation.ToString(), *LeftFootSocketLocation.ToString())//, *RootLocation.ToString())
    // UE_LOG(LogTemp, Warning, TEXT("Bone Name is %s and %s and "), *RightFoot.ToString(), *LeftFoot.ToString())//, *Root.ToString())
    
}