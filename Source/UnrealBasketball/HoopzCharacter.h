// Cache$ Copyrights

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HoopzCharacter.generated.h"

class UHoopzCharacterMovementComponent;
class USplineComponent;
class AStaticMeshActor;
class UCameraComponent;
class UCapsuleComponent;
class USceneComponent;
class UMainAnimInstance;
class USpringArmComponent;

struct FAttachmentTransformRules;
struct FDetachmentTransformRules;

UCLASS()
class UNREALBASKETBALL_API AHoopzCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Constructors
	AHoopzCharacter();
	AHoopzCharacter(const FObjectInitializer& ObjectInitializer);

	// Variables needed for MainAnimInstance
	int32 PivotInputKey = -1;
	bool PivotMode = false;
	bool EstablishPivot = false;
	bool PivotKey = false;
	bool PivotAttached = true;
	bool CanTurn = true;
	bool PivotTurn = false;
	bool PivotTurnLeft = false;
	bool PivotTurnRight = false;
	FRotator PivotPointRotation;

	FVector BasketLocation;

	int32 ShotKey = 0;
	bool ShotSelect = false;
	bool CanChangeShot = true;

	FVector ForwardThrow;
	FVector RightThrow;

	// Modify Capsule Half Height
	void SetCapsuleHalfHeight(float MaxValue, float MinValue);

	// Pivot Point Reference
	AActor* PivotPoint = nullptr;
	USplineComponent* CapsulePivotPoints = nullptr;
	USplineComponent* FootPivotPoints = nullptr;

	int32 CurrentState;

	// Rotation of Player
	FRotator TotalRotation;
	bool LocomotionTurn = false;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// On Jumped
	virtual void OnJumped_Implementation() override;

	// On Landed
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;

private:	
	// Character Components
	UHoopzCharacterMovementComponent* HoopzCharacterMovementComponent = nullptr;
	USplineComponent* PivotComponent = nullptr;
	AStaticMeshActor* Basket = nullptr;
	UCameraComponent* Camera = nullptr;
	UCapsuleComponent* CapsuleComponent = nullptr;
	UMainAnimInstance* MainAnimInstance = nullptr;
	USpringArmComponent* SpringArm = nullptr;

	// Movement
	void MoveForward(float Throw);
	void MoveRight(float Throw);

	// Pivot Variables
	void Pivot();
	void SetPivot();
	FVector PivotForward;
	FVector PivotRight;
	void TurnLeft();
	void TurnRight();

	// PostUp
	void PostLeft();
	void PostRight();

	// Turn
	void OnTurnTimerExpire();
	float TurnDelay = 0.2;
	
	// Jump (x)
	void JumpPressed();
	void JumpReleased();
	float JumpHeldTime;
	bool Jumped = false;

	// Dribble (R2) - *** NOTE : There is an option for Right Trigger to be an Axis ***
	void DribbleRight();
	void DribbleLeft();

	// Dash
	void OnDashTimerExpire();
	bool CanDash = true;

	// Capsule
	bool CapsuleDip = false;
	void CapsuleDipper();
	float MaxCapsuleHalfHeight = 90;
	float MinCapsuleHalfHeight = 80;

	// Face Button
	void DashOrShot();
	void LayUp();
	void Dunk();
	void Floater();

	// Spring Arm
	FVector SpringArmTarget;
	void SpringArmLerp(float DeltaTime);
	float SpringArmTurnTime = 0;
	float SpringArmTurnDuration = 0.2;
	FRotator SpringArmRotation;
	FRotator TargetSpringArmRotation;

	// Testing
	void TogglePivot();
	void ToggleOffense();
};
