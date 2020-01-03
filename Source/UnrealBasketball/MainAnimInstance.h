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

	// Limb Scale
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "JointAngles")
	FVector ArmScale;   // component space
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "JointAngles")
	FVector LegScale;   // component space

	// Motion Frequency - Controls range of Locomotion(Connected to Limb & Spine Base Points)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "JointAngles")
	FVector MotionFrequency;   // world space

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "JointAngles")
	FVector TestTarget;   // world space

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

	// Locomotion
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "JointAngles")
	FRotator PelvisMotion;   // bone space
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "JointAngles")
	FVector PelvisHeight;   // bone space
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "JointAngles")
	FRotator Spine2Motion;   // bone space

	
	// Transition Variables
	UPROPERTY(BlueprintReadOnly, Category= "Transition Variables")
	bool HasBall = false;
	UPROPERTY(BlueprintReadOnly, Category= "Transition Variables")
	bool Jumped = false;
	UPROPERTY(BlueprintReadOnly, Category= "Transition Variables")
	bool Dribble = false;
	UPROPERTY(BlueprintReadOnly, Category= "Transition Variables")
	bool PostUp = false;

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
	UFUNCTION(BlueprintCallable)
	void AnimNotify_PostUpEntry();

	// Montage Reference
	UPROPERTY(BlueprintReadWrite, Category= "Montage Reference")
	UAnimMontage* CurrentMontage;

	// Pivot
	UPROPERTY(BlueprintReadOnly, Category= "Transition Variables")
	int32 PivotPoseIndex = 0;

	// Shot
	UPROPERTY(BlueprintReadOnly, Category= "Transition Variables")
	int32 ShotPoseIndex = 0;

	// Post
	UPROPERTY(BlueprintReadOnly, Category= "Transition Variables")
	int32 PostPoseIndex = -1;

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

	// Foot IK Alpha
	UPROPERTY(BlueprintReadOnly, Category= "Transition Variables")
	float IKAlpha = 0.85;
	UPROPERTY(BlueprintReadOnly, Category= "Transition Variables")
	float LeftIKAlpha = 0.85;
	UPROPERTY(BlueprintReadOnly, Category= "Transition Variables")
	float RightIKAlpha = 0.85;

	// Pivot
	bool FootPlanted = false;

private:
	// State Machines
	FAnimNode_StateMachine *MainState;

	// Helper Functions
	void CapsuleTargetLerp(float DeltaTimeX, float TurnDuration);
	void Locomotion(float DeltaTimeX);

	// Idle
	void Idle(float DeltaTimeX);
	bool FootKey = false;
	bool InitialPose = true;
	bool InitialStep = true;
	bool PointSet1 = false;
	bool PointSet2 = false;
	bool PointSet3 = false;
	bool PointSet4 = false;
	float MaxReach = 20;
	float MaxStepOffset = 30;
	float StartTime;
	float MotionTime;
	float MotionWave;
	float LeftInterpSpeed = 50;
	float RightInterpSpeed = 50;
	FVector LeftLegInterpTo;
	FVector RightLegInterpTo;
	FVector LeftLegTarget;
	FVector RightLegTarget;

	// PostUp
	void IdlePostUp(float DeltaTimeX);
	float PostUpSpeedMod = 1;   // Multiplied
	float PostUpInterpMod = 0;  // Added
	int32 PostUpReverse = 1;

	// State Machine Functions
	void Pivot(float DeltaTimeX);
	int32 PrevMontageKey = -1;   // default -1
	bool CanMove = true;

	// Pivot variables
	float StepDelay = 0.2;    // TODO : Tweak to fix multiple input pivot turn
	int32 PoseKey = -1;
	int32 PrevPoseKey = -1;  
	void OnStepTimerExpire();
	void PivotStep();
	FRotator CapsuleAnchorRotation;
	FVector PivotLeftFootAnchor;
	FVector PivotRightFootAnchor;
	float PivotInterpTime = 0;
	float PivotInterpDuration = .2;
	void PivotInterp(float DeltaTimeX);
	FVector NewCapsuleLocation;
	FRotator NewCapsuleRotation;
	FVector NewOffFootLocation;
	FVector NewLeftFootLocation;
	FVector NewRightFootLocation;
	FVector FirstStepLeftFootLocation;
	FVector FirstStepRightFootLocation;
	bool FirstStep = true;

	// Jump
	bool LeftFootJump = false;
	bool RightFootJump = false;

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
	float CapsuleTurnDuration = 0.3;

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
