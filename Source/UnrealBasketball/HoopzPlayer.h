// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "HoopzPlayer.generated.h"

class UPlayerCapsuleComponent;   // Root Component
class UHoopzPlayerMovementComponent;

UCLASS()
class UNREALBASKETBALL_API AHoopzPlayer : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AHoopzPlayer();

private:
	// TODO: Player Stats

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
