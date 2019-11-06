// ChoiBoi Copyrights

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MainAnimInstance.generated.h"

class UAnimMontage;
class UPlayerCapsuleComponent;
class USkeletalMeshComponent;

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

	// Transition Events
	UFUNCTION(BlueprintCallable)
	void AnimNotify_ResetPrevMontageKey();

	// Montage Reference
	UPROPERTY(BlueprintReadWrite, Category= "Montage Reference")
	UAnimMontage* CurrentMontage;

private:
	// State Machines
	FAnimNode_StateMachine *MainState;
	FName CurrentStateName;
	int32 CurrentStateIndex;

	// State Machine Functions
	void Pivot();
	int32 MontageKey;
	int32 PrevMontageKey = -1;
	bool PivotKey; // false = left, true = right;
	bool EstablishPivotFoot;
	bool CanMove = true;
	FName CurrentPivotPos;

	// Foot Trace
	FName TraceTag = FName(TEXT("TraceTag"));;
	FCollisionQueryParams TraceParameters;
	FVector IKFootTrace(bool PivotKey);

	// Capsule Component
	UPlayerCapsuleComponent* PlayerCapsuleComponent = nullptr;
	USkeletalMeshComponent* PlayerSkeletalMesh = nullptr;

protected:
	// Native initialization override point
	virtual void NativeInitializeAnimation() override;

	// Tick
	virtual void NativeUpdateAnimation(float DeltaTimeX) override;
};
