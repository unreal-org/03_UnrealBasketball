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
	FVector RightFootLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "JointAngles")
	FVector LeftFootLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "JointAngles")
	FVector RightFootSocketLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "JointAngles")
	FVector LeftFootSocketLocation;
	
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
	FName RightFoot;
	FName LeftFoot;
	FName Pelvis;
	//FName Root;

	// Lerp Time
	float LerpTime;
	float LerpDuration = .5;
	
	FRotator PelvisTargetRotation;
	FVector TargetRightFootLocation;
	FVector TargetLeftFootLocation;
	//FVector RootLocation;
	
protected:
	UPROPERTY(BluePrintReadOnly)
	USkeletalMeshComponent* PlayerSkeletalMesh = nullptr;

};
