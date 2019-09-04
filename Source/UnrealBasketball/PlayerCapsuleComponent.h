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
	UFUNCTION(BluePrintCallable, Category = "Input")
	void SetMoveForwardRate(float Rate);
	UFUNCTION(BluePrintCallable, Category = "Input")
	void SetMoveRightRate(float Rate);

	UPROPERTY(EditDefaultsOnly)
	float MaxMoveForce = 35000;  

private:
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

	void Move();
	//void MoveRight();

	float CurrentForwardRate = 0;
	float CurrentRightRate = 0;

protected:
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


};
