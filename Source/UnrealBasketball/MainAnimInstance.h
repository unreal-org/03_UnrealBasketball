// ChoiBoi Copyrights

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MainAnimInstance.generated.h"

class USkeletalMeshComponent;

USTRUCT()
struct FBone
{
	GENERATED_USTRUCT_BODY()
	
	FName BoneName;
	FVector BoneLength = FVector(0, 0, 0);

	FBone(FName Name)
	{
		BoneName = Name;
	}
};

/**
 * 
 */
UCLASS( transient, Blueprintable, hideCategories = AnimInstance, BlueprintType )
class UNREALBASKETBALL_API UMainAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	// Walking 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "JointAngles")
	FRotator Pelvis;
	// Joints to Rotate - Left = 0, Right = 1
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "JointAngles")
	TArray<FRotator> Thigh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "JointAngles")
	TArray<FRotator> Calf;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "JointAngles")
	TArray<FRotator> Foot;

	// Max Leg Reach
	float MaxReach;
	bool RightFootFree = true;

	// Inverse Kinematics
	FVector ForwardKinematics();
	float DistanceFromTarget(FVector TargetFootPosition);
	float PartialGradient(FVector TargetFootPosition);
	void InverseKinematics(FVector TargetFootPosition);

public:
	// Constructor
	UMainAnimInstance(const FObjectInitializer& ObjectInitializer);

	// Native initialization override point
	virtual void NativeInitializeAnimation() override;
	// Tick
	virtual void NativeUpdateAnimation(float DeltaTimeX) override;

private:
	USkeletalMeshComponent* PlayerSkeletalMesh = nullptr;

	FVector GetBoneLength(FName Bone);

	// Left = 0, Right = 1
	FBone FPelvis;
	TArray<FBone> FThigh;
	TArray<FBone> FCalf;
	TArray<FBone> FFoot;

	// Foot Positions - To be updated every tick & locked while walking - Left = 0, Right = 1
	TArray<FVector> DefaultFootPosition;      // Feet will default back to this position
	TArray<FVector> TargetFootPosition;       // Calculate next foot position on move()

};
