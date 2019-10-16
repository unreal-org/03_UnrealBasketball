// Fill out your copyright notice in the Description page of Project Settings.


#include "HoopzPlayerMovementComponent.h"
//#include "Components/CapsuleComponent.h"
#include "Math/UnrealMathUtility.h"
#include "PlayerCapsuleComponent.h"
#include "Engine/World.h"

// void UHoopzPlayerMovementComponent::SetUpdatedComponent(USceneComponent* NewUpdatedComponent)
// {
// 	Super::SetUpdatedComponent(NewUpdatedComponent);
// }

// void UHoopzPlayerMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
// {
//     Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

//     //if (OldRotation != NewRotation) { Pivot(DeltaTime); }
//     //Pivot(DeltaTime);
// }

void UHoopzPlayerMovementComponent::Initialize(UPlayerCapsuleComponent* PlayerCapsuleComponentToSet)
{
    PlayerCapsuleComponent = PlayerCapsuleComponentToSet;

    if (!ensure(PlayerCapsuleComponent)) { return; }
    //UpdatedComponent = Cast<USceneComponent>(PlayerCapsuleComponent);
    //SetUpdatedComponent(UpdatedComponent);
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
}

void UHoopzPlayerMovementComponent::IntendJumpReleased()
{
    JumpPressedTime = GetWorld()->GetTimeSeconds() - JumpCalledTime;
    if (!ensure(PlayerCapsuleComponent)) { return; }
    PlayerCapsuleComponent->SetJumpRate(JumpPressedTime);
}

void UHoopzPlayerMovementComponent::TurnLeft()
{
    if (!ensure(PlayerCapsuleComponent)) { return; }
    PlayerCapsuleComponent->Turn(-1);
}

void UHoopzPlayerMovementComponent::TurnRight()
{
    if (!ensure(PlayerCapsuleComponent)) { return; }
    PlayerCapsuleComponent->Turn(1);
}

// void UHoopzPlayerMovementComponent::Pivot(float DeltaTime)
// {
//     if (!ensure(UpdatedComponent)) { return; }

// 	// Compute new rotation
// 	// const FQuat OldRotation = UpdatedComponent->GetComponentQuat();
// 	// const FQuat DeltaRotation = (RotationRate * DeltaTime).Quaternion();
// 	// const FQuat NewRotation = (OldRotation * DeltaRotation);   // Local space
//     //const FQuat NewRotation = (DeltaRotation * OldRotation);   // World space

//     FRotator OldRotation = UpdatedComponent->GetComponentRotation();
// 	//const FRotator DeltaRotation = (RotationRate * DeltaTime).Quaternion();
// 	FRotator NewRotation = OldRotation + RotationRate;   // World space

// 	// Compute new location
// 	FVector DeltaLocation = FVector::ZeroVector;
// 	if (!PivotTranslation.IsZero())
// 	{
// 		const FVector OldPivot = OldRotation.RotateVector(PivotTranslation);
// 		const FVector NewPivot = NewRotation.RotateVector(PivotTranslation);
// 		DeltaLocation = (OldPivot - NewPivot); // ConstrainDirectionToPlane() not necessary because it's done by MoveUpdatedComponent() below.
// 	}

// 	const bool bEnableCollision = false;
// 	MoveUpdatedComponent(DeltaLocation, NewRotation, bEnableCollision);
//     // const FVector NewDelta = ConstrainDirectionToPlane(DeltaLocation);
//     // UpdatedComponent->MoveComponent(DeltaLocation, NewRotation, bEnableCollision, OutHit, MoveComponentFlags, Teleport);
// }