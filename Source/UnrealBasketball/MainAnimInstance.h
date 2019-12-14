// ChoiBoi Copyrights

#pragma once

#include "PivotAnimNotifyState.h"

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MainAnimInstance.generated.h"

class UAnimMontage;
class UCapsuleComponent;
class USkeletalMeshComponent;
class AHoopzCharacter;
class USplineComponent;

/**
 * To be used to play Animations
 * TODO : Apply Physics Constraints
 */
UCLASS( Transient, Blueprintable, hideCategories = AnimInstance, BlueprintType )
class UNREALBASKETBALL_API UMainAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	// Constructor
	UMainAnimInstance(const FObjectInitializer& ObjectInitializer);

	// Feet
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "JointAngles")
	FVector RightFootLocation;   // world space
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "JointAngles")
	FVector LeftFootLocation;    // world space
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "JointAngles")
	FRotator RightFootRotation;   // world space
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "JointAngles")
	FRotator LeftFootRotation;    // world space
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "JointAngles")
	FVector RightJointTargetLocation;   // world space
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "JointAngles")
	FVector LeftJointTargetLocation;    // world space
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "JointAngles")
	bool IK = false;

	
	// Transition Variables
	UPROPERTY(BlueprintReadOnly, Category= "Transition Variables")
	bool HasBall = false;
	UPROPERTY(BlueprintReadOnly, Category= "Transition Variables")
	bool Jumped = false;
	UPROPERTY(BlueprintReadOnly, Category= "Transition Variables")
	bool Dribble = false;

	// Transition Events
	UFUNCTION(BlueprintCallable)
	void AnimNotify_IdleOffense();
	UFUNCTION(BlueprintCallable)
	void AnimNotify_SetPivot();
	UFUNCTION(BlueprintCallable)
	void AnimNotify_IdleEntry();
	UFUNCTION(BlueprintCallable)
	void AnimNotify_IdleJump();
	UFUNCTION(BlueprintCallable)
	void AnimNotify_OnDribble();

	// Montage Reference
	UPROPERTY(BlueprintReadWrite, Category= "Montage Reference")
	UAnimMontage* CurrentMontage;

	// Pivot
	UPROPERTY(BlueprintReadOnly, Category= "Transition Variables")
	int32 PivotPoseIndex = 0;
	// Shot
	UPROPERTY(BlueprintReadOnly, Category= "Transition Variables")
	int32 ShotPoseIndex = 0;

	// Offense Transition & Variables
	UPROPERTY(BlueprintReadWrite, Category= "Static Mesh Actors")
	bool Offense = false;
	UPROPERTY(BlueprintReadWrite, Category= "Static Mesh Actors")
	float ThrowX;
	UPROPERTY(BlueprintReadWrite, Category= "Static Mesh Actors")
	float ThrowY;

	// Basket
	UPROPERTY(BlueprintReadWrite, Category= "Static Mesh Actors")
	FVector BasketLocation;

	UPROPERTY(BlueprintReadOnly, Category= "Transition Variables")
	float IKAlpha = 0.85;

	// Pivot
	bool FootPlanted = false;

private:
	// State Machines
	FAnimNode_StateMachine *MainState;

	// Idle
	void Idle(float DeltaTimeX);

	// State Machine Functions
	void Pivot(float DeltaTimeX);
	int32 PrevMontageKey = -1;   // default -1
	bool CanMove = true;

	// Pivot by Pose Blend
	float StepDelay = 0.3;    // TODO : Tweak to fix multiple input pivot turn
	int32 PoseKey = -1;
	int32 PrevPoseKey = -1;
	void OnStepTimerExpire();
	void PivotStep();
	FRotator CapsuleAnchorRotation;
	FVector PivotLeftFootAnchor;
	FVector PivotRightFootAnchor;
	float PivotInterpTime = 0;
	float PivotInterpDuration = .3;
	void PivotInterp(float DeltaTimeX);
	FVector NewCapsuleLocation;
	FRotator NewCapsuleRotation;
	FVector NewOffFootLocation;
	FVector NewLeftFootLocation;
	FVector NewRightFootLocation;
	FVector FirstStepLeftFootLocation;
	FVector FirstStepRightFootLocation;
	bool FirstStep = true;

	// Foot Trace
	FName TraceTag = FName(TEXT("TraceTag"));;
	FCollisionQueryParams TraceParameters;
	FVector IKFootTrace(int32 Foot, float DeltaTimeX);

	// Components
	UCapsuleComponent* PlayerCapsuleComponent = nullptr;
	USkeletalMeshComponent* PlayerSkeletalMesh = nullptr;
	AHoopzCharacter* HoopzCharacter = nullptr;

	// Shot Selection
	void WhileJumped(float DeltaTimeX);
	float CapsuleTurnTime = 0;
	float CapsuleTurnDuration = 0.15;

	// Idle Offense
	void IdleOffense(float DeltaTimeX);

	// Dribble
	void IdleDribble(float DeltaTimeX);

protected:
	// Native initialization override point
	virtual void NativeInitializeAnimation() override;

	// Tick
	virtual void NativeUpdateAnimation(float DeltaTimeX) override;
};
