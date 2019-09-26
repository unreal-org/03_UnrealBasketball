// ChoiBoi Copyrights

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MainAnimInstance.generated.h"

class USkeletalMeshComponent;

/**
 * Controls Upper Body and Passes Feet variables to adjust legs in SubAnimInstance
 */
UCLASS( transient, Blueprintable, hideCategories = AnimInstance, BlueprintType )
class UNREALBASKETBALL_API UMainAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	// Body Angle
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "JointAngles")
	FRotator PelvisRotation;

	// Feet
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "JointAngles")
	FVector RightFootLocation;   // world space
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "JointAngles")
	FVector LeftFootLocation;    // world space
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "JointAngles")
	FVector RightJointTargetLocation;   // world space
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "JointAngles")
	FVector LeftJointTargetLocation;    // world space
	
public:
	// Constructor
	UMainAnimInstance(const FObjectInitializer& ObjectInitializer);

	// Movement
	void SetZRotation(float ZThrow);
	void SetFootTargetLocation(FVector AddToDirection);
	
private:
	FName Root;
	FName RightFoot;
	FName LeftFoot;
	FName Pelvis;
	FName RightJointTarget;
	FName LeftJointTarget;
	// FName RightHeel;
	// FName LeftHeel;

	// Lerp Time
	float LerpTime;
	float LerpDuration = .5;
	
	FRotator PelvisTargetRotation;
	FVector RightFootTargetLocation;
	FVector LeftFootTargetLocation;

	// Foot Trace
	FName TraceTag;
	FCollisionQueryParams TraceParameters;
	float IKFootTrace(FName Foot);
	
	void TurnBody(float DeltaTimeX);
	void SetRightFoot();
	void SetLeftFoot();

	float MaxReach = 10;
	bool RightFootFree = true;
	bool LeftFootFree = false;
	
protected:
	// Native initialization override point
	virtual void NativeInitializeAnimation() override;

	// Tick
	virtual void NativeUpdateAnimation(float DeltaTimeX) override;

	UPROPERTY(BluePrintReadOnly)
	USkeletalMeshComponent* PlayerSkeletalMesh = nullptr;

};
