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
	FVector RightFootTargetLocation;   // heel - world space
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "JointAngles")
	FVector LeftFootTargetLocation;    // heel - world space
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "JointAngles")
	FVector RightJointTargetLocation;   // world space
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "JointAngles")
	FVector LeftJointTargetLocation;    // world space
	
public:
	// Constructor
	UMainAnimInstance(const FObjectInitializer& ObjectInitializer);

	// Native initialization override point
	virtual void NativeInitializeAnimation() override;
	// Tick
	virtual void NativeUpdateAnimation(float DeltaTimeX) override;

	// Rotates Pelvis
	void SetZRotation(float ZThrow);
	void TurnBody(float DeltaTimeX);
	void SetFeet();
	
	// IK Foot Trace Offset
	float IKFootTrace(FName Foot);
	
private:
	FName Root;
	FName RightFoot;
	FName LeftFoot;
	FName Pelvis;
	FName RightJointTarget;
	FName LeftJointTarget;
	FName RightHeel;
	FName LeftHeel;

	// Lerp Time
	float LerpTime;
	float LerpDuration = .5;
	float ZRotation = 0;
	
	FRotator PelvisTargetRotation;
	FVector RootLocation;
	//FVector TargetRightFootLocation;
	//FVector TargetLeftFootLocation;
	//FVector RootLocation;
	
protected:
	UPROPERTY(BluePrintReadOnly)
	USkeletalMeshComponent* PlayerSkeletalMesh = nullptr;

};
