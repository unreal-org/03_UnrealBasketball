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
    //PlayerCapsuleComponent = dynamic_cast<UPlayerCapsuleComponent*>(PlayerSkeletalMesh->GetOwner()->GetRootComponent());
    PlayerCapsuleComponent = PlayerSkeletalMesh->GetOwner()->GetRootComponent();
    if (!ensure(PlayerCapsuleComponent)) { return; }

    // MainAnimInstance = PlayerSkeletalMesh->GetAnimInstance();

    TraceParameters = FCollisionQueryParams(TraceTag, false);
    TraceParameters.AddIgnoredComponent(Cast<UPrimitiveComponent>(PlayerSkeletalMesh));
    TraceParameters.AddIgnoredActor(Cast<AActor>(PlayerSkeletalMesh->GetOwner()));

    FVector RootLocation = PlayerCapsuleComponent->GetComponentLocation();
    RootLocation.Z = 0;

    PelvisTargetRotation = PlayerSkeletalMesh->GetSocketRotation(Root);
    //PlayerCapsuleComponent->PelvisRotation = PelvisTargetRotation;
    // = PelvisRotation;

    RightJointTargetPos = RootLocation + PlayerSkeletalMesh->GetSocketLocation(RightJointTarget);
    LeftJointTargetPos = RootLocation + PlayerSkeletalMesh->GetSocketLocation(LeftJointTarget);

    RightFootTargetLocation = RootLocation + PlayerSkeletalMesh->GetSocketLocation(RightFoot);
    LeftFootTargetLocation = RootLocation + PlayerSkeletalMesh->GetSocketLocation(LeftFoot);

    RightFootTargetLocation.Z = IKFootTrace(RightFoot);
    //RightFootLocation = RightFootTargetLocation;
    LeftFootTargetLocation.Z = IKFootTrace(LeftFoot);
    //LeftFootLocation = LeftFootTargetLocation;

    if (!ensure(MainAnimInstance)) { return; }
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
    //PelvisRotation.Yaw += 1.0f * DeltaSeconds;
    
    //SetZRotation();
    
    if (PlayerCapsuleComponent->GetComponentVelocity().Size() > 0)
    {
        if (RightFootFree == true) { 
            SetFootTargetLocation(PlayerCapsuleComponent->GetComponentVelocity());
            //SetRightFoot(DeltaSeconds);
            return;
        }
        if (LeftFootFree == true) { 
            SetFootTargetLocation(PlayerCapsuleComponent->GetComponentVelocity());
            //SetLeftFoot(DeltaSeconds);
            return;
        }
        else if (RightFootFree == false && LeftFootFree == false) { 
            SetFootTargetLocation(PlayerCapsuleComponent->GetComponentVelocity());
            //if (RightFootLocation != RightFootTargetLocation) { SetRightFoot(DeltaSeconds); }
            //if (LeftFootLocation != LeftFootTargetLocation) { SetLeftFoot(DeltaSeconds); }
            return;
        }
    }

    
    //UE_LOG(LogTemp, Warning, TEXT("%s"), *PlayerCapsuleComponent->GetComponentVelocity().ToString())
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
    //PelvisTargetRotation.Yaw += 45 * ZThrow;
    if (!ensure(PlayerCapsuleComponent)) { return; }
    //PelvisTargetRotation.Yaw = PlayerCapsuleComponent->PelvisRotation.Yaw;
    //UE_LOG(LogTemp, Warning, TEXT("%f"), PelvisTargetRotation.Yaw)
}

// TODO : Base leg movement on capsule velocity
void FMainAnimInstanceProxy::SetFootTargetLocation(FVector AddToDirection)
{
    if (!ensure(PlayerSkeletalMesh)) { return; }
    if (!ensure(PlayerCapsuleComponent)) { return; }
    // if (AddToDirection.Size() == 0) 
    // { 
    //     // RightFootTargetLocation = PlayerSkeletalMesh->GetSocketLocation(RightFoot);
    //     // LeftFootTargetLocation = PlayerSkeletalMesh->GetSocketLocation(LeftFoot);

    //     return; 
    // }
    
    //if (RightFootFree == false && LeftFootFree == false) { return; }

    // for sideways movement
    // if (AddToDirection orthogonal to IKFootRoot Forward Direction) then multiply by limited SideMaxReach
    // otherwise multiply by MaxReach
    //AddToDirection *= MaxReach;
    FVector RootLocation = PlayerCapsuleComponent->GetComponentLocation() + AddToDirection;


    if (RightFootFree == true)
    {
        //FVector TargetLocation = RootLocation + FVector(AddToDirection.Y, -AddToDirection.X, 0).GetClampedToSize2D(0, 20);
        RightFootTargetLocation = PlayerSkeletalMesh->GetSocketLocation(RightFoot) + AddToDirection;
        RightFootTargetLocation.Z = IKFootTrace(RightFoot);
        RightJointTargetPos = PlayerSkeletalMesh->GetSocketLocation(RightJointTarget);
        //RightFootTargetLocation = TargetLocation;
        //RightFootFree = false;
        //LeftFootFree = true;
        // SetRightFoot();
        //UE_LOG(LogTemp, Warning, TEXT("%s"), *RightFootTargetLocation.ToString())
        return;
    }

    if (LeftFootFree == true)
    {
        //FVector TargetLocation = RootLocation + FVector(-AddToDirection.Y, AddToDirection.X, 0).GetClampedToSize2D(0, 20);
        LeftFootTargetLocation = PlayerSkeletalMesh->GetSocketLocation(LeftFoot) + AddToDirection;
        LeftFootTargetLocation.Z = IKFootTrace(LeftFoot);
        LeftJointTargetPos = PlayerSkeletalMesh->GetSocketLocation(LeftJointTarget);
        //LeftFootTargetLocation = TargetLocation;
        //LeftFootFree = false;
        //RightFootFree = true;
        //UE_LOG(LogTemp, Warning, TEXT("%s"), *LeftFootTargetLocation.ToString())
        return;
    }

    if (RightFootFree == false && LeftFootFree == false)
    {
        if ((RootLocation - PlayerSkeletalMesh->GetSocketLocation(RightFoot)).Size() <= (RootLocation - PlayerSkeletalMesh->GetSocketLocation(LeftFoot)).Size())   // TODO : Compare AddToDirection vector
        {
            //FVector TargetLocation = RootLocation + FVector(AddToDirection.Y, -AddToDirection.X, 0).GetClampedToSize2D(0, 20);
            RightFootTargetLocation = PlayerSkeletalMesh->GetSocketLocation(RightFoot) + AddToDirection;
            RightFootTargetLocation.Z = IKFootTrace(RightFoot);
            RightJointTargetPos = PlayerSkeletalMesh->GetSocketLocation(RightJointTarget);
            //RightFootTargetLocation = TargetLocation;
            //RightFootFree = false;
            //LeftFootFree = true;
            //SetRightFoot();
            //UE_LOG(LogTemp, Warning, TEXT("%s"), *RightFootTargetLocation.ToString())
            return;
        }
        else
        {
            //FVector TargetLocation = RootLocation + FVector(-AddToDirection.Y, AddToDirection.X, 0).GetClampedToSize2D(0, 20);
            LeftFootTargetLocation = PlayerSkeletalMesh->GetSocketLocation(LeftFoot) + AddToDirection;
            LeftFootTargetLocation.Z = IKFootTrace(LeftFoot);
            LeftJointTargetPos = PlayerSkeletalMesh->GetSocketLocation(LeftJointTarget);
            //LeftFootTargetLocation = TargetLocation;
            //LeftFootFree = false;
            //RightFootFree = true;
            //UE_LOG(LogTemp, Warning, TEXT("%s"), *LeftFootTargetLocation.ToString())
            return;
        }  
    }

    return;    
}


////////////////// Main Anim Instance ////////////////////
void UMainAnimInstance::NativeUpdateAnimation(float DeltaTimeX)
{
    //if (PelvisRotation != PelvisLerpTo) { TurnBody(DeltaTimeX); }
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
    FootLerpTime = 0;

    if(FootLerpTime < FootLerpDuration)
    {
        FootLerpTime += DeltaTimeX;
        RightFootLocation = FMath::VInterpTo(RightFootLocation, RightFootInterpTo, DeltaTimeX, FootLerpTime / FootLerpDuration);
    }
    // RightFootLocation = RightFootInterpTo;
    // RightFootFree = false;
    // LeftFootFree = true;
}

void UMainAnimInstance::SetLeftFoot(float DeltaTimeX)
{
    FootLerpTime = 0;

    if(FootLerpTime < FootLerpDuration)
    {
        FootLerpTime += DeltaTimeX;
        LeftFootLocation = FMath::VInterpTo(LeftFootLocation, LeftFootInterpTo, DeltaTimeX, FootLerpTime / FootLerpDuration);
    }

    // LeftFootFree = false;
    // RightFootFree = true;
}