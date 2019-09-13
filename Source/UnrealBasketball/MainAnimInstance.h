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
	float BoneLength;
	FRotator BoneJoint;

};

/**
 * 
 */
UCLASS( transient, Blueprintable, hideCategories = AnimInstance, BlueprintType )
class UNREALBASKETBALL_API UMainAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	// Walking - Joints to Rotate - Left = 0, Right = 1
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "JointAngles")
	FRotator Pelvis;
	// Right Leg
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "JointAngles")
	FRotator RightThigh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "JointAngles")
	FRotator RightCalf;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "JointAngles")
	FRotator RightFoot;
	// Left Leg
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "JointAngles")
	FRotator LeftThigh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "JointAngles")
	FRotator LeftCalf;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "JointAngles")
	FRotator LeftFoot;

	// Max Leg Reach
	float MaxReach;
	bool RightFootFree = true;

	// Inverse Kinematics
	FVector ForwardKinematics(TArray<float> JointAngles);
	float DistanceFromTarget(FVector TargetFootPosition, TArray<float> JointAngles);
	float PartialGradient(FVector TargetFootPosition, TArray<float> JointAngles, int i);
	void InverseKinematics(FVector TargetFootPosition, TArray<float> JointAngles);

public:
	// Constructor
	UMainAnimInstance(const FObjectInitializer& ObjectInitializer);

	// Native initialization override point
	virtual void NativeInitializeAnimation() override;
	// Tick
	virtual void NativeUpdateAnimation(float DeltaTimeX) override;

private:
	USkeletalMeshComponent* PlayerSkeletalMesh = nullptr;

	// Left = 0, Right = 1
	// Consider having above Frotators as member variables when initializing
	FBone Pelvis;
	TArray<FBone> Thigh;
	TArray<FBone> Calf;
	TArray<FBone> Leg;

	// Foot Positions - To be updated every tick & locked while walking - Left = 0, Right = 1
	TArray<FVector> DefaultFootPosition;      // Feet will default back to this position
	TArray<FVector> TargetFootPosition;       // Calculate next foot position on move()

};
