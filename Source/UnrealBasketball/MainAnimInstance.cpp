// ChoiBoi Copyrights

#include "MainAnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Math/Rotator.h"
#include "Engine/World.h"	
#include "Kismet/KismetMathLibrary.h"
#include "Misc/App.h"
//#include "Kismet/GameplayStatics.h"
#include "PlayerCapsuleComponent.h"
#include "GameFramework/Actor.h"

UMainAnimInstance::UMainAnimInstance(const FObjectInitializer &ObjectInitializer)
    : Super(ObjectInitializer)
{}

void FMainAnimInstanceProxy::Initialize(UAnimInstance* InAnimInstance)
{
    Super::Initialize(InAnimInstance);

    MainAnimInstance = dynamic_cast<UMainAnimInstance*>(GetAnimInstanceObject());
    if (!ensure(MainAnimInstance)) { return; }
    PlayerSkeletalMesh = GetSkelMeshComponent();
    if (!ensure(PlayerSkeletalMesh)) { return; }
    PlayerCapsuleComponent = dynamic_cast<UPlayerCapsuleComponent*>(PlayerSkeletalMesh->GetOwner()->GetRootComponent());
    if (!ensure(PlayerCapsuleComponent)) { return; }

    TraceParameters = FCollisionQueryParams(TraceTag, false);
    TraceParameters.AddIgnoredComponent(Cast<UPrimitiveComponent>(PlayerSkeletalMesh));
    TraceParameters.AddIgnoredActor(Cast<AActor>(PlayerSkeletalMesh->GetOwner()));

    FVector RootLocation = PlayerCapsuleComponent->GetComponentLocation();
    RootLocation.Z = 0;

    PelvisTargetRotation = PlayerSkeletalMesh->GetSocketRotation(Root);
    PlayerCapsuleComponent->PelvisRotation = PelvisTargetRotation;

    RightJointTargetPos = RootLocation + PlayerSkeletalMesh->GetSocketLocation(RightJointTarget);
    LeftJointTargetPos = RootLocation + PlayerSkeletalMesh->GetSocketLocation(LeftJointTarget);

    RightFootTargetLocation = RootLocation + PlayerSkeletalMesh->GetSocketLocation(RightFoot);
    LeftFootTargetLocation = RootLocation + PlayerSkeletalMesh->GetSocketLocation(LeftFoot);

    RightFootTargetLocation.Z = IKFootTrace(RightFoot);
    LeftFootTargetLocation.Z = IKFootTrace(LeftFoot);

    MainAnimInstance->PelvisLerpTo = PelvisTargetRotation;
    MainAnimInstance->RightJointTargetLocation = RightJointTargetPos;
    MainAnimInstance->LeftJointTargetLocation = LeftJointTargetPos;
    MainAnimInstance->RightFootInterpTo = RightFootTargetLocation;
    MainAnimInstance->LeftFootInterpTo = LeftFootTargetLocation;
}

void FMainAnimInstanceProxy::Update(float DeltaSeconds)
{
    // Update internal variables
    if (!ensure(PlayerCapsuleComponent)) { return; }
    if (!ensure(PlayerSkeletalMesh)) { return; }
    
    SetZRotation();  // Pelvis

    if (CanMove == false && RightFootFree == true && PlayerSkeletalMesh->GetSocketLocation("foot_r").Equals(RightFootTargetLocation, 1)) {
        RightFootFree = false;
        LeftFootFree = true;
        CanMove = true;
    }
    if (CanMove == false && LeftFootFree == true && PlayerSkeletalMesh->GetSocketLocation("foot_l").Equals(LeftFootTargetLocation, 1)) {
        LeftFootFree = false;
        RightFootFree = true;
        CanMove = true;
    }
    if (CanMove == true && PlayerCapsuleComponent->GetComponentVelocity().Size() > 1)
    {
        SetFootTargetLocation(PlayerCapsuleComponent->GetComponentVelocity());
    }
}

void FMainAnimInstanceProxy::PostUpdate(UAnimInstance* InAnimInstance) const
{
    Super::PostUpdate(InAnimInstance);

    if (!ensure(MainAnimInstance)) { return; }
    MainAnimInstance->PelvisLerpTo = PelvisTargetRotation;
    MainAnimInstance->RightJointTargetLocation = RightJointTargetPos;
    MainAnimInstance->LeftJointTargetLocation = LeftJointTargetPos;
    MainAnimInstance->RightFootInterpTo = RightFootTargetLocation;
    MainAnimInstance->LeftFootInterpTo = LeftFootTargetLocation;
}

///////////////////////// Target Foot Position Calculator /////////////////////////////
float FMainAnimInstanceProxy::IKFootTrace(FName Foot)
{
    if(!ensure(PlayerSkeletalMesh)) { return 0; }
    if (!ensure(PlayerCapsuleComponent)) { return 0; }

    FVector FootSocketLocation = PlayerSkeletalMesh->GetSocketLocation(Foot);
    FVector RootLocation = PlayerCapsuleComponent->GetComponentLocation();
    
    FVector StartTrace = FVector(FootSocketLocation.X, FootSocketLocation.Y, RootLocation.Z);
    FVector EndTrace = FVector(FootSocketLocation.X, FootSocketLocation.Y, RootLocation.Z - 90 - 15); // - capsule half height - trace distance;

    FHitResult HitResult(ForceInit);
    
    if (!ensure(MainAnimInstance)) { return 0; }
    if (!ensure(MainAnimInstance->GetWorld())) { return 0; }
    bool HitConfirm = MainAnimInstance->GetWorld()->LineTraceSingleByChannel(
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
void FMainAnimInstanceProxy::SetZRotation()
{
    if (!ensure(PlayerCapsuleComponent)) { return; }
    PelvisTargetRotation.Yaw = PlayerCapsuleComponent->PelvisRotation.Yaw;
}

// TODO : Base leg movement on capsule velocity
void FMainAnimInstanceProxy::SetFootTargetLocation(FVector AddToDirection)
{
    if (!ensure(PlayerSkeletalMesh)) { return; }
    if (!ensure(PlayerCapsuleComponent)) { return; }

    FVector Direction = AddToDirection.GetClampedToMaxSize2D(MaxReach);
    if (RightFootFree == true)
    {
        RightFootTargetLocation = PlayerSkeletalMesh->GetSocketLocation(RightFoot) + Direction;
        RightFootTargetLocation.Z = IKFootTrace(RightFoot);
        RightJointTargetPos = PlayerSkeletalMesh->GetSocketLocation(RightJointTarget);

        CanMove = false;
        return;
    }

    if (LeftFootFree == true)
    {
        LeftFootTargetLocation = PlayerSkeletalMesh->GetSocketLocation(LeftFoot) + Direction;
        LeftFootTargetLocation.Z = IKFootTrace(LeftFoot);
        LeftJointTargetPos = PlayerSkeletalMesh->GetSocketLocation(LeftJointTarget);
        
        CanMove = false;
        return;
    }

    if (RightFootFree == false && LeftFootFree == false)
    {
        FVector RootLocation = PlayerCapsuleComponent->GetComponentLocation() + AddToDirection;
        if ((RootLocation - PlayerSkeletalMesh->GetSocketLocation(RightFoot)).Size() <= (RootLocation - PlayerSkeletalMesh->GetSocketLocation(LeftFoot)).Size())
        {
            RightFootTargetLocation = PlayerSkeletalMesh->GetSocketLocation(RightFoot) + Direction;
            RightFootTargetLocation.Z = IKFootTrace(RightFoot);
            RightJointTargetPos = PlayerSkeletalMesh->GetSocketLocation(RightJointTarget);
            
            RightFootFree = true;
            CanMove = false;
            return;
        }
        else
        {
            LeftFootTargetLocation = PlayerSkeletalMesh->GetSocketLocation(LeftFoot) + Direction;
            LeftFootTargetLocation.Z = IKFootTrace(LeftFoot);
            LeftJointTargetPos = PlayerSkeletalMesh->GetSocketLocation(LeftJointTarget);

            LeftFootFree = true;
            CanMove = false;
            return;
        }  
    }

    return;    
}


////////////////// Main Anim Instance ////////////////////
void UMainAnimInstance::NativeUpdateAnimation(float DeltaTimeX)
{
    TurnBody(DeltaTimeX);
    SetRightFoot(DeltaTimeX);
    SetLeftFoot(DeltaTimeX);
}

// TODO : Rotate IKFootRoot
// TODO : Consider GetWorld()->GetTime() instead of tick
void UMainAnimInstance::TurnBody(float DeltaTimeX)   
{
    PelvisLerpTime = 0;

    if(PelvisLerpTime < PelvisLerpDuration)
    {
        PelvisLerpTime += DeltaTimeX;
        PelvisRotation = FMath::Lerp(PelvisRotation, PelvisLerpTo, PelvisLerpTime / PelvisLerpDuration);
    }
}

void UMainAnimInstance::SetRightFoot(float DeltaTimeX)
{
    if(RightFootLerpTime < RightFootLerpDuration)
    {
        RightFootLerpTime += DeltaTimeX;
        RightFootLocation = FMath::VInterpTo(RightFootLocation, RightFootInterpTo, DeltaTimeX, 8);
    }
    //RightFootLocation = RightFootInterpTo;
    // RightFootFree = false;
    // LeftFootFree = true;
    RightFootLerpTime = 0;
}

void UMainAnimInstance::SetLeftFoot(float DeltaTimeX)
{
    if(LeftFootLerpTime < LeftFootLerpDuration)
    {
        LeftFootLerpTime += DeltaTimeX;
        LeftFootLocation = FMath::VInterpTo(LeftFootLocation, LeftFootInterpTo, DeltaTimeX, 8);
    }
    //LeftFootLocation = LeftFootInterpTo;
    // LeftFootFree = false;
    // RightFootFree = true;
    LeftFootLerpTime = 0;
}