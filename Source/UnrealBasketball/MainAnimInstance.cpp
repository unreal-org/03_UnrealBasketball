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
    Root = FName(TEXT("root"));
    Pelvis = FName(TEXT("pelvis_socket"));
    RightFoot = FName(TEXT("foot_target_r"));
    LeftFoot = FName(TEXT("foot_target_l"));
    RightJointTarget = FName(TEXT("joint_target_r"));
    LeftJointTarget = FName(TEXT("joint_target_l"));
 
}

void UMainAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();
    
    PlayerSkeletalMesh = GetSkelMeshComponent();
    
    if (!ensure(PlayerSkeletalMesh)) { return; }
    
    //RootLocation = PlayerSkeletalMesh->GetBoneLocation(Root, EBoneSpaces::WorldSpace);      // world space
    RootLocation = PlayerSkeletalMesh->GetOwner()->GetRootComponent()->GetComponentLocation();
    RootLocation.Z = 0;

    PelvisRotation = PlayerSkeletalMesh->GetSocketRotation(Pelvis);
    PelvisTargetRotation = PelvisRotation;

    RightJointTargetLocation = RootLocation + PlayerSkeletalMesh->GetSocketLocation(RightJointTarget);
    LeftJointTargetLocation = RootLocation + PlayerSkeletalMesh->GetSocketLocation(LeftJointTarget);

    RightFootTargetLocation = RootLocation + PlayerSkeletalMesh->GetSocketLocation(RightFoot);
    LeftFootTargetLocation = RootLocation + PlayerSkeletalMesh->GetSocketLocation(LeftFoot);

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

    TurnBody(DeltaTimeX);
    SetFeet();
}

///////////////////////// Target Foot Position Calculator /////////////////////////////
float UMainAnimInstance::IKFootTrace(FName Foot)
{
    if(!ensure(PlayerSkeletalMesh)) { return 0; }

    // TODO: ADD VIRTUAL BONES
    FVector FootSocketLocation = PlayerSkeletalMesh->GetSocketLocation(Foot);
    FVector RootLocation = PlayerSkeletalMesh->GetOwner()->GetRootComponent()->GetComponentLocation();
    FootSocketLocation.Z = RootLocation.Z;
    RootLocation.X = FootSocketLocation.X;   
    RootLocation.Y = FootSocketLocation.Y;
    RootLocation.Z -= 86;

    FHitResult HitResult;
    FName TraceTag("TraceTag");
    GetWorld()->DebugDrawTraceTag = TraceTag;
    FCollisionQueryParams TraceParameters(TraceTag, false);
    TraceParameters.AddIgnoredComponent(Cast<UPrimitiveComponent>(PlayerSkeletalMesh));
    TraceParameters.AddIgnoredActor(Cast<AActor>(PlayerSkeletalMesh->GetOwner()));
    if (GetWorld()->LineTraceSingleByChannel(
            HitResult,
            FootSocketLocation,
            RootLocation,
            ECollisionChannel::ECC_WorldStatic,
            TraceParameters))
    {
        if (!ensure(HitResult.GetActor())) { return 0; }
        float FootOffset = abs(HitResult.GetActor()->GetActorLocation().Z + 13);   // TODO: change to CapsuleLocation
        //UE_LOG(LogTemp, Warning, TEXT("%s "),  *PlayerSkeletalMesh->GetSocketLocation(Foot).ToString())
        return FootOffset; // /CapsuleScale;
    }
    //UE_LOG(LogTemp, Warning, TEXT("nothing hit."))
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
    LerpTime = 0;

    if(LerpTime < LerpDuration)
    {
        LerpTime += DeltaTimeX;
        PelvisRotation = FMath::Lerp(PelvisRotation, PelvisTargetRotation, LerpTime / LerpDuration);
    }
    ZRotation = 0;
}

void UMainAnimInstance::SetFeet()
{
    // TargetRightFootLocation = FVector(0, 30, 0);  // z = 10 lays foot flat on ground
    // TargetLeftFootLocation = FVector(0, -30, 0);
    
    // TargetRightFootLocation.Z = IKFootTrace(RightFoot);
    // TargetLeftFootLocation.Z = IKFootTrace(LeftFoot);

    // RightFootLocation = TargetRightFootLocation;
    // LeftFootLocation = TargetLeftFootLocation;
    // UE_LOG(LogTemp, Warning, TEXT("Right %s. Left %s. Root "), *RightFootSocketLocation.ToString(), *LeftFootSocketLocation.ToString())//, *RootLocation.ToString())
    // UE_LOG(LogTemp, Warning, TEXT("Bone Name is %s and %s and "), *RightFoot.ToString(), *LeftFoot.ToString())//, *Root.ToString())
    
    UE_LOG(LogTemp, Warning, TEXT("Root %s."), *RootLocation.ToString())
    UE_LOG(LogTemp, Warning, TEXT("RFT %s. LFT %s."), *RightFootTargetLocation.ToString(), *LeftFootTargetLocation.ToString())
    UE_LOG(LogTemp, Warning, TEXT("RJT %s. LJT %s."), *RightJointTargetLocation.ToString(), *LeftJointTargetLocation.ToString())
}