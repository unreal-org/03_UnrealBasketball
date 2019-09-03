// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "HoopzPlayerMovementComponent.generated.h"

class UCapsuleComponent;  // For Capsule Component Input/Movement

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
	// Input setup
	UFUNCTION(BluePrintCallable, Category = "Setup")      
	void Initialize(UCapsuleComponent* CapsuleComponentToSet);

	// Capsule Movement
	UFUNCTION(BluePrintCallable, Category = "Input")
	void MoveForward(float Throw);
	UFUNCTION(BluePrintCallable, Category = "Input")
	void MoveRight(float Throw);

	// Max force for Capsule Movement in newtons
	UPROPERTY(EditDefaultsOnly)
	float MaxMovementForce = 35;

private:
	// Components to set
	UCapsuleComponent* CapsuleComponent = nullptr;

	float CurrentThrow = 0;

};
