// Fill out your copyright notice in the Description page of Project Settings.


#include "HoopzPlayer.h"
#include "PlayerCapsuleComponent.h"
#include "HoopzPlayerMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "UObject/UObjectBaseUtility.h"

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

	PlayerCapsuleComponent = FindComponentByClass<UPlayerCapsuleComponent>();
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

/** Get FAnimMontageInstance playing RootMotion */
FAnimMontageInstance * AHoopzPlayer::GetRootMotionAnimMontageInstance() const
{
	if (!ensure(PlayerCapsuleComponent)) { return; }
	USkeletalMeshComponent* Mesh = FindComponentByClass<USkeletalMeshComponent>();
	return (Mesh && Mesh->GetAnimInstance()) ? Mesh->GetAnimInstance()->GetRootMotionMontageInstance() : nullptr;
}

/**	Change the Pawn's base. */
void AHoopzPlayer::SetBase( UPrimitiveComponent* NewBaseComponent, const FName InBoneName, bool bNotifyPawn )
{
	// If NewBaseComponent is nullptr, ignore bone name.
	const FName BoneName = (NewBaseComponent ? InBoneName : NAME_None);

	// See what changed.
	const bool bBaseChanged = (NewBaseComponent != BasedMovement.MovementBase);
	const bool bBoneChanged = (BoneName != BasedMovement.BoneName);

	if (bBaseChanged || bBoneChanged)
	{
		// Verify no recursion.
		APawn* Loop = (NewBaseComponent ? Cast<APawn>(NewBaseComponent->GetOwner()) : nullptr);
		while (Loop)
		{
			if (Loop == this)
			{
				UE_LOG(LogTemp, Warning, TEXT(" SetBase failed! Recursion detected. Pawn %s already based on %s."), *GetName(), *NewBaseComponent->GetName()); //-V595 -LogCharacter?
				return;
			}
			if (UPrimitiveComponent* LoopBase =	Loop->GetMovementBase())
			{
				Loop = Cast<APawn>(LoopBase->GetOwner());
			}
			else
			{
				break;
			}
		}

		// Set base.
		UPrimitiveComponent* OldBase = BasedMovement.MovementBase;
		BasedMovement.MovementBase = NewBaseComponent;
		BasedMovement.BoneName = BoneName;

		if (PawnMovement)
		{
			const bool bBaseIsSimulating = NewBaseComponent && NewBaseComponent->IsSimulatingPhysics();
			if (bBaseChanged)
			{
				MovementBaseUtility::RemoveTickDependency(PawnMovement->PrimaryComponentTick, OldBase);
				// We use a special post physics function if simulating, otherwise add normal tick prereqs.
				if (!bBaseIsSimulating)
				{
					MovementBaseUtility::AddTickDependency(PawnMovement->PrimaryComponentTick, NewBaseComponent);
				}
			}

			if (NewBaseComponent)
			{
				// Update OldBaseLocation/Rotation as those were referring to a different base
				// ... but not when handling replication for proxies (since they are going to copy this data from the replicated values anyway)
				if (!bInBaseReplication)
				{
					// Force base location and relative position to be computed since we have a new base or bone so the old relative offset is meaningless.
					PawnMovement->SaveBaseLocation();
				}

				// Enable PostPhysics tick if we are standing on a physics object, as we need to to use post-physics transforms
				PawnMovement->PostPhysicsTickFunction.SetTickFunctionEnable(bBaseIsSimulating);
			}
			else
			{
				BasedMovement.BoneName = NAME_None; // None, regardless of whether user tried to set a bone name, since we have no base component.
				BasedMovement.bRelativeRotation = false;
				PawnMovement->CurrentFloor.Clear();
				PawnMovement->PostPhysicsTickFunction.SetTickFunctionEnable(false);
			}

			if (Role == ROLE_Authority || Role == ROLE_AutonomousProxy)
			{
				BasedMovement.bServerHasBaseComponent = (BasedMovement.MovementBase != nullptr); // Also set on proxies for nicer debugging.
				UE_LOG(LogTemp, Verbose, TEXT("Setting base on %s for '%s' to '%s'"), Role == ROLE_Authority ? TEXT("Server") : TEXT("AutoProxy"), *GetName(), *GetFullNameSafe(NewBaseComponent)); // LogCharacter?
			}
			else
			{
				UE_LOG(LogTemp, Verbose, TEXT("Setting base on Client for '%s' to '%s'"), *GetName(), *GetFullNameSafe(NewBaseComponent)); // LogCharacter?
			}

		}

		// Notify this actor of his new floor.
		if ( bNotifyPawn )
		{
			BaseChange();
		}
	}
}

void AHoopzPlayer::BaseChange()
{
	if (PawnMovement && PawnMovement->MovementMode != MOVE_None)
	{
		AActor* ActualMovementBase = GetMovementBaseActor(this);
		if ((ActualMovementBase != nullptr) && !ActualMovementBase->CanBeBaseForCharacter(this))
		{
			PawnMovement->JumpOff(ActualMovementBase);
		}
	}
}
