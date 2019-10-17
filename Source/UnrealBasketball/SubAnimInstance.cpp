// Cache$ Copyrights


#include "SubAnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Math/Rotator.h"
#include "Engine/World.h"	
#include "Kismet/KismetMathLibrary.h"
#include "Misc/App.h"
//#include "Kismet/GameplayStatics.h"
#include "PlayerCapsuleComponent.h"
#include "GameFramework/Actor.h"

void FMainAnimInstanceProxy::Initialize(UAnimInstance* InAnimInstance)
{
    Super::Initialize(InAnimInstance);

    SubAnimInstance = dynamic_cast<USubAnimInstance*>(GetAnimInstanceObject());
    if (!ensure(SubAnimInstance)) { return; }
    PlayerSkeletalMesh = GetSkelMeshComponent();
    if (!ensure(PlayerSkeletalMesh)) { return; }
    PlayerCapsuleComponent = dynamic_cast<UPlayerCapsuleComponent*>(PlayerSkeletalMesh->GetOwner()->GetRootComponent());
    //PlayerCapsuleComponent = PlayerSkeletalMesh->GetOwner()->FindComponentByClass<UPlayerCapsuleComponent>();
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

    SubAnimInstance->PelvisLerpTo = PelvisTargetRotation;
    SubAnimInstance->RightJointTargetLocation = RightJointTargetPos;
    SubAnimInstance->LeftJointTargetLocation = LeftJointTargetPos;
    SubAnimInstance->RightFootInterpTo = RightFootTargetLocation;
    SubAnimInstance->LeftFootInterpTo = LeftFootTargetLocation;
}

// TODO : Account for foot placement on jumps
void FMainAnimInstanceProxy::Update(float DeltaSeconds)
{
    // Update internal variables
    if (!ensure(PlayerCapsuleComponent)) { return; }
    if (!ensure(PlayerSkeletalMesh)) { return; }
    
    SetZRotation();  // Root Rotation

    if (CanMove == false && RightFootFree == true) {
        FootMoveStart += DeltaSeconds;
        if (FootMoveStart > FootMoveEnd) {
            RightFootFree = false;
            LeftFootFree = true;
            CanMove = true;
        }
        RightFootTargetLocation += PlayerCapsuleComponent->GetComponentVelocity() * DeltaSeconds;
        
        return;
    }

    if (CanMove == false && LeftFootFree == true) {
        FootMoveStart += DeltaSeconds;
        if (FootMoveStart > FootMoveEnd) {
            LeftFootFree = false;
            RightFootFree = true;
            CanMove = true;
        }
        LeftFootTargetLocation += PlayerCapsuleComponent->GetComponentVelocity() * DeltaSeconds;
        
        return;
    }

    if (CanMove == true && PlayerCapsuleComponent->GetComponentVelocity().Size() > 1)
    {
        SetFootTargetLocation(PlayerCapsuleComponent->GetComponentVelocity());
        return;
    }
}

void FMainAnimInstanceProxy::PostUpdate(UAnimInstance* InAnimInstance) const
{
    Super::PostUpdate(InAnimInstance);

    if (!ensure(SubAnimInstance)) { return; }
    SubAnimInstance->PelvisLerpTo = PelvisTargetRotation;
    SubAnimInstance->RightJointTargetLocation = RightJointTargetPos;
    SubAnimInstance->LeftJointTargetLocation = LeftJointTargetPos;
    SubAnimInstance->RightFootInterpTo = RightFootTargetLocation;
    SubAnimInstance->LeftFootInterpTo = LeftFootTargetLocation;
}

// TODO : Pass Target Rotations by TArray to void 180 (back turned towards basket) angle
// TODO : Consider limiting too many turns in given time
// TODO : add Camera angles for these rotations
// TODO : Joint Targets
void FMainAnimInstanceProxy::SetZRotation()
{
    if (!ensure(PlayerCapsuleComponent)) { return; }
    PelvisTargetRotation.Yaw = PlayerCapsuleComponent->PelvisRotation.Yaw;
    //SetFootTargetLocation(FVector(1, 1, 1));
}

///////////////////////// Target Foot Position Calculator /////////////////////////////
// TODO : Base leg movement on capsule velocity
void FMainAnimInstanceProxy::SetFootTargetLocation(FVector AddToDirection)
{
    if (!ensure(PlayerSkeletalMesh)) { return; }
    if (!ensure(PlayerCapsuleComponent)) { return; }

    FVector Direction = AddToDirection.GetClampedToMaxSize2D(MaxReach);
    if (RightFootFree == true)
    {
        RightFootTargetLocation = PlayerSkeletalMesh->GetSocketLocation(RightFoot) + AddToDirection / 2;
        //RightFootTargetLocation += AddToDirection;
        RightFootTargetLocation.Z = IKFootTrace(RightFoot);
        RightFootOriginal = RightFootTargetLocation;
        RightJointTargetPos = PlayerSkeletalMesh->GetSocketLocation(RightJointTarget) + AddToDirection / 2;
        //RightJointTargetPos += AddToDirection;

        CanMove = false;
        FootMoveStart = 0;
        return;
    }

    if (LeftFootFree == true)
    {
        LeftFootTargetLocation = PlayerSkeletalMesh->GetSocketLocation(LeftFoot) + AddToDirection / 2;
        //LeftFootTargetLocation += AddToDirection;
        LeftFootTargetLocation.Z = IKFootTrace(LeftFoot);
        LeftFootOriginal = LeftFootTargetLocation;
        LeftJointTargetPos = PlayerSkeletalMesh->GetSocketLocation(LeftJointTarget) + AddToDirection / 2;
        //LeftJointTargetPos += AddToDirection;
        
        CanMove = false;
        FootMoveStart = 0;
        return;
    }

    if (RightFootFree == false && LeftFootFree == false)
    {
        FVector RootLocation = PlayerCapsuleComponent->GetComponentLocation() + AddToDirection;
        if ((RootLocation - PlayerSkeletalMesh->GetSocketLocation(RightFoot)).Size() <= (RootLocation - PlayerSkeletalMesh->GetSocketLocation(LeftFoot)).Size())
        {
            RightFootTargetLocation = PlayerSkeletalMesh->GetSocketLocation(RightFoot) + AddToDirection / 2;
            //RightFootTargetLocation += AddToDirection;
            RightFootTargetLocation.Z = IKFootTrace(RightFoot);
            RightFootOriginal = RightFootTargetLocation;
            RightJointTargetPos = PlayerSkeletalMesh->GetSocketLocation(RightJointTarget) + AddToDirection / 2;
            //RightJointTargetPos += AddToDirection;
            
            RightFootFree = true;
            CanMove = false;
            FootMoveStart = 0;
            return;
        }
        else
        {
            LeftFootTargetLocation = PlayerSkeletalMesh->GetSocketLocation(LeftFoot) + AddToDirection / 2;
            //LeftFootTargetLocation += AddToDirection;
            LeftFootTargetLocation.Z = IKFootTrace(LeftFoot);
            LeftFootOriginal = LeftFootTargetLocation;
            LeftJointTargetPos = PlayerSkeletalMesh->GetSocketLocation(LeftJointTarget) + AddToDirection / 2;
            //LeftJointTargetPos += AddToDirection;

            LeftFootFree = true;
            CanMove = false;
            FootMoveStart = 0;
            return;
        }  
    }

    return;    
}

void FMainAnimInstanceProxy::UpdateRightFootTargetLocation(float Angle)
{
    RightFootTargetLocation = RightFootOriginal + PlayerCapsuleComponent->GetComponentVelocity() / Angle;
}
void FMainAnimInstanceProxy::UpdateLeftFootTargetLocation(float Angle)
{
    RightFootTargetLocation = LeftFootOriginal + PlayerCapsuleComponent->GetComponentVelocity() / Angle;
}

float FMainAnimInstanceProxy::IKFootTrace(FName Foot)
{
    if(!ensure(PlayerSkeletalMesh)) { return 0; }
    if (!ensure(PlayerCapsuleComponent)) { return 0; }

    FVector FootSocketLocation = PlayerSkeletalMesh->GetSocketLocation(Foot);
    FVector RootLocation = PlayerCapsuleComponent->GetComponentLocation();
    
    FVector StartTrace = FVector(FootSocketLocation.X, FootSocketLocation.Y, FootSocketLocation.Z);
    FVector EndTrace = FVector(FootSocketLocation.X, FootSocketLocation.Y, FootSocketLocation.Z - 90 - 15); // - capsule half height - trace distance;

    FHitResult HitResult(ForceInit);
    
    if (!ensure(SubAnimInstance)) { return 0; }
    if (!ensure(SubAnimInstance->GetWorld())) { return 0; }
    bool HitConfirm = SubAnimInstance->GetWorld()->LineTraceSingleByChannel(
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

////////////////// Sub Anim Instance ////////////////////
// void USubAnimInstance::NativeInitializeAnimation()
// {
//     Super::NativeInitializeAnimation();

//     // PlayerSkeletalMesh = GetSkelMeshComponent();

//     // if(!ensure(PlayerSkeletalMesh)) { return; }
//     // PelvisRotation.Add(90, 90, 90);
//     // TargetRotation = PelvisRotation;
//     // CapsuleHalfHeight = 1.0f;
// 	// CapsuleScale = 1.0f;
//     //if(!ensure(PlayerSkeletalMesh)) { return; }
// }
USubAnimInstance::USubAnimInstance(const FObjectInitializer &ObjectInitializer)
    : Super(ObjectInitializer)
{
    // Set capsule half height and scale here with parameter
    
}

void USubAnimInstance::NativeUpdateAnimation(float DeltaTimeX)
{
    TurnBody(DeltaTimeX);
    SetRightFoot(DeltaTimeX);
    SetLeftFoot(DeltaTimeX);
}

// TODO : Rotate IKFootRoot
// TODO : Consider GetWorld()->GetTime() instead of tick
void USubAnimInstance::TurnBody(float DeltaTimeX)   
{
    PelvisLerpTime = 0;

    if(PelvisLerpTime < PelvisLerpDuration)
    {
        PelvisLerpTime += DeltaTimeX;
        PelvisRotation = FMath::Lerp(PelvisRotation, PelvisLerpTo, PelvisLerpTime / PelvisLerpDuration);
    }
}

void USubAnimInstance::SetRightFoot(float DeltaTimeX)
{
    if(RightFootLerpTime < RightFootLerpDuration)
    {
        RightFootLerpTime += DeltaTimeX;
        RightFootLocation = FMath::VInterpTo(RightFootLocation, RightFootInterpTo, DeltaTimeX, 7);
    }
    //RightFootLocation = RightFootInterpTo;
    // RightFootFree = false;
    // LeftFootFree = true;
    RightFootLerpTime = 0;
}

void USubAnimInstance::SetLeftFoot(float DeltaTimeX)
{
    if(LeftFootLerpTime < LeftFootLerpDuration)
    {
        LeftFootLerpTime += DeltaTimeX;
        LeftFootLocation = FMath::VInterpTo(LeftFootLocation, LeftFootInterpTo, DeltaTimeX, 7);
    }
    //LeftFootLocation = LeftFootInterpTo;
    // LeftFootFree = false;
    // RightFootFree = true;
    LeftFootLerpTime = 0;
}

