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
    //MoveRight();
    CurrentForwardRate = 0;  
    CurrentRightRate = 0;  
}

void UPlayerCapsuleComponent::SetMoveForwardRate(float Rate)
{
    CurrentForwardRate = FMath::Clamp<float>(CurrentForwardRate + Rate, -1, 1);
    //UE_LOG(LogTemp, Warning, TEXT("MoveRate is %i."), Rate)
}

void UPlayerCapsuleComponent::SetMoveRightRate(float Rate)
{
    CurrentRightRate = FMath::Clamp<float>(CurrentRightRate + Rate, -1, 1);
    //UE_LOG(LogTemp, Warning, TEXT("MoveRate is %i."), Rate)
}

void UPlayerCapsuleComponent::Move()
{
    // FVector ForwardForceToApply = GetForwardVector() * CurrentForwardRate * MaxMoveForce;
    // FVector RightForceToApply = GetRightVector() * CurrentRightRate * MaxMoveForce;
    // FVector TotalForceToApply = ForwardForceToApply + RightForceToApply;
    //FVector ForceLocation = GetComponentLocation();
    //UPrimitiveComponent* PlayerRoot = Cast<UPrimitiveComponent>(GetOwner()->GetRootComponent());
    AddForce(FVector(-1000, 0, 0), NAME_None, true);
    //UE_LOG(LogTemp, Warning, TEXT("Move called on %s."), *PlayerRoot->GetName())
}

// void UPlayerCapsuleComponent::MoveRight()
// {
//     FVector ForceToApply = GetRightVector() * CurrentRate * MaxMoveForce;
//     FVector ForceLocation = GetComponentLocation();
//     //UPrimitiveComponent* PlayerRoot = Cast<UPrimitiveComponent>(GetOwner()->GetRootComponent());
//     this->AddForceAtLocation(ForceToApply, ForceLocation);
//     //UE_LOG(LogTemp, Warning, TEXT("MoveRight called."))
// }