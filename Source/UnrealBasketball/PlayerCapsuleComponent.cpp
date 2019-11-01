// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCapsuleComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Math/UnrealMathUtility.h"
#include "Components/SplineComponent.h"

UPlayerCapsuleComponent::UPlayerCapsuleComponent()
{
    CapsuleHalfHeight = 95.0f;        // Capsule height

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
    PivotComponent = GetOwner()->FindComponentByClass<USceneComponent>();
    if (!ensure(PivotComponent)) { return; }
    CapsulePivotBig = dynamic_cast<USplineComponent*>(PivotComponent->GetChildComponent(6));
}

void UPlayerCapsuleComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UPlayerCapsuleComponent::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{ 
    Move();
    Jump();
    // CurrentForwardRate = 0;  
    // CurrentRightRate = 0;
    // CurrentJumpRate = 0;
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
    if(GetComponentLocation().Z > CapsuleHalfHeight + 20.1) { return; }    // TODO: quick workaround to check if capsule is in air - May have to fix later
    CurrentJumpRate = FMath::Clamp<float>(Rate, 0, 1);
}

// TODO : Negate left stick Axis rate when moving back to center
void UPlayerCapsuleComponent::Move() 
{
    if (CurrentForwardRate == 0 && CurrentRightRate == 0) { return; }
    if (Pivot == true)
    {
        /*
        if (PivotSet == false)
            if (!ensure(CapsulePivotBig)) { return; }
            PivotComponent->SetWorldLocation(GetComponentLocation());
            PivotComponent->SetWorldRotation(GetComponentRotation());
        */

        FVector ForwardForceToApply = GetForwardVector() * CurrentForwardRate * 40;
        FVector RightForceToApply = GetRightVector() * CurrentRightRate * 40;
        FVector TotalForceToApply = ForwardForceToApply + RightForceToApply;
        TotalForceToApply = TotalForceToApply.GetClampedToSize2D(-40, 40);

        if (!ensure(CapsulePivotBig)) { return; }
        TotalForceToApply += GetComponentLocation();
        PivotInputKey = CapsulePivotBig->FindInputKeyClosestToWorldLocation(TotalForceToApply);

        if (EstablishPivotFoot == false)
        { 
            if (PivotInputKey <= 4) { PivotFoot = 0; } // Left Foot
            else { PivotFoot = 1; } // Right Foot
            EstablishPivotFoot = true;
        }

        CurrentForwardRate = 0;  
        CurrentRightRate = 0;
        return;
    }

    if (GetComponentVelocity().Size2D() < 100) {
        FVector ForwardForceToApply = GetForwardVector() * CurrentForwardRate * MaxMoveForce;
        FVector RightForceToApply = GetRightVector() * CurrentRightRate * MaxMoveForce;
        FVector TotalForceToApply = ForwardForceToApply + RightForceToApply;
        TotalForceToApply = TotalForceToApply.GetClampedToSize2D(-MaxMoveForce, MaxMoveForce);
        AddForce(TotalForceToApply, NAME_None, true);
    }

    CurrentForwardRate = 0;  
    CurrentRightRate = 0;
}

// TODO : Update Feet position when jumping
void UPlayerCapsuleComponent::Jump()
{
    AddImpulse(FVector(0, 0, CurrentJumpRate * MaxMoveForce), NAME_None, true);
    CurrentJumpRate = 0;
}

// TODO : Rotate Capsule according to Pivot Splines
// TODO : Remap to Triangle and Circle
void UPlayerCapsuleComponent::Turn(float ZRotation)
{
    //PivotSet = false;
    //if (!ensure(PivotPoint)) { return; }
    //PelvisRotation.Yaw += ZRotation * 45;
}