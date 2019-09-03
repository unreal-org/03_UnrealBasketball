// Fill out your copyright notice in the Description page of Project Settings.


#include "HoopzPlayerMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Math/UnrealMathUtility.h"

void UHoopzPlayerMovementComponent::Initialize(UCapsuleComponent* CapsuleComponentToSet)
{
    CapsuleComponent = CapsuleComponentToSet;
}

void UHoopzPlayerMovementComponent::MoveForward(float Throw)
{
    if (!ensure(CapsuleComponent)) { return; }

    CurrentThrow = FMath::Clamp<float>(CurrentThrow + Throw, -1, 1);
    FVector ForceToApply = CapsuleComponent->GetForwardVector() * CurrentThrow * MaxMovementForce;
    FVector CapsuleLocation = CapsuleComponent->GetComponentLocation();
    CapsuleComponent->AddForceAtLocation(ForceToApply, CapsuleLocation);
    
}

void UHoopzPlayerMovementComponent::MoveRight(float Throw)
{
    CurrentThrow = FMath::Clamp<float>(CurrentThrow + Throw, -1, 1);
    FVector ForceToApply = CapsuleComponent->GetRightVector() * CurrentThrow * MaxMovementForce;
    FVector CapsuleLocation = CapsuleComponent->GetComponentLocation();
    CapsuleComponent->AddForceAtLocation(ForceToApply, CapsuleLocation);
}