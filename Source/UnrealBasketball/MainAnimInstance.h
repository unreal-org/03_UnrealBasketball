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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "JointAngles")
	FRotator Spine3Rotation;

public:
	// Constructor
	UMainAnimInstance(const FObjectInitializer& ObjectInitializer);

	// Native initialization override point
	virtual void NativeInitializeAnimation() override;
	// Tick
	virtual void NativeUpdateAnimation(float DeltaTimeX) override;

	// IK Foot Trace Offset
	// float IKFootTrace(FName Foot, float CapsuleLocationZ);

	// Rotates Pelvis
	void SetZRotation(float ZThrow);
	void TurnBody(float DeltaTimeX);

private:
	//FName Pelvis = FName(TEXT("pelvis"));

	// Lerp Time
	float LerpTime;
	float LerpDuration = .5;
	float ZRotation = 0;

	// Rotation Target
	FRotator TargetRotation;

	
protected:
	UPROPERTY(BluePrintReadOnly)
	USkeletalMeshComponent* PlayerSkeletalMesh = nullptr;

};
