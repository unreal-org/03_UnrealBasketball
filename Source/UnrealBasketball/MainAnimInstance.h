// ChoiBoi Copyrights

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MainAnimInstance.generated.h"

class UAnimMontage;
class UPlayerCapsuleComponent;

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
	
	// Transition Variables
	UPROPERTY(BlueprintReadOnly, Category= "Transition Variables")
	bool HasBall = false;

	// Transition Events
	UFUNCTION(BlueprintCallable)
	void AnimNotify_ChangeStateInfo();

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

	// Capsule Component
	UPlayerCapsuleComponent* PlayerCapsuleComponent = nullptr;

protected:
	// Native initialization override point
	virtual void NativeInitializeAnimation() override;

	// Tick
	virtual void NativeUpdateAnimation(float DeltaTimeX) override;
};
