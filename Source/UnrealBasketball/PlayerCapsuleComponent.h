// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "MainAnimInstance.h"

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "PlayerCapsuleComponent.generated.h"

class USceneComponent;
class USplineComponent;

/**
 * 
 */
UCLASS( meta=(BlueprintSpawnableComponent) )
class UNREALBASKETBALL_API UPlayerCapsuleComponent : public UCapsuleComponent
{
	GENERATED_BODY()
	
public:
	// Constructor to set default capsule values
	UPlayerCapsuleComponent();

	// Movement
	UFUNCTION(BluePrintCallable, Category = "Input")
	void SetMoveForwardRate(float Rate);
	UFUNCTION(BluePrintCallable, Category = "Input")
	void SetMoveRightRate(float Rate);
	UFUNCTION(BluePrintCallable, Category = "Input")
	void SetJumpRate(float Rate);
	
	void Turn(float ZRotation);

	// Properties
	UPROPERTY(EditDefaultsOnly)
	float MaxMoveForce = 800;
	
	int32 PivotInputKey = -1;   // default -1 to signal Player hasn't moved
	bool EstablishPivotFoot = false;

	// TODO : Contstruct and Set Sub Anim Instance Capsule Half Height * Scale.Z Here

private:
	// To Apply Force only when in contact with Court
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

	// Movement and Movement Throw
	void Move();
	void Jump();

	float CurrentForwardRate = 0;
	float CurrentRightRate = 0;
	float CurrentJumpRate = 0;

	bool Pivot = true;
	bool PivotSet = false;
	
	USceneComponent* PivotComponent = nullptr;
	USplineComponent* CapsulePivotBig = nullptr;

protected:
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


};
