// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCapsuleComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Math/UnrealMathUtility.h"

UPlayerCapsuleComponent::UPlayerCapsuleComponent()
{
    CapsuleHalfHeight = 90.0f;        // Capsule height

    SetSimulatePhysics(true);
    BodyInstance.bLockXRotation = true;     // Lock Capsule Rotations
    BodyInstance.bLockYRotation = true;
    
    SetNotifyRigidBodyCollision(true);          // Simulation Generates Hit Events
    SetCollisionProfileName(TEXT("Pawn"));      // Collision Preset Pawn

    // TODO: Reduce Bounciness to Capsule - Possibly do this with physics material
}

void UPlayerCapsuleComponent::BeginPlay()
{
    Super::BeginPlay();

    OnComponentHit.AddDynamic(this, &UPlayerCapsuleComponent::OnHit);
    //MainAnimation = Cast<UMainAnimInstance>(Cast<USkeletalMeshComponent>(GetChildComponent(0))->GetAnimInstance());
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

//////////////////////////////// Movement ///////////////////////////////////////
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
    if(GetComponentLocation().Z > CapsuleHalfHeight + 20.1) { return; }    // quick workaround to check if capsul is in air - May have to fix later
    CurrentJumpRate = FMath::Clamp<float>(Rate, 0, 1);
}

// TODO : Use Camera Forward and Right Vectors
void UPlayerCapsuleComponent::Move() 
{
    // if (Pivot == true)
    // {
    //     FVector TotalForceToApply = -GetForwardVector() * CurrentRightRate * MaxMoveForce;
    //     AddForce(TotalForceToApply, NAME_None, true);
    // }

    // If CurrentForwardRate and CurrentRightRate == 0 
        // Then Auto place feet to default positions (according to capsule location and body angle)
    
    FVector ForwardForceToApply = GetRightVector() * CurrentForwardRate * MaxMoveForce;
    FVector RightForceToApply = -GetForwardVector() * CurrentRightRate * MaxMoveForce;
    FVector TotalForceToApply = ForwardForceToApply + RightForceToApply;
    TotalForceToApply = TotalForceToApply.GetClampedToSize2D(-MaxMoveForce, MaxMoveForce);
    AddForce(TotalForceToApply, NAME_None, true);

    //if (!ensure(MainAnimation)) { return; }

    // If Capsule moves,
    // Pass (TotalForceToApply vector / MaxMoveForce) then add (Result * MaxReach) to TargetFootLocation

    //MainAnimation->SetFootTargetLocation(AddToDirection/MaxMoveForce);
        // use vector magnitude as amplitude for sine wave (used to calculate foot positions)
        // On PIVOT
            // 1. left stick - closer foot moves towards direction of capsule movement - pass FVector( current forward rate, current right rate, 0)
}

// TODO : Update Feet position when jumping
void UPlayerCapsuleComponent::Jump()
{
    AddImpulse(FVector(0, 0, CurrentJumpRate * MaxMoveForce), NAME_None, true);
}

// TODO : Change angle to direction of right stick
// TODO : Should be rotating the capsule itself with circular motion on pivot
void UPlayerCapsuleComponent::Turn(float ZRotation)
{
    //if (!ensure(MainAnimation)) { return; }

    // if pivot
        // Calculate root distance from target leg and make circular capsule movement
        
    //MainAnimation->SetZRotation(ZRotation);
}