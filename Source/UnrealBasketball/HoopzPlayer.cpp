// Fill out your copyright notice in the Description page of Project Settings.


#include "HoopzPlayer.h"

// Sets default values
AHoopzPlayer::AHoopzPlayer()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Possess default player
	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

// Called when the game starts or when spawned
void AHoopzPlayer::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AHoopzPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AHoopzPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

