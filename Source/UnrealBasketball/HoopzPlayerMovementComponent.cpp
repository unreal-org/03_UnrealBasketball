// Fill out your copyright notice in the Description page of Project Settings.


#include "HoopzPlayerMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Math/UnrealMathUtility.h"
#include "PlayerCapsuleComponent.h"

// void UHoopzPlayerMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
// {
//     Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

//     // Make sure that everything is still valid, and that we are allowed to move.
//     if (!PawnOwner || !UpdatedComponent || ShouldSkipUpdate(DeltaTime))
//     {
//         return;
//     }

//     // Get (and then clear) the movement vector that we set in ACollidingPawn::Tick
//     FVector DesiredMovementThisFrame = ConsumeInputVector().GetClampedToMaxSize(1.0f) * DeltaTime * 150.0f;
//     if (!DesiredMovementThisFrame.IsNearlyZero())
//     {
//         FHitResult Hit;
//         SafeMoveUpdatedComponent(DesiredMovementThisFrame, UpdatedComponent->GetComponentRotation(), true, Hit);

//         // If we bumped into something, try to slide along it
//         if (Hit.IsValidBlockingHit())
//         {
//             SlideAlongSurface(DesiredMovementThisFrame, 1.f - Hit.Time, Hit.Normal, Hit);
//         }
//     }
// };

void UHoopzPlayerMovementComponent::Initialize(UPlayerCapsuleComponent* PlayerCapsuleComponentToSet)
{
    PlayerCapsuleComponent = PlayerCapsuleComponentToSet;
}

void UHoopzPlayerMovementComponent::IntendMoveForward(float Throw)
{
    if (!ensure(PlayerCapsuleComponent)) { return; }

    PlayerCapsuleComponent->SetMoveForwardRate(Throw);
    //UE_LOG(LogTemp, Warning, TEXT("ForwardMoveRate is %i."), Throw)
}

void UHoopzPlayerMovementComponent::IntendMoveRight(float Throw)
{
    if (!ensure(PlayerCapsuleComponent)) { return; }

    PlayerCapsuleComponent->SetMoveRightRate(Throw);
    //UE_LOG(LogTemp, Warning, TEXT("RightMoveRate is %i."), Throw)
}