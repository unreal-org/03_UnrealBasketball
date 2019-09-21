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
    FVector FootSocketLocation = PlayerSkeletalMesh->GetSocketLocation(Foot);
    //FVector RootLocation = PlayerSkeletalMesh->GetSocketLocation("root");  // Trace from Foot straight down to Capsule location Z
    FVector RootLocation = PlayerSkeletalMesh->GetOwner()->GetRootComponent()->GetComponentLocation();
    RootLocation.X = FootSocketLocation.X;   
    RootLocation.Y = FootSocketLocation.Y;
    RootLocation.Z -= PlayerSkeletalMesh->GetOwner()->GetRootComponent()->CapsuleHalfHeight;

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
        float FootOffset = abs(PlayerSkeletalMesh->GetSocketLocation(Foot).Z - HitResult.GetActor()->GetActorLocation().Z);   // TODO: change to CapsuleLocation
        UE_LOG(LogTemp, Warning, TEXT("%s hit at %s and %f offset."),  *HitResult.GetActor()->GetName(), *HitResult.GetActor()->GetActorLocation().ToString(), FootOffset)
        return FootOffset; // /CapsuleScale;
    }
    UE_LOG(LogTemp, Warning, TEXT("nothing hit."))
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
    TargetRightFootLocation = FVector(0, 20, 0);  // z = 10 lays foot flat on ground
    TargetLeftFootLocation = FVector(0, -20, 0);
    
    TargetRightFootLocation.Z += IKFootTrace(RightFoot);
    TargetLeftFootLocation.Z += IKFootTrace(LeftFoot);

    RightFootLocation = TargetRightFootLocation;
    LeftFootLocation = TargetLeftFootLocation;
    // UE_LOG(LogTemp, Warning, TEXT("Right %s. Left %s. Root "), *RightFootSocketLocation.ToString(), *LeftFootSocketLocation.ToString())//, *RootLocation.ToString())
    // UE_LOG(LogTemp, Warning, TEXT("Bone Name is %s and %s and "), *RightFoot.ToString(), *LeftFoot.ToString())//, *Root.ToString())
    
}