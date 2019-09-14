// ChoiBoi Copyrights


#include "MainAnimInstance.h"
#include "Components/SceneComponent.h"
#include "Math/Rotator.h"

UMainAnimInstance::UMainAnimInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UMainAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    PlayerSkeletalMesh = GetSkelMeshComponent();
    if(!ensure(PlayerSkeletalMesh)) { return; }

    FPelvis = FBone(FName(TEXT("pelvis")), Pelvis);
    Pelvis = PlayerSkeletalMesh->GetSocketRotation(FPelvis.BoneName);

    FThigh.Add(FBone(FName(TEXT("thigh_l")));
    FThigh.Add(FBone(FName(TEXT("thigh_r")));
    FThigh.[0].BoneLength = GetBoneLength(FThigh.[0].BoneName);
    FThigh.[1].BoneLength = GetBoneLength(FThigh.[1].BoneName);
    Thigh.Add(PlayerSkeletalMesh->GetSocketRotation(FThigh.[0].BoneName);
    Thigh.Add(PlayerSkeletalMesh->GetSocketRotation(FThigh.[1].BoneName);

    FCalf.Add(FBone(FName(TEXT("calf_l")));
    FCalf.Add(FBone(FName(TEXT("calf_r")));
    FCalf.[0].BoneLength = GetBoneLength(FCalf.[0].BoneName);
    FCalf.[1].BoneLength = GetBoneLength(FCalf.[1].BoneName);
    Calf.Add(PlayerSkeletalMesh->GetSocketRotation(FCalf.[0].BoneName);
    Calf.Add(PlayerSkeletalMesh->GetSocketRotation(FCalf.[1].BoneName);

    FFoot.Add(FBone(FName(TEXT("foot_l")));
    FFoot.Add(FBone(FName(TEXT("foot_r")));
    FFoot.[0].BoneLength = GetBoneLength(FFoot.[0].BoneName);
    FFoot.[1].BoneLength = GetBoneLength(FFoot.[1].BoneName);
    Foot.Add(PlayerSkeletalMesh->GetSocketRotation(FFoot.[0].BoneName);
    Foot.Add(PlayerSkeletalMesh->GetSocketRotation(FFoot.[1].BoneName);
}

void UMainAnimInstance::NativeUpdateAnimation(float DeltaTimeX)
{
    Super::NativeUpdateAnimation(DeltaTimeX);

    
}

FVector GetBoneLength(FName Bone)
{
    FName ParentBone = GetParentBone(Bone);
	return PlayerSkeletalMesh->GetSocketLocation(ParentBone) - PlayerSkeletalMesh->GetSocketLocation(Bone);
}

///////////////////////// Inverse Kinematics /////////////////////////////
FVector UMainAnimInstance::ForwardKinematics()
{
    if (RightFootFree == true) {
        int L = 0;
        int R = 1;
    } else {
        int L = 1;
        int R = 0;
    }
    // Foot BoneLength is Calf Length... etc...  *** Consider Making one Array of Bone ***
    FVector PreviousPoint = PlayerSkeletalMesh->GetSocketLocation(FFoot.[L].BoneName);
    FVector NextPoint = PreviousPoint + Foot.[L].Quaternion() * FFoot.[L].BoneLength;

    PreviousPoint = NextPoint + Calf.[L].Quaternion() * FCalf.[L].BoneLength;
    NextPoint = PreviousPoint + Thigh.[L].Quaternion() * FThigh.[L].BoneLength;
    PreviousPoint = NextPoint + Pelvis.[L].Quaternion() * FThigh.[R].BoneLength;
    NextPoint = PreviousPoint + Thigh.[R].Quaternion() * FCalf.[R].BoneLength;
    PreviousPoint = NextPoint + Calf.[R].Quaternion() * FFoot.[R].BoneLength;

    return PreviousPoint;
}

float UMainAnimInstance::DistanceFromTarget(FVector TargetFootPosition)
{
    
    return 0.0f;
}

float UMainAnimInstance::PartialGradient(FVector TargetFootPosition)
{
    return 0.0f;
}

void UMainAnimInstance::InverseKinematics(FVector TargetFootPosition)
{

}