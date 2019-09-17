// ChoiBoi Copyrights

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MainAnimInstance.generated.h"

class USkeletalMeshComponent;

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
	FVector RightFootLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "JointAngles")
	FVector LeftFootLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "JointAngles")
	FRotator PelvisRotation;

	// Max Leg Reach between feet
	float MaxReach = 50;
	bool RightFootFree = true;

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

	float CapsuleHalfHeight;
	float CapsuleScale;

	bool Pivot = true;
	bool FootCanMove = false;
	bool PostUp = false;

	FName RightFoot = FName(TEXT("foot_r"));
	FName LeftFoot = FName(TEXT("foot_l"));
	FName Pelvis = FName(TEXT("pelvis"));

	// Lerp Time
	float LerpTime;
	float LerpDuration = .5;
	float ZRotation = 0;

	// Rotation Target
	FRotator TargetRotation;

	// Foot Target Position Calculator - to be called by capsule with move();
	void CalculateTargetFootPosition(FVector MoveDirection);

protected:
	UPROPERTY(BluePrintReadOnly)
	USkeletalMeshComponent* PlayerSkeletalMesh = nullptr;

};
