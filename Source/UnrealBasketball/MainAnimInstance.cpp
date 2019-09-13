// ChoiBoi Copyrights


#include "MainAnimInstance.h"
#include "Components/SceneComponent.h"

UMainAnimInstance::UMainAnimInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UMainAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    PlayerSkeletalMesh = GetSkelMeshComponent();
}

void UMainAnimInstance::NativeUpdateAnimation(float DeltaTimeX)
{
    Super::NativeUpdateAnimation(DeltaTimeX);


}

// Inverse Kinematics
FVector UMainAnimInstance::ForwardKinematics(TArray<float> JointAngles)
{
    if (RightFootFree == true) {
        int R = 0;
        int L = 1;
    } else {
        int R = 1;
        int L = 0;
    }
    
    //FVector StartingPoint = CurrentFootPosition.[R];


    return FVector();
}

float UMainAnimInstance::DistanceFromTarget(FVector TargetFootPosition, TArray<float> JointAngles)
{
    
    return 0.0f;
}

float UMainAnimInstance::PartialGradient(FVector TargetFootPosition, TArray<float> JointAngles, int i)
{
    return 0.0f;
}

void UMainAnimInstance::InverseKinematics(FVector TargetFootPosition, TArray<float> JointAngles)
{

}