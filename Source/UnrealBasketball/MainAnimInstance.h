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

/**
 * To be used to play Animations
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
	FVector RightJointTargetLocation;   // world space
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "JointAngles")
	FVector LeftJointTargetLocation;    // world space
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "JointAngles")
	bool IK = false;

	
	// Transition Variables
	UPROPERTY(BlueprintReadOnly, Category= "Transition Variables")
	bool HasBall = true;
	UPROPERTY(BlueprintReadOnly, Category= "Transition Variables")
	bool Jumped = false;

	// Transition Events
	UFUNCTION(BlueprintCallable)
	void AnimNotify_ResetPrevMontageKey();

	UFUNCTION(BlueprintCallable)
	void AnimNotify_SetPivot();

	UFUNCTION(BlueprintCallable)
	void AnimNotify_SetBasketLocation();

	UFUNCTION(BlueprintCallable)
	void AnimNotify_PivotToJumpTransition();

	// Montage Reference
	UPROPERTY(BlueprintReadWrite, Category= "Montage Reference")
	UAnimMontage* CurrentMontage;

	// Pivot
	UPROPERTY(BlueprintReadOnly, Category= "Transition Variables")
	int32 PoseIndex = 0;

	// Basket
	UPROPERTY(BlueprintReadWrite, Category= "Static Mesh Actors")
	FVector BasketLocation;

	UPROPERTY(BlueprintReadOnly, Category= "Transition Variables")
	float IKAlpha = 0.85;

private:
	// State Machines
	FAnimNode_StateMachine *MainState;

	// State Machine Functions
	void Pivot(float DeltaTimeX);
	int32 PrevMontageKey = -1;   // default -1
	bool CanMove = true;

	// Pivot by Pose Blend
	float StepDelay = .3;
	int32 PoseKey = -1;
	void OnStepTimerExpire();
	void PivotStep();

	// Foot Trace
	FName TraceTag = FName(TEXT("TraceTag"));;
	FCollisionQueryParams TraceParameters;
	FVector IKFootTrace(int32 Foot);

	// Components
	UCapsuleComponent* PlayerCapsuleComponent = nullptr;
	USkeletalMeshComponent* PlayerSkeletalMesh = nullptr;
	AHoopzCharacter* HoopzCharacter = nullptr;

	// Shot Selection
	void ShotSelection(float DeltaTimeX);

protected:
	// Native initialization override point
	virtual void NativeInitializeAnimation() override;

	// Tick
	virtual void NativeUpdateAnimation(float DeltaTimeX) override;
};
