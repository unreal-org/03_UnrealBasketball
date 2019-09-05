// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "HoopzPlayerMovementComponent.generated.h"

class UPlayerCapsuleComponent;  // For Capsule Component Input/Movement
class UInputComponent;

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
	///// Input Component Setup
	// void SetupInputComponent();

	// UInputComponent* InputComponent = nullptr;

	///// Capsule Component Setup
	UFUNCTION(BluePrintCallable, Category = "Setup")      
	void Initialize(UPlayerCapsuleComponent* PlayerCapsuleComponentToSet);
	
	UPlayerCapsuleComponent* PlayerCapsuleComponent = nullptr;
	
	///// Capsule Movement
	UFUNCTION(BluePrintCallable, Category = "Input")
	void IntendMoveForward(float Throw);
	UFUNCTION(BluePrintCallable, Category = "Input")
	void IntendMoveRight(float Throw);
	UFUNCTION(BluePrintCallable, Category = "Input")
	void IntendJumpCalled();
	UFUNCTION(BluePrintCallable, Category = "Input")
	void IntendJumpReleased();

	///// Timer
	float JumpCalledTime = 0;
	float JumpPressedTime = 0;

 	
};
