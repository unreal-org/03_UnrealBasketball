// Cache$ Copyrights

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SubAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class UNREALBASKETBALL_API USubAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	USubAnimInstance(const FObjectInitializer& ObjectInitializer);

	// Walking 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "JointAngles")
	FVector RightFootLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "JointAngles")
	FVector LeftFootLocation;

	// Foot Target Position Calculator - to be called by capsule with move();
	//void CalculateTargetFootPosition(FVector MoveDirection);


private:
	// Max Leg Reach between feet
	float MaxReach = 50;
	bool RightFootFree = true;

	float CapsuleHalfHeight;
	float CapsuleScale;

	bool Pivot = true;
	bool FootCanMove = false;
	bool PostUp = false;

	// Lerp Time
	float LerpTime;
	float LerpDuration = .3;

	// FName RightFoot = FName(TEXT("foot_r"));
	// FName LeftFoot = FName(TEXT("foot_l"));

protected:
	// Native initialization override point
	virtual void NativeInitializeAnimation() override;
	// Tick
	virtual void NativeUpdateAnimation(float DeltaTimeX) override;

	UPROPERTY(BluePrintReadOnly)
	USkeletalMeshComponent* PlayerSkeletalMesh = nullptr;

};
