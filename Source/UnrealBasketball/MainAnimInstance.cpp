// ChoiBoi Copyrights

#include "MainAnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Math/Rotator.h"
#include "Engine/World.h"	
#include "Kismet/KismetMathLibrary.h"
//#include "Kismet/GameplayStatics.h"

UMainAnimInstance::UMainAnimInstance(const FObjectInitializer &ObjectInitializer)
    : Super(ObjectInitializer)
{
    // Set capsule half height and scale here with parameter
    //Root = FName(TEXT("root"));
    Pelvis = FName(TEXT("pelvis_socket"));
    RightFoot = FName(TEXT("foot_target_r"));
    LeftFoot = FName(TEXT("foot_target_l"));
    RightJointTarget = FName(TEXT("joint_target_r"));
    LeftJointTarget = FName(TEXT("joint_target_l"));
    // RightHeel = FName(TEXT("heel_r"));
    // LeftHeel = FName(TEXT("heel_l"));

    TraceTag = FName(TEXT("TraceTag"));
}

void UMainAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();
    
    PlayerSkeletalMesh = GetSkelMeshComponent();
    
    if (!ensure(PlayerSkeletalMesh)) { return; }

    //GetWorld()->DebugDrawTraceTag = TraceTag;
    TraceParameters = FCollisionQueryParams(TraceTag, false);
    TraceParameters.AddIgnoredComponent(Cast<UPrimitiveComponent>(PlayerSkeletalMesh));
    TraceParameters.AddIgnoredActor(Cast<AActor>(PlayerSkeletalMesh->GetOwner()));
    
    //RootLocation = PlayerSkeletalMesh->GetBoneLocation(Root, EBoneSpaces::WorldSpace);      // world space
    RootLocation = PlayerSkeletalMesh->GetOwner()->GetRootComponent()->GetComponentLocation();
    RootLocation.Z = 0;

    PelvisRotation = PlayerSkeletalMesh->GetSocketRotation(Pelvis);
    PelvisTargetRotation = PelvisRotation;

    RightJointTargetLocation = RootLocation + PlayerSkeletalMesh->GetSocketLocation(RightJointTarget);
    LeftJointTargetLocation = RootLocation + PlayerSkeletalMesh->GetSocketLocation(LeftJointTarget);

    RightFootTargetLocation = RootLocation + PlayerSkeletalMesh->GetSocketLocation(RightFoot);
    LeftFootTargetLocation = RootLocation + PlayerSkeletalMesh->GetSocketLocation(LeftFoot);

    RightFootLocation = RightFootTargetLocation;
    LeftFootLocation = LeftFootTargetLocation;

    // UE_LOG(LogTemp, Warning, TEXT("RJT %s. LJT %s."), *RightFootTargetLocation.ToString(), *LeftFootTargetLocation.ToString())
    // UE_LOG(LogTemp, Warning, TEXT("RJT %s. LJT %s."), *RightFootTargetLocation.ToString(), *LeftFootTargetLocation.ToString())
    
    //PelvisRotation.Add(90, 90, 180);
    // PelvisRotation = PlayerSkeletalMesh->GetSocketRotation(Pelvis);
    // PelvisTargetRotation = PelvisRotation;

    
    //Root = PlayerSkeletalMesh->GetBoneName(0);
    
    // TargetRightFootLocation = PlayerSkeletalMesh->GetBoneLocation(RightFoot, EBoneSpaces::WorldSpace);
    // TargetLeftFootLocation = PlayerSkeletalMesh->GetBoneLocation(LeftFoot, EBoneSpaces::WorldSpace);
    //RootLocation = PlayerSkeletalMesh->GetSocketLocation(Root);
    
}

void UMainAnimInstance::NativeUpdateAnimation(float DeltaTimeX)
{
    Super::NativeUpdateAnimation(DeltaTimeX);

    if (PelvisRotation != PelvisTargetRotation) { TurnBody(DeltaTimeX); }
    if (RightFootLocation != RightFootTargetLocation) { SetRightFoot(); }
    if (LeftFootLocation != LeftFootTargetLocation) { SetLeftFoot(); }
}

///////////////////////// Target Foot Position Calculator /////////////////////////////
float UMainAnimInstance::IKFootTrace(FName Foot)
{
    if(!ensure(PlayerSkeletalMesh)) { return 0; }

    FVector FootSocketLocation = PlayerSkeletalMesh->GetSocketLocation(Foot);
    FVector RootLocation = PlayerSkeletalMesh->GetOwner()->GetRootComponent()->GetComponentLocation();
    
    FVector StartTrace = FVector(FootSocketLocation.X, FootSocketLocation.Y, RootLocation.Z);
    FVector EndTrace = FVector(FootSocketLocation.X, FootSocketLocation.Y, RootLocation.Z - 90 - 15); // - capsule half height - trace distance;

    FHitResult HitResult(ForceInit);
    
    if (!ensure(GetWorld())) { return 0; }
    bool HitConfirm = GetWorld()->LineTraceSingleByChannel(
            HitResult,
            StartTrace,
            EndTrace,
            ECollisionChannel::ECC_Visibility,
            TraceParameters);
    if (HitConfirm)
    {
        if (!ensure(HitResult.GetActor())) { return 0; }
        return (HitResult.Location - HitResult.TraceEnd).Size() - 15 + 13.47; // FootOffset
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

void UMainAnimInstance::TurnBody(float DeltaTimeX)   // TODO : Rotate Target Joints
{
    LerpTime = 0;

    if(LerpTime < LerpDuration)
    {
        LerpTime += DeltaTimeX;
        PelvisRotation = FMath::Lerp(PelvisRotation, PelvisTargetRotation, LerpTime / LerpDuration);
    }
    ZRotation = 0;
}

void UMainAnimInstance::SetRightFoot()
{
    // set target foot pos
    RightFootTargetLocation.Z = IKFootTrace(LeftFoot);
    RightFootLocation = RightFootTargetLocation;
}

void UMainAnimInstance::SetLeftFoot()
{
    // set target foot pos
    LeftFootTargetLocation.Z = IKFootTrace(LeftFoot);
    LeftFootLocation = LeftFootTargetLocation;
}