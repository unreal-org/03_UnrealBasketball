// ChoiBoi Copyrights

#include "MainAnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Math/Rotator.h"
#include "Engine/World.h"	
#include "Kismet/KismetMathLibrary.h"
#include "Misc/App.h"
//#include "Kismet/GameplayStatics.h"

UMainAnimInstance::UMainAnimInstance(const FObjectInitializer &ObjectInitializer)
    : Super(ObjectInitializer)
{
    // Set capsule half height and scale here with parameter
    Root = FName(TEXT("root_socket"));
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
    FVector RootLocation = PlayerSkeletalMesh->GetOwner()->GetRootComponent()->GetComponentLocation();
    RootLocation.Z = 0;

    PelvisRotation = PlayerSkeletalMesh->GetSocketRotation(Root);
    PelvisTargetRotation = PelvisRotation;

    RightJointTargetLocation = RootLocation + PlayerSkeletalMesh->GetSocketLocation(RightJointTarget);
    LeftJointTargetLocation = RootLocation + PlayerSkeletalMesh->GetSocketLocation(LeftJointTarget);

    RightFootTargetLocation = RootLocation + PlayerSkeletalMesh->GetSocketLocation(RightFoot);
    LeftFootTargetLocation = RootLocation + PlayerSkeletalMesh->GetSocketLocation(LeftFoot);

    //RightFootTargetLocation.Z = IKFootTrace(RightFoot);
    //LeftFootTargetLocation.Z = IKFootTrace(LeftFoot);
    SetRightFoot();
    SetLeftFoot();
}

void UMainAnimInstance::NativeUpdateAnimation(float DeltaTimeX)
{
    Super::NativeUpdateAnimation(DeltaTimeX);

    if (PelvisRotation != PelvisTargetRotation) { TurnBody(DeltaTimeX); }
    //if (RightFootLocation != RightFootTargetLocation) { SetRightFoot(); }  // may have to use socket location here
    //SetRightFoot();
    //if (LeftFootLocation != LeftFootTargetLocation) { SetLeftFoot(); }   // check to see how often this is being called
    //SetLeftFoot();
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
    PelvisTargetRotation.Yaw += 45 * ZThrow;
}

// TODO : Rotate IKFootRoot
// TODO : Consider GetWorld()->GetTime() instead of tick
void UMainAnimInstance::TurnBody(float DeltaTimeX)   
{
    LerpTime = 0;

    if(LerpTime < LerpDuration)
    {
        LerpTime += DeltaTimeX;
        PelvisRotation = FMath::Lerp(PelvisRotation, PelvisTargetRotation, LerpTime / LerpDuration);
    }
}

// TODO : Base leg movement on capsule velocity
void UMainAnimInstance::SetFootTargetLocation(FVector AddToDirection)
{
    if (!ensure(PlayerSkeletalMesh)) { return; }
    if (AddToDirection.Size() == 0) 
    { 
        // RightFootTargetLocation = PlayerSkeletalMesh->GetSocketLocation(RightFoot);
        // LeftFootTargetLocation = PlayerSkeletalMesh->GetSocketLocation(LeftFoot);

        return; 
    }
    
    //if (RightFootFree == false && LeftFootFree == false) { return; }

    // for sideways movement
    // if (AddToDirection orthogonal to IKFootRoot Forward Direction) then multiply by limited SideMaxReach
    // otherwise multiply by MaxReach
    AddToDirection *= MaxReach;
    FVector RootLocation = PlayerSkeletalMesh->GetOwner()->GetRootComponent()->GetComponentLocation() + AddToDirection;

    if (RightFootFree == true)
    {
        //FVector TargetLocation = RootLocation + FVector(AddToDirection.Y, -AddToDirection.X, 0).GetClampedToSize2D(0, 20);
        FVector TargetLocation = PlayerSkeletalMesh->GetSocketLocation(RightFoot) + AddToDirection;
        RightFootTargetLocation = TargetLocation;
        //RightFootFree = false;
        //LeftFootFree = true;
        SetRightFoot();
        return;
    }

    // if (LeftFootFree == true)
    // {
    //     //FVector TargetLocation = RootLocation + FVector(-AddToDirection.Y, AddToDirection.X, 0).GetClampedToSize2D(0, 20);
    //     FVector TargetLocation = PlayerSkeletalMesh->GetSocketLocation(LeftFoot) + AddToDirection;
    //     LeftFootTargetLocation = TargetLocation;
    //     //LeftFootFree = false;
    //     //RightFootFree = true;
    //     return;
    // }

    if (RightFootFree == false && LeftFootFree == false)
    {
        if ((RootLocation - RightFootLocation).Size() <= (RootLocation - LeftFootLocation).Size())   // TODO : Compare AddToDirection vector
        {
            //FVector TargetLocation = RootLocation + FVector(AddToDirection.Y, -AddToDirection.X, 0).GetClampedToSize2D(0, 20);
            FVector TargetLocation = PlayerSkeletalMesh->GetSocketLocation(RightFoot) + AddToDirection;
            RightFootTargetLocation = TargetLocation;
            //RightFootFree = false;
            //LeftFootFree = true;
            SetRightFoot();
            return;
        }
        // else
        // {
        //     //FVector TargetLocation = RootLocation + FVector(-AddToDirection.Y, AddToDirection.X, 0).GetClampedToSize2D(0, 20);
        //     FVector TargetLocation = PlayerSkeletalMesh->GetSocketLocation(LeftFoot) + AddToDirection;
        //     LeftFootTargetLocation = TargetLocation;
        //     //LeftFootFree = false;
        //     //RightFootFree = true;
        //     return;
        // }  
    }

    return;    
}

// TODO : Interpolate Speed
void UMainAnimInstance::SetRightFoot()
{
    RightJointTargetLocation = PlayerSkeletalMesh->GetSocketLocation(RightJointTarget);
    //RightFootTargetLocation = PlayerSkeletalMesh->GetSocketLocation(RightFoot);
    // set target foot pos
    RightFootTargetLocation.Z = IKFootTrace(RightFoot);
    RightFootLocation = RightFootTargetLocation;
    //RightFootFree = false;
}

void UMainAnimInstance::SetLeftFoot()
{
    LeftJointTargetLocation = PlayerSkeletalMesh->GetSocketLocation(LeftJointTarget);
    //LeftFootTargetLocation = PlayerSkeletalMesh->GetSocketLocation(LeftFoot);
    // set target foot pos
    LeftFootTargetLocation.Z = IKFootTrace(LeftFoot);
    LeftFootLocation = LeftFootTargetLocation;
    //LeftFootFree = false;
}