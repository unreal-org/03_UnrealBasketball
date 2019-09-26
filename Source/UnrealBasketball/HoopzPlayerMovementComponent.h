// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "HoopzPlayerMovementComponent.generated.h"

class UPlayerCapsuleComponent;  // For Capsule Component Input/Movement
class USceneComponent;          // For Pivot

/**
 * Controls Hoopz Player Movement:
 * 		1. Movement
 * 		2. Jumping
 * 		3. 
 * Note: Capsule is set to simulate physics but locked in X & Y Axes.
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UNREALBASKETBALL_API UHoopzPlayerMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()
	
public:
	///// Capsule Component Setup
	UFUNCTION(BluePrintCallable, Category = "Setup")      
	void Initialize(UPlayerCapsuleComponent* PlayerCapsuleComponentToSet);
	
	///// Capsule Movement
	UFUNCTION(BluePrintCallable, Category = "Input")
	void IntendMoveForward(float Throw);
	UFUNCTION(BluePrintCallable, Category = "Input")
	void IntendMoveRight(float Throw);
	UFUNCTION(BluePrintCallable, Category = "Input")
	void IntendJumpCalled();
	UFUNCTION(BluePrintCallable, Category = "Input")
	void IntendJumpReleased();

	UFUNCTION(BluePrintCallable, Category = "Input")
	void TurnLeft();
	UFUNCTION(BluePrintCallable, Category = "Input")
	void TurnRight();

private:
	UPlayerCapsuleComponent* PlayerCapsuleComponent = nullptr;
	USceneComponent * UpdatedComponent = nullptr;
	
	///// Timer
	float JumpCalledTime = 0;
	float JumpPressedTime = 0;

	/// Capsule Pivot
	void Pivot(float DeltaTime);
	FVector PivotTranslation;
	FRotator RotationRate = FRotator(0, 0, 45);
	
protected:
	virtual void SetUpdatedComponent(USceneComponent* NewUpdatedComponent) override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
 	
};
