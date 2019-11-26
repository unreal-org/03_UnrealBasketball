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
	// Sets default values for this character's properties
	AHoopzCharacter();
	AHoopzCharacter(const FObjectInitializer& ObjectInitializer);

	UFUNCTION()
	void MoveForward(float Throw);
	UFUNCTION()
	void MoveRight(float Throw);
	

	bool PivotMode = false;
	int32 PivotInputKey = -1;

	FVector BasketLocation;

	bool EstablishPivot = false;
	bool PivotKey = false;
	bool PivotDetached = true;

	int32 ShotKey = 0;
	bool CanChangeShot = true;

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
	UHoopzCharacterMovementComponent* HoopzCharacterMovementComponent = nullptr;
	USplineComponent* PivotComponent = nullptr;
	AStaticMeshActor* Basket = nullptr;
	UCameraComponent* Camera = nullptr;
	UCapsuleComponent* CapsuleComponent = nullptr;
	AActor* PivotPoint = nullptr;
	UMainAnimInstance* MainAnimInstance = nullptr;
	USpringArmComponent* SpringArm = nullptr;
	
	FAttachmentTransformRules AttachRules = FAttachmentTransformRules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, true);
	FDetachmentTransformRules DetachRules = FDetachmentTransformRules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, true);

	// Pivot Variables
	void Pivot();
	void SetPivot();
	bool PivotSet = false;
	bool CanTurn = true;
	FVector PivotForward;
	FVector PivotRight;
	void TurnLeft();
	void TurnRight();

	// Turn Timer & Lerp
	void OnTurnTimerExpire();
	float TurnDelay = 0.3;
	float TurnTime = 0;
	float TurnDuration = 0.3;
	FRotator TargetPlayerRotation;
	FRotator PlayerRotation;
	void TurnLerp(float DeltaTime);
	
	// Jump (x)
	void JumpPressed();
	void JumpReleased();
	float JumpHeldTime;
	bool Jumped = false;

	// Capsule
	bool CapsuleDip = false;
	void CapsuleDipper();
	float MaxCapsuleHalfHeight = 90;
	float MinCapsuleHalfHeight = 80;
	FVector CapsuleTarget;
	void CapsuleLerp(float DeltaTime);
	float CapsuleTurnTime = 0;
	float CapsuleTurnDuration = 0.2;
	FRotator CapsuleRotation;
	FRotator TargetCapsuleRotation;

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
};
