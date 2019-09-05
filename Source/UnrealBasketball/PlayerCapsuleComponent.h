// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "PlayerCapsuleComponent.generated.h"

/**
 * 
 */
UCLASS( meta=(BlueprintSpawnableComponent) )
class UNREALBASKETBALL_API UPlayerCapsuleComponent : public UCapsuleComponent
{
	GENERATED_BODY()
	
public:
	// Movement
	UFUNCTION(BluePrintCallable, Category = "Input")
	void SetMoveForwardRate(float Rate);
	UFUNCTION(BluePrintCallable, Category = "Input")
	void SetMoveRightRate(float Rate);
	UFUNCTION(BluePrintCallable, Category = "Input")
	void SetJumpRate(float Rate);

	UPROPERTY(EditDefaultsOnly)
	float MaxMoveForce = 1500;  

private:
	// Apply Force only when in contact with Court
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

	// Movement and Movement Throw
	void Move();
	void Jump();

	float CurrentForwardRate = 0;
	float CurrentRightRate = 0;
	float CurrentJumpRate = 0;

protected:
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


};
