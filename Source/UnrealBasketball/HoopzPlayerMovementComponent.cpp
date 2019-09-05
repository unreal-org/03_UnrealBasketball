// Fill out your copyright notice in the Description page of Project Settings.


#include "HoopzPlayerMovementComponent.h"
//#include "Components/CapsuleComponent.h"
#include "Math/UnrealMathUtility.h"
#include "PlayerCapsuleComponent.h"
#include "Engine/World.h"

// void UHoopzPlayerMovementComponent::SetupInputComponent()
// {
//     InputComponent = 
// }

void UHoopzPlayerMovementComponent::Initialize(UPlayerCapsuleComponent* PlayerCapsuleComponentToSet)
{
    PlayerCapsuleComponent = PlayerCapsuleComponentToSet;
}

void UHoopzPlayerMovementComponent::IntendMoveForward(float Throw)
{
    if (!ensure(PlayerCapsuleComponent)) { return; }
    PlayerCapsuleComponent->SetMoveForwardRate(Throw);
}

void UHoopzPlayerMovementComponent::IntendMoveRight(float Throw)
{
    if (!ensure(PlayerCapsuleComponent)) { return; }
    PlayerCapsuleComponent->SetMoveRightRate(Throw);
}

void UHoopzPlayerMovementComponent::IntendJumpCalled()
{
    JumpCalledTime = GetWorld()->GetTimeSeconds();
    //JumpPressedTime = 
    // float Throw = 0.0f;
    // PlayerCapsuleComponent->SetJumpRate(Throw);
}

void UHoopzPlayerMovementComponent::IntendJumpReleased()
{
    JumpPressedTime = GetWorld()->GetTimeSeconds() - JumpCalledTime;
    if (!ensure(PlayerCapsuleComponent)) { return; }
    PlayerCapsuleComponent->SetJumpRate(JumpPressedTime);
}