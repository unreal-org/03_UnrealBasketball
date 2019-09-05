// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCapsuleComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Math/UnrealMathUtility.h"

void UPlayerCapsuleComponent::BeginPlay()
{
    Super::BeginPlay();
    OnComponentHit.AddDynamic(this, &UPlayerCapsuleComponent::OnHit);
}

void UPlayerCapsuleComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UPlayerCapsuleComponent::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{ 
    Move();
    Jump();
    CurrentForwardRate = 0;  
    CurrentRightRate = 0;
    CurrentJumpRate = 0;
}

void UPlayerCapsuleComponent::SetMoveForwardRate(float Rate)
{
    CurrentForwardRate = FMath::Clamp<float>(CurrentForwardRate + Rate, -1, 1);
}

void UPlayerCapsuleComponent::SetMoveRightRate(float Rate)
{
    CurrentRightRate = FMath::Clamp<float>(CurrentRightRate + Rate, -1, 1);
}

void UPlayerCapsuleComponent::SetJumpRate(float Rate)
{
    CurrentJumpRate = FMath::Clamp<float>(Rate, 0, 1);
}

void UPlayerCapsuleComponent::Move()
{
    FVector ForwardForceToApply = FVector(1, 0, 0) * CurrentForwardRate * MaxMoveForce;
    FVector RightForceToApply = FVector(0, 1, 0) * CurrentRightRate * MaxMoveForce;
    FVector TotalForceToApply = ForwardForceToApply + RightForceToApply;
    AddForce(TotalForceToApply, NAME_None, true);
}

void UPlayerCapsuleComponent::Jump()
{
    AddImpulse(FVector(0, 0, CurrentJumpRate * MaxMoveForce), NAME_None, true);
}