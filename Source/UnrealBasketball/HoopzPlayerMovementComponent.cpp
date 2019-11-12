// Fill out your copyright notice in the Description page of Project Settings.


#include "HoopzPlayerMovementComponent.h"
//#include "Components/CapsuleComponent.h"
#include "Math/UnrealMathUtility.h"
#include "PlayerCapsuleComponent.h"
#include "Engine/World.h"
#include "Components/SkeletalMeshComponent.h"
//#include "GameFramework/Actor.h"
#include "HoopzPlayer.h"
//#include "GameFramework/CharacterMovementComponent.h"
#include "AI/NavigationSystemBase.h"
#include "ParticleEmitterInstances.h"

//DECLARE_CYCLE_STAT(TEXT("Char FindFloor"), STAT_CharFindFloor, STATGROUP_Character);

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
    // HoopzPlayer = dynamic_cast<AHoopzPlayer*>(PlayerCapsuleComponent->GetOwner());
    // if (!ensure(HoopzPlayer)) { return; }
    // PlayerSkeletalMesh = GetOwner()->FindComponentByClass<USkeletalMeshComponent>();
    // if (!ensure(PlayerSkeletalMesh)) { return; }
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

/** Root Motion 

bool UHoopzPlayerMovementComponent::HasRootMotionSources() const
{
	return CurrentRootMotion.HasActiveRootMotionSources() || (HoopzPlayer && PlayerSkeletalMesh && PlayerSkeletalMesh->IsPlayingRootMotion());
}

uint16 UHoopzPlayerMovementComponent::ApplyRootMotionSource(FRootMotionSource* SourcePtr)
{
	if (SourcePtr != nullptr)
	{
		// Set default StartTime if it hasn't been set manually
		if (!SourcePtr->IsStartTimeValid())
		{
			if (HoopzPlayer)
			{
				if (HoopzPlayer->Role == ROLE_AutonomousProxy)
				{
					// Autonomous defaults to local timestamp
					FNetworkPredictionData_Client_HoopzPawn* ClientData = GetPredictionData_Client_HoopzPawn();
					if (ClientData)
					{
						SourcePtr->StartTime = ClientData->CurrentTimeStamp;
					}
				}
				else if (HoopzPlayer->Role == ROLE_Authority && !IsNetMode(NM_Client))
				{
					// Authority defaults to current client time stamp, meaning it'll start next tick if not corrected
					FNetworkPredictionData_Server_HoopzPawn* ServerData = GetPredictionData_Server_HoopzPawn();
					if (ServerData)
					{
						SourcePtr->StartTime = ServerData->CurrentClientTimeStamp;
					}
				}
			}
		}

		OnRootMotionSourceBeingApplied(SourcePtr);

		return CurrentRootMotion.ApplyRootMotionSource(SourcePtr);
	}
	else
	{
		checkf(false, TEXT("Passing nullptr into UCharacterMovementComponent::ApplyRootMotionSource"));
	}

	return (uint16)ERootMotionSourceID::Invalid;
}

void UHoopzPlayerMovementComponent::OnRootMotionSourceBeingApplied(const FRootMotionSource* Source)
{
}

TSharedPtr<FRootMotionSource> UHoopzPlayerMovementComponent::GetRootMotionSource(FName InstanceName)
{
	return CurrentRootMotion.GetRootMotionSource(InstanceName);
}

TSharedPtr<FRootMotionSource> UHoopzPlayerMovementComponent::GetRootMotionSourceByID(uint16 RootMotionSourceID)
{
	return CurrentRootMotion.GetRootMotionSourceByID(RootMotionSourceID);
}

void UHoopzPlayerMovementComponent::RemoveRootMotionSource(FName InstanceName)
{
	CurrentRootMotion.RemoveRootMotionSource(InstanceName);
}

void UHoopzPlayerMovementComponent::RemoveRootMotionSourceByID(uint16 RootMotionSourceID)
{
	CurrentRootMotion.RemoveRootMotionSourceByID(RootMotionSourceID);
}

FTransform UHoopzPlayerMovementComponent::ConvertLocalRootMotionToWorld(const FTransform& LocalRootMotionTransform)
{
	const FTransform PreProcessedRootMotion = ProcessRootMotionPreConvertToWorld.IsBound() ? ProcessRootMotionPreConvertToWorld.Execute(LocalRootMotionTransform, this) : LocalRootMotionTransform;
	const FTransform WorldSpaceRootMotion = PlayerSkeletalMesh->ConvertLocalRootMotionToWorld(PreProcessedRootMotion);
	return ProcessRootMotionPostConvertToWorld.IsBound() ? ProcessRootMotionPostConvertToWorld.Execute(WorldSpaceRootMotion, this) : WorldSpaceRootMotion;
}

FVector UHoopzPlayerMovementComponent::CalcAnimRootMotionVelocity(const FVector& RootMotionDeltaMove, float DeltaSeconds, const FVector& CurrentVelocity) const
{
	if (ensure(DeltaSeconds > 0.f))
	{
		FVector RootMotionVelocity = RootMotionDeltaMove / DeltaSeconds;
		return RootMotionVelocity;
	}
	else
	{
		return CurrentVelocity;
	}
}

FVector UHoopzPlayerMovementComponent::ConstrainAnimRootMotionVelocity(const FVector& RootMotionVelocity, const FVector& CurrentVelocity) const
{
	FVector Result = RootMotionVelocity;

	// Do not override Velocity.Z if in falling physics, we want to keep the effect of gravity.
	if (IsFalling())
	{
		Result.Z = CurrentVelocity.Z;
	}

	return Result;
}

FNetworkPredictionData_Client_HoopzPawn* UHoopzPlayerMovementComponent::GetPredictionData_Client_HoopzPawn() const
{
	// Should only be called on client or listen server (for remote clients) in network games
	checkSlow(CharacterOwner != NULL);
	checkSlow(CharacterOwner->Role < ROLE_Authority || (CharacterOwner->GetRemoteRole() == ROLE_AutonomousProxy && GetNetMode() == NM_ListenServer));
	checkSlow(GetNetMode() == NM_Client || GetNetMode() == NM_ListenServer);

	if (ClientPredictionData == nullptr)
	{
		UHoopzPlayerMovementComponent* MutableThis = const_cast<UHoopzPlayerMovementComponent*>(this);
		MutableThis->ClientPredictionData = static_cast<class FNetworkPredictionData_Client_HoopzPawn*>(GetPredictionData_Client());
	}

	return ClientPredictionData;
}

FNetworkPredictionData_Server_HoopzPawn* UHoopzPlayerMovementComponent::GetPredictionData_Server_HoopzPawn() const
{
	// Should only be called on server in network games
	checkSlow(CharacterOwner != NULL);
	checkSlow(CharacterOwner->Role == ROLE_Authority);
	checkSlow(GetNetMode() < NM_Client);

	if (ServerPredictionData == nullptr)
	{
		UHoopzPlayerMovementComponent* MutableThis = const_cast<UHoopzPlayerMovementComponent*>(this);
		MutableThis->ServerPredictionData = static_cast<class FNetworkPredictionData_Server_HoopzPawn*>(GetPredictionData_Server());
	}

	return ServerPredictionData;
}


FNetworkPredictionData_Client* UHoopzPlayerMovementComponent::GetPredictionData_Client() const
{
	if (ClientPredictionData == nullptr)
	{
		UHoopzPlayerMovementComponent* MutableThis = const_cast<UHoopzPlayerMovementComponent*>(this);
		MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_HoopzPawn(*this);
	}

	return ClientPredictionData;
}

FNetworkPredictionData_Server* UHoopzPlayerMovementComponent::GetPredictionData_Server() const
{
	if (ServerPredictionData == nullptr)
	{
		UHoopzPlayerMovementComponent* MutableThis = const_cast<UHoopzPlayerMovementComponent*>(this);
		MutableThis->ServerPredictionData = new FNetworkPredictionData_Server_HoopzPawn(*this);
	}

	return ServerPredictionData;
}

namespace PackedMovementModeConstants
{
	const uint32 GroundShift = FMath::CeilLogTwo(MOVE_MAX);
	const uint8 CustomModeThr = 2 * (1 << GroundShift);
	const uint8 GroundMask = (1 << GroundShift) - 1;
}

uint8 UHoopzPlayerMovementComponent::PackNetworkMovementMode() const
{
	if (MovementMode != MOVE_Custom)
	{
		ensureMsgf(GroundMovementMode == MOVE_Walking || GroundMovementMode == MOVE_NavWalking, TEXT("Invalid GroundMovementMode %d."), GroundMovementMode.GetValue());
		const uint8 GroundModeBit = (GroundMovementMode == MOVE_Walking ? 0 : 1);
		return uint8(MovementMode.GetValue()) | (GroundModeBit << PackedMovementModeConstants::GroundShift);
	}
	else
	{
		return CustomMovementMode + PackedMovementModeConstants::CustomModeThr;
	}
}

ETeleportType UHoopzPlayerMovementComponent::GetTeleportType() const
{ 
	return bJustTeleported || bNetworkLargeClientCorrection ? ETeleportType::TeleportPhysics : ETeleportType::None;
}

void UHoopzPlayerMovementComponent::SetBase( UPrimitiveComponent* NewBase, FName BoneName, bool bNotifyActor )
{
	// prevent from changing Base while server is NavWalking (no Base in that mode), so both sides are in sync
	// otherwise it will cause problems with position smoothing

	if (HoopzPlayer && !bIsNavWalkingOnServer)
	{
		HoopzPlayer->SetBase(NewBase, NewBase ? BoneName : NAME_None, bNotifyActor);
	}
}

void UHoopzPlayerMovementComponent::SaveBaseLocation()
{
	if (!HasValidData())
	{
		return;
	}

	const UPrimitiveComponent* MovementBase = HoopzPlayer->GetMovementBase();
	if (MovementBaseUtility::UseRelativeLocation(MovementBase) && !HoopzPlayer->IsMatineeControlled())
	{
		// Read transforms into OldBaseLocation, OldBaseQuat
		MovementBaseUtility::GetMovementBaseTransform(MovementBase, HoopzPlayer->GetBasedMovement().BoneName, OldBaseLocation, OldBaseQuat);

		// Location
		const FVector RelativeLocation = UpdatedComponent->GetComponentLocation() - OldBaseLocation;

		// Rotation
		if (bIgnoreBaseRotation)
		{
			// Absolute rotation
			HoopzPlayer->SaveRelativeBasedMovement(RelativeLocation, UpdatedComponent->GetComponentRotation(), false);
		}
		else
		{
			// Relative rotation
			const FRotator RelativeRotation = (FQuatRotationMatrix(UpdatedComponent->GetComponentQuat()) * FQuatRotationMatrix(OldBaseQuat).GetTransposed()).Rotator();
			HoopzPlayer->SaveRelativeBasedMovement(RelativeLocation, RelativeRotation, true);
		}
	}
}

bool UHoopzPlayerMovementComponent::HasValidData() const
{
	const bool bIsValid = UpdatedComponent && IsValid(HoopzPlayer);
#if ENABLE_NAN_DIAGNOSTIC
	if (bIsValid)
	{
		// NaN-checking updates
		if (Velocity.ContainsNaN())
		{
			logOrEnsureNanError(TEXT("UCharacterMovementComponent::HasValidData() detected NaN/INF for (%s) in Velocity:\n%s"), *GetPathNameSafe(this), *Velocity.ToString());
			UCharacterMovementComponent* MutableThis = const_cast<UCharacterMovementComponent*>(this);
			MutableThis->Velocity = FVector::ZeroVector;
		}
		if (!UpdatedComponent->GetComponentTransform().IsValid())
		{
			logOrEnsureNanError(TEXT("UCharacterMovementComponent::HasValidData() detected NaN/INF for (%s) in UpdatedComponent->ComponentTransform:\n%s"), *GetPathNameSafe(this), *UpdatedComponent->GetComponentTransform().ToHumanReadableString());
		}
		if (UpdatedComponent->GetComponentRotation().ContainsNaN())
		{
			logOrEnsureNanError(TEXT("UCharacterMovementComponent::HasValidData() detected NaN/INF for (%s) in UpdatedComponent->GetComponentRotation():\n%s"), *GetPathNameSafe(this), *UpdatedComponent->GetComponentRotation().ToString());
		}
	}
#endif
	return bIsValid;
}

void UHoopzPlayerMovementComponent::JumpOff(AActor* MovementBaseActor)
{
	if ( !bPerformingJumpOff )
	{
		bPerformingJumpOff = true;
		if ( HoopzPlayer )
		{
			const float MaxSpeed = GetMaxSpeed() * 0.85f;
			Velocity += MaxSpeed * GetBestDirectionOffActor(MovementBaseActor);
			if ( Velocity.Size2D() > MaxSpeed )
			{
				Velocity = MaxSpeed * Velocity.GetSafeNormal();
			}
			Velocity.Z = JumpOffJumpZFactor * JumpZVelocity;
			SetMovementMode(MOVE_Falling);
		}
		bPerformingJumpOff = false;
	}
}

FVector UHoopzPlayerMovementComponent::GetBestDirectionOffActor(AActor* BaseActor) const
{
	// By default, just pick a random direction.  Derived character classes can choose to do more complex calculations,
	// such as finding the shortest distance to move in based on the BaseActor's Bounding Volume.
	const float RandAngle = FMath::DegreesToRadians(GetNetworkSafeRandomAngleDegrees());
	return FVector(FMath::Cos(RandAngle), FMath::Sin(RandAngle), 0.5f).GetSafeNormal();
}

float UHoopzPlayerMovementComponent::GetNetworkSafeRandomAngleDegrees() const
{
	float Angle = RandomStream.FRand() * 360.f;

	if (!IsNetMode(NM_Standalone))
	{
		// Networked game
		// Get a timestamp that is relatively close between client and server (within ping).
		FNetworkPredictionData_Server_HoopzPawn const* ServerData = (HasPredictionData_Server() ? GetPredictionData_Server_HoopzPawn() : NULL);
		FNetworkPredictionData_Client_HoopzPawn const* ClientData = (HasPredictionData_Client() ? GetPredictionData_Client_HoopzPawn() : NULL);

		float TimeStamp = Angle;
		if (ServerData)
		{
			TimeStamp = ServerData->CurrentClientTimeStamp;
		}
		else if (ClientData)
		{
			TimeStamp = ClientData->CurrentTimeStamp;
		}
		
		// Convert to degrees with a faster period.
		const float PeriodMult = 8.0f;
		Angle = TimeStamp * PeriodMult;
		Angle = FMath::Fmod(Angle, 360.f);
	}

	return Angle;
}

bool UHoopzPlayerMovementComponent::HasPredictionData_Client() const
{
	return (ClientPredictionData != nullptr) && HasValidData();
}

bool UHoopzPlayerMovementComponent::HasPredictionData_Server() const
{
	return (ServerPredictionData != nullptr) && HasValidData();
}

void UHoopzPlayerMovementComponent::SetMovementMode(EMovementMode NewMovementMode, uint8 NewCustomMode)
{
	if (NewMovementMode != MOVE_Custom)
	{
		NewCustomMode = 0;
	}

	// If trying to use NavWalking but there is no navmesh, use walking instead.
	if (NewMovementMode == MOVE_NavWalking)
	{
		if (GetNavData() == nullptr)
		{
			NewMovementMode = MOVE_Walking;
		}
	}

	// Do nothing if nothing is changing.
	if (MovementMode == NewMovementMode)
	{
		// Allow changes in custom sub-mode.
		if ((NewMovementMode != MOVE_Custom) || (NewCustomMode == CustomMovementMode))
		{
			return;
		}
	}

	const EMovementMode PrevMovementMode = MovementMode;
	const uint8 PrevCustomMode = CustomMovementMode;

	MovementMode = NewMovementMode;
	CustomMovementMode = NewCustomMode;

	// We allow setting movement mode before we have a component to update, in case this happens at startup.
	if (!HasValidData())
	{
		return;
	}
	
	// Handle change in movement mode
	OnMovementModeChanged(PrevMovementMode, PrevCustomMode);

	// @todo UE4 do we need to disable ragdoll physics here? Should this function do nothing if in ragdoll?
}

const INavigationDataInterface* UHoopzPlayerMovementComponent::GetNavData() const
{
	const UWorld* World = GetWorld();
	if (World == nullptr || World->GetNavigationSystem() == nullptr 
		|| !HasValidData()
		|| HoopzPlayer == nullptr)
	{
		return nullptr;
	}

	const INavigationDataInterface* NavData = FNavigationSystem::GetNavDataForActor(*HoopzPlayer);

	return NavData;
}

void UHoopzPlayerMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	if (!HasValidData())
	{
		return;
	}

	// Update collision settings if needed
	if (MovementMode == MOVE_NavWalking)
	{
		GroundMovementMode = MovementMode;
		// Walking uses only XY velocity
		Velocity.Z = 0.f;
		SetNavWalkingPhysics(true);
	}
	else if (PreviousMovementMode == MOVE_NavWalking)
	{
		if (MovementMode == DefaultLandMovementMode || IsWalking())
		{
			const bool bSucceeded = TryToLeaveNavWalking();
			if (!bSucceeded)
			{
				return;
			}
		}
		else
		{
			SetNavWalkingPhysics(false);
		}
	}

	// React to changes in the movement mode.
	if (MovementMode == MOVE_Walking)
	{
		// Walking uses only XY velocity, and must be on a walkable floor, with a Base.
		Velocity.Z = 0.f;
		bCrouchMaintainsBaseLocation = true;
		GroundMovementMode = MovementMode;

		// make sure we update our new floor/base on initial entry of the walking physics
		FindFloor(UpdatedComponent->GetComponentLocation(), CurrentFloor, false);
		AdjustFloorHeight();
		SetBaseFromFloor(CurrentFloor);
	}
	else
	{
		CurrentFloor.Clear();
		bCrouchMaintainsBaseLocation = false;

		if (MovementMode == MOVE_Falling)
		{
			Velocity += GetImpartedMovementBaseVelocity();
			HoopzPlayer->Falling();
		}

		SetBase(NULL);

		if (MovementMode == MOVE_None)
		{
			// Kill velocity and clear queued up events
			StopMovementKeepPathing();
			HoopzPlayer->ResetJumpState();
			ClearAccumulatedForces();
		}
	}

	if (MovementMode == MOVE_Falling && PreviousMovementMode != MOVE_Falling)
	{
		IPathFollowingAgentInterface* PFAgent = GetPathFollowingAgent();
		if (PFAgent)
		{
			PFAgent->OnStartedFalling();
		}
	}

	HoopzPlayer->OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
	ensureMsgf(GroundMovementMode == MOVE_Walking || GroundMovementMode == MOVE_NavWalking, TEXT("Invalid GroundMovementMode %d. MovementMode: %d, PreviousMovementMode: %d"), GroundMovementMode.GetValue(), MovementMode.GetValue(), PreviousMovementMode);
}

void UHoopzPlayerMovementComponent::SetNavWalkingPhysics(bool bEnable)
{
	if (UpdatedPrimitive)
	{
		if (bEnable)
		{
			UpdatedPrimitive->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Ignore);
			UpdatedPrimitive->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);
			CachedProjectedNavMeshHitResult.Reset();

			// Stagger timed updates so many different characters spawned at the same time don't update on the same frame.
			// Initially we want an immediate update though, so set time to a negative randomized range.
			NavMeshProjectionTimer = (NavMeshProjectionInterval > 0.f) ? FMath::FRandRange(-NavMeshProjectionInterval, 0.f) : 0.f;
		}
		else
		{
			UPrimitiveComponent* DefaultCapsule = nullptr;
			if (HoopzPlayer && HoopzPlayer->GetCapsuleComponent() == UpdatedComponent)
			{
				AHoopzPlayer* DefaultCharacter = HoopzPlayer->GetClass()->GetDefaultObject<AHoopzPlayer>();
				DefaultCapsule = DefaultCharacter ? DefaultCharacter->GetCapsuleComponent() : nullptr;
			}

			if (DefaultCapsule)
			{
				UpdatedPrimitive->SetCollisionResponseToChannel(ECC_WorldStatic, DefaultCapsule->GetCollisionResponseToChannel(ECC_WorldStatic));
				UpdatedPrimitive->SetCollisionResponseToChannel(ECC_WorldDynamic, DefaultCapsule->GetCollisionResponseToChannel(ECC_WorldDynamic));
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Can't revert NavWalking collision settings for %s.%s"), // LogCharacterMovement?
					*GetNameSafe(HoopzPlayer), *GetNameSafe(UpdatedComponent));
			}
		}
	}
}

void UHoopzPlayerMovementComponent::FindFloor(const FVector& CapsuleLocation, FFindFloorResult& OutFloorResult, bool bCanUseCachedLocation, const FHitResult* DownwardSweepResult) const
{
	SCOPE_CYCLE_COUNTER(STAT_CharFindFloor);

	// No collision, no floor...
	if (!HasValidData() || !UpdatedComponent->IsQueryCollisionEnabled())
	{
		OutFloorResult.Clear();
		return;
	}

	UE_LOG(LogCharacterMovement, VeryVerbose, TEXT("[Role:%d] FindFloor: %s at location %s"), (int32)CharacterOwner->Role, *GetNameSafe(CharacterOwner), *CapsuleLocation.ToString());
	check(CharacterOwner->GetCapsuleComponent());

	// Increase height check slightly if walking, to prevent floor height adjustment from later invalidating the floor result.
	const float HeightCheckAdjust = (IsMovingOnGround() ? MAX_FLOOR_DIST + KINDA_SMALL_NUMBER : -MAX_FLOOR_DIST);

	float FloorSweepTraceDist = FMath::Max(MAX_FLOOR_DIST, MaxStepHeight + HeightCheckAdjust);
	float FloorLineTraceDist = FloorSweepTraceDist;
	bool bNeedToValidateFloor = true;
	
	// Sweep floor
	if (FloorLineTraceDist > 0.f || FloorSweepTraceDist > 0.f)
	{
		UCharacterMovementComponent* MutableThis = const_cast<UCharacterMovementComponent*>(this);

		if ( bAlwaysCheckFloor || !bCanUseCachedLocation || bForceNextFloorCheck || bJustTeleported )
		{
			MutableThis->bForceNextFloorCheck = false;
			ComputeFloorDist(CapsuleLocation, FloorLineTraceDist, FloorSweepTraceDist, OutFloorResult, CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleRadius(), DownwardSweepResult);
		}
		else
		{
			// Force floor check if base has collision disabled or if it does not block us.
			UPrimitiveComponent* MovementBase = CharacterOwner->GetMovementBase();
			const AActor* BaseActor = MovementBase ? MovementBase->GetOwner() : NULL;
			const ECollisionChannel CollisionChannel = UpdatedComponent->GetCollisionObjectType();

			if (MovementBase != NULL)
			{
				MutableThis->bForceNextFloorCheck = !MovementBase->IsQueryCollisionEnabled()
				|| MovementBase->GetCollisionResponseToChannel(CollisionChannel) != ECR_Block
				|| MovementBaseUtility::IsDynamicBase(MovementBase);
			}

			const bool IsActorBasePendingKill = BaseActor && BaseActor->IsPendingKill();

			if ( !bForceNextFloorCheck && !IsActorBasePendingKill && MovementBase )
			{
				//UE_LOG(LogCharacterMovement, Log, TEXT("%s SKIP check for floor"), *CharacterOwner->GetName());
				OutFloorResult = CurrentFloor;
				bNeedToValidateFloor = false;
			}
			else
			{
				MutableThis->bForceNextFloorCheck = false;
				ComputeFloorDist(CapsuleLocation, FloorLineTraceDist, FloorSweepTraceDist, OutFloorResult, CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleRadius(), DownwardSweepResult);
			}
		}
	}

	// OutFloorResult.HitResult is now the result of the vertical floor check.
	// See if we should try to "perch" at this location.
	if (bNeedToValidateFloor && OutFloorResult.bBlockingHit && !OutFloorResult.bLineTrace)
	{
		const bool bCheckRadius = true;
		if (ShouldComputePerchResult(OutFloorResult.HitResult, bCheckRadius))
		{
			float MaxPerchFloorDist = FMath::Max(MAX_FLOOR_DIST, MaxStepHeight + HeightCheckAdjust);
			if (IsMovingOnGround())
			{
				MaxPerchFloorDist += FMath::Max(0.f, PerchAdditionalHeight);
			}

			FFindFloorResult PerchFloorResult;
			if (ComputePerchResult(GetValidPerchRadius(), OutFloorResult.HitResult, MaxPerchFloorDist, PerchFloorResult))
			{
				// Don't allow the floor distance adjustment to push us up too high, or we will move beyond the perch distance and fall next time.
				const float AvgFloorDist = (MIN_FLOOR_DIST + MAX_FLOOR_DIST) * 0.5f;
				const float MoveUpDist = (AvgFloorDist - OutFloorResult.FloorDist);
				if (MoveUpDist + PerchFloorResult.FloorDist >= MaxPerchFloorDist)
				{
					OutFloorResult.FloorDist = AvgFloorDist;
				}

				// If the regular capsule is on an unwalkable surface but the perched one would allow us to stand, override the normal to be one that is walkable.
				if (!OutFloorResult.bWalkableFloor)
				{
					OutFloorResult.SetFromLineTrace(PerchFloorResult.HitResult, OutFloorResult.FloorDist, FMath::Min(PerchFloorResult.FloorDist, PerchFloorResult.LineDist), true);
				}
			}
			else
			{
				// We had no floor (or an invalid one because it was unwalkable), and couldn't perch here, so invalidate floor (which will cause us to start falling).
				OutFloorResult.bWalkableFloor = false;
			}
		}
	}
}


// ///////////////////////////** FSavedMove_HoopzPawn *///////////////////////////////

// FSavedMove_HoopzPawn::FSavedMove_HoopzPawn()
// {
// 	AccelMagThreshold = 1.f;
// 	AccelDotThreshold = 0.9f;
// 	AccelDotThresholdCombine = 0.996f; // approx 5 degrees.
// 	MaxSpeedThresholdCombine = 10.0f;
// }

// FSavedMove_HoopzPawn::~FSavedMove_HoopzPawn()
// {
// }

// void FSavedMove_HoopzPawn::Clear()
// {
// 	bPressedJump = false;
// 	bWantsToCrouch = false;
// 	bForceMaxAccel = false;
// 	bForceNoCombine = false;
// 	bOldTimeStampBeforeReset = false;
// 	bWasJumping = false;

// 	TimeStamp = 0.f;
// 	DeltaTime = 0.f;
// 	CustomTimeDilation = 1.0f;
// 	JumpKeyHoldTime = 0.0f;
// 	JumpForceTimeRemaining = 0.0f;
// 	JumpCurrentCount = 0;
// 	JumpMaxCount = 1;
// 	MovementMode = 0; // Deprecated, keep backwards compat until removed

// 	StartPackedMovementMode = 0;
// 	StartLocation = FVector::ZeroVector;
// 	StartRelativeLocation = FVector::ZeroVector;
// 	StartVelocity = FVector::ZeroVector;
// 	// StartFloor = FFindFloorResult();
// 	StartRotation = FRotator::ZeroRotator;
// 	StartControlRotation = FRotator::ZeroRotator;
// 	StartBaseRotation = FQuat::Identity;
// 	StartCapsuleRadius = 0.f;
// 	StartCapsuleHalfHeight = 0.f;
// 	StartBase = nullptr;
// 	StartBoneName = NAME_None;
// 	StartActorOverlapCounter = 0;
// 	StartComponentOverlapCounter = 0;

// 	StartAttachParent = nullptr;
// 	StartAttachSocketName = NAME_None;
// 	StartAttachRelativeLocation = FVector::ZeroVector;
// 	StartAttachRelativeRotation = FRotator::ZeroRotator;

// 	SavedLocation = FVector::ZeroVector;
// 	SavedRotation = FRotator::ZeroRotator;
// 	SavedRelativeLocation = FVector::ZeroVector;
// 	SavedControlRotation = FRotator::ZeroRotator;
// 	Acceleration = FVector::ZeroVector;
// 	MaxSpeed = 0.0f;
// 	AccelMag = 0.0f;
// 	AccelNormal = FVector::ZeroVector;

// 	EndBase = nullptr;
// 	EndBoneName = NAME_None;
// 	EndActorOverlapCounter = 0;
// 	EndComponentOverlapCounter = 0;
// 	EndPackedMovementMode = 0;

// 	EndAttachParent = nullptr;
// 	EndAttachSocketName = NAME_None;
// 	EndAttachRelativeLocation = FVector::ZeroVector;
// 	EndAttachRelativeRotation = FRotator::ZeroRotator;

// 	RootMotionMontage = NULL;
// 	RootMotionTrackPosition = 0.f;
// 	RootMotionMovement.Clear();

// 	SavedRootMotion.Clear();
// }

// void FSavedMove_HoopzPawn::SetMoveFor(AHoopzPlayer* Character, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_HoopzPawn & ClientData)
// {
// 	HoopzPlayer = Character;
// 	DeltaTime = InDeltaTime;
	
// 	SetInitialPosition(Character);

// 	AccelMag = NewAccel.Size();
// 	AccelNormal = (AccelMag > SMALL_NUMBER ? NewAccel / AccelMag : FVector::ZeroVector);
	
// 	// Round value, so that client and server match exactly (and so we can send with less bandwidth). This rounded value is copied back to the client in ReplicateMoveToServer.
// 	// This is done after the AccelMag and AccelNormal are computed above, because those are only used client-side for combining move logic and need to remain accurate.
// 	Acceleration = Character->GetPawnMovement()->RoundAcceleration(NewAccel);
	
// 	MaxSpeed = Character->GetPawnMovement()->GetMaxSpeed();

// 	// CheckJumpInput will increment JumpCurrentCount.
// 	// Therefore, for replicated moves we want it to set it at 1 less to properly
// 	// handle the change.
// 	JumpCurrentCount = Character->JumpCurrentCount > 0 ? Character->JumpCurrentCount - 1 : 0;
// 	bWantsToCrouch = Character->GetPawnMovement()->bWantsToCrouch;
// 	bForceMaxAccel = Character->GetPawnMovement()->bForceMaxAccel;
// 	StartPackedMovementMode = Character->GetPawnMovement()->PackNetworkMovementMode();
// 	MovementMode = StartPackedMovementMode; // Deprecated, keep backwards compat until removed

// 	// Root motion source-containing moves should never be combined
// 	// Main discovered issue being a move without root motion combining with
// 	// a move with it will cause the DeltaTime for that next move to be larger than
// 	// intended (effectively root motion applies to movement that happened prior to its activation)
// 	if (Character->GetPawnMovement()->CurrentRootMotion.HasActiveRootMotionSources())
// 	{
// 		bForceNoCombine = true;
// 	}

// 	// Launch velocity gives instant and potentially huge change of velocity
// 	// Avoid combining move to prevent from reverting locations until server catches up
// 	const bool bHasLaunchVelocity = !Character->GetPawnMovement()->PendingLaunchVelocity.IsZero();
// 	if (bHasLaunchVelocity)
// 	{
// 		bForceNoCombine = true;
// 	}

// 	TimeStamp = ClientData.CurrentTimeStamp;
// }

// void FSavedMove_HoopzPawn::SetInitialPosition(AHoopzPlayer* Character)
// {
// 	StartLocation = Character->GetActorLocation();
// 	StartRotation = Character->GetActorRotation();
// 	StartVelocity = Character->GetPawnMovement()->Velocity;
// 	UPrimitiveComponent* const MovementBase = Character->GetMovementBase();
// 	StartBase = MovementBase;
// 	StartBaseRotation = FQuat::Identity;
// 	StartFloor = Character->GetPawnMovement()->CurrentFloor;
// 	CustomTimeDilation = Character->CustomTimeDilation;
// 	StartBoneName = Character->GetBasedMovement().BoneName;
// 	StartActorOverlapCounter = Character->NumActorOverlapEventsCounter;
// 	StartComponentOverlapCounter = UPrimitiveComponent::GlobalOverlapEventsCounter;

// 	if (MovementBaseUtility::UseRelativeLocation(MovementBase))
// 	{
// 		StartRelativeLocation = Character->GetBasedMovement().Location;
// 		FVector StartBaseLocation_Unused;
// 		MovementBaseUtility::GetMovementBaseTransform(MovementBase, StartBoneName, StartBaseLocation_Unused, StartBaseRotation);
// 	}

// 	// Attachment state
// 	if (const USceneComponent* UpdatedComponent = Character->GetPawnMovement()->UpdatedComponent)  // TODO : Set Updated Component
// 	{
// 		StartAttachParent = UpdatedComponent->GetAttachParent();
// 		StartAttachSocketName = UpdatedComponent->GetAttachSocketName();
// 		StartAttachRelativeLocation = UpdatedComponent->RelativeLocation;
// 		StartAttachRelativeRotation = UpdatedComponent->RelativeRotation;
// 	}

// 	StartControlRotation = Character->GetControlRotation().Clamp();
// 	Character->GetCapsuleComponent()->GetScaledCapsuleSize(StartCapsuleRadius, StartCapsuleHalfHeight);

// 	// Jump state
// 	bPressedJump = Character->bPressedJump;
// 	bWasJumping = Character->bWasJumping;
// 	JumpKeyHoldTime = Character->JumpKeyHoldTime;
// 	JumpForceTimeRemaining = Character->JumpForceTimeRemaining;
// 	JumpMaxCount = Character->JumpMaxCount;
// }

// bool FSavedMove_HoopzPawn::IsImportantMove(const FSavedMovePtr& LastAckedMovePtr) const
// {
// 	const FSavedMove_HoopzPawn* LastAckedMove = LastAckedMovePtr.Get();

// 	// Check if any important movement flags have changed status.
// 	if (GetCompressedFlags() != LastAckedMove->GetCompressedFlags())
// 	{
// 		return true;
// 	}

// 	if (StartPackedMovementMode != LastAckedMove->EndPackedMovementMode)
// 	{
// 		return true;
// 	}

// 	if (EndPackedMovementMode != LastAckedMove->EndPackedMovementMode)
// 	{
// 		return true;
// 	}

// 	// check if acceleration has changed significantly
// 	if (Acceleration != LastAckedMove->Acceleration)
// 	{
// 		// Compare magnitude and orientation
// 		if( (FMath::Abs(AccelMag - LastAckedMove->AccelMag) > AccelMagThreshold) || ((AccelNormal | LastAckedMove->AccelNormal) < AccelDotThreshold) )
// 		{
// 			return true;
// 		}
// 	}
// 	return false;
// }

// FVector FSavedMove_HoopzPawn::GetRevertedLocation() const
// {
// 	if (const USceneComponent* AttachParent = StartAttachParent.Get())
// 	{
// 		return AttachParent->GetSocketTransform(StartAttachSocketName).TransformPosition(StartAttachRelativeLocation);
// 	}

// 	const UPrimitiveComponent* MovementBase = StartBase.Get();
// 	if (MovementBaseUtility::UseRelativeLocation(MovementBase))
// 	{
// 		FVector BaseLocation; FQuat BaseRotation;
// 		MovementBaseUtility::GetMovementBaseTransform(MovementBase, StartBoneName, BaseLocation, BaseRotation);
// 		return BaseLocation + StartRelativeLocation;
// 	}

// 	return StartLocation;
// }

// void FSavedMove_HoopzPawn::PostUpdate(AHoopzPlayer* Character, FSavedMove_HoopzPawn::EPostUpdateMode PostUpdateMode)
// {
// 	// Common code for both recording and after a replay.
// 	{
// 		EndPackedMovementMode = Character->GetPawnMovement()->PackNetworkMovementMode();
// 		MovementMode = EndPackedMovementMode; // Deprecated, keep backwards compat until removed
// 		SavedLocation = Character->GetActorLocation();
// 		SavedRotation = Character->GetActorRotation();
// 		SavedVelocity = Character->GetVelocity();
// #if ENABLE_NAN_DIAGNOSTIC
// 		const float WarnVelocitySqr = 20000.f * 20000.f;
// 		if (SavedVelocity.SizeSquared() > WarnVelocitySqr)
// 		{
// 			if (Character->SavedRootMotion.HasActiveRootMotionSources())
// 			{
// 				UE_LOG(LogCharacterMovement, Log, TEXT("FSavedMove_Character::PostUpdate detected very high Velocity! (%s), but with active root motion sources (could be intentional)"), *SavedVelocity.ToString());
// 			}
// 			else
// 			{
// 				UE_LOG(LogCharacterMovement, Warning, TEXT("FSavedMove_Character::PostUpdate detected very high Velocity! (%s)"), *SavedVelocity.ToString());
// 			}
// 		}
// #endif
// 		UPrimitiveComponent* const MovementBase = Character->GetMovementBase();
// 		EndBase = MovementBase;
// 		EndBoneName = Character->GetBasedMovement().BoneName;
// 		if (MovementBaseUtility::UseRelativeLocation(MovementBase))
// 		{
// 			SavedRelativeLocation = Character->GetBasedMovement().Location;
// 		}

// 		// Attachment state
// 		if (const USceneComponent* UpdatedComponent = Character->GetPawnMovement()->UpdatedComponent)   // TODO : Set UpdatedComponent
// 		{
// 			EndAttachParent = UpdatedComponent->GetAttachParent();
// 			EndAttachSocketName = UpdatedComponent->GetAttachSocketName();
// 			EndAttachRelativeLocation = UpdatedComponent->RelativeLocation;
// 			EndAttachRelativeRotation = UpdatedComponent->RelativeRotation;
// 		}

// 		SavedControlRotation = Character->GetControlRotation().Clamp();
// 	}

// 	// Only save RootMotion params when initially recording
// 	if (PostUpdateMode == PostUpdate_Record)
// 	{
// 		const FAnimMontageInstance* RootMotionMontageInstance = Character->GetRootMotionAnimMontageInstance();
// 		if (RootMotionMontageInstance && !RootMotionMontageInstance->IsRootMotionDisabled())
// 		{
// 			RootMotionMontage = RootMotionMontageInstance->Montage;
// 			RootMotionTrackPosition = RootMotionMontageInstance->GetPosition();
// 			RootMotionMovement = Character->ClientRootMotionParams;
// 		}

// 		// Save off Root Motion Sources
// 		if( Character->SavedRootMotion.HasActiveRootMotionSources() )
// 		{
// 			SavedRootMotion = Character->SavedRootMotion;
// 		}

// 		// Don't want to combine moves that trigger overlaps, because by moving back and replaying the move we could retrigger overlaps.
// 		EndActorOverlapCounter = Character->NumActorOverlapEventsCounter;
// 		EndComponentOverlapCounter = UPrimitiveComponent::GlobalOverlapEventsCounter;
// 		if ((StartActorOverlapCounter != EndActorOverlapCounter) || (StartComponentOverlapCounter != EndComponentOverlapCounter))
// 		{
// 			bForceNoCombine = true;
// 		}

// 		// Don't combine or delay moves where velocity changes to/from zero.
// 		if (StartVelocity.IsZero() != SavedVelocity.IsZero())
// 		{
// 			bForceNoCombine = true;
// 		}
// 	}
// 	else if (PostUpdateMode == PostUpdate_Replay)
// 	{
// 		if( Character->bClientResimulateRootMotionSources )
// 		{
// 			// When replaying moves, the next move should use the results of this move
// 			// so that future replayed moves account for the server correction
// 			Character->SavedRootMotion = Character->GetPawnMovement()->CurrentRootMotion;
// 		}
// 	}
// }

// // CVars
// namespace CharacterMovementCVars
// {
// 	// Listen server smoothing
// 	static int32 NetEnableListenServerSmoothing = 1;
// 	FAutoConsoleVariableRef CVarNetEnableListenServerSmoothing(
// 		TEXT("p.NetEnableListenServerSmoothing"),
// 		NetEnableListenServerSmoothing,
// 		TEXT("Whether to enable mesh smoothing on listen servers for the local view of remote clients.\n")
// 		TEXT("0: Disable, 1: Enable"),
// 		ECVF_Default);

// 	// Latent proxy prediction
// 	static int32 NetEnableSkipProxyPredictionOnNetUpdate = 1;
// 	FAutoConsoleVariableRef CVarNetEnableSkipProxyPredictionOnNetUpdate(
// 		TEXT("p.NetEnableSkipProxyPredictionOnNetUpdate"),
// 		NetEnableSkipProxyPredictionOnNetUpdate,
// 		TEXT("Whether to allow proxies to skip prediction on frames with a network position update, if bNetworkSkipProxyPredictionOnNetUpdate is also true on the movement component.\n")
// 		TEXT("0: Disable, 1: Enable"),
// 		ECVF_Default);

// 	// Logging when character is stuck. Off by default in shipping.
// #if UE_BUILD_SHIPPING
// 	static float StuckWarningPeriod = -1.f;
// #else
// 	static float StuckWarningPeriod = 1.f;
// #endif

// 	FAutoConsoleVariableRef CVarStuckWarningPeriod(
// 		TEXT("p.CharacterStuckWarningPeriod"),
// 		StuckWarningPeriod,
// 		TEXT("How often (in seconds) we are allowed to log a message about being stuck in geometry.\n")
// 		TEXT("<0: Disable, >=0: Enable and log this often, in seconds."),
// 		ECVF_Default);

// 	static int32 NetEnableMoveCombining = 1;
// 	FAutoConsoleVariableRef CVarNetEnableMoveCombining(
// 		TEXT("p.NetEnableMoveCombining"),
// 		NetEnableMoveCombining,
// 		TEXT("Whether to enable move combining on the client to reduce bandwidth by combining similar moves.\n")
// 		TEXT("0: Disable, 1: Enable"),
// 		ECVF_Default);

// 	static int32 NetEnableMoveCombiningOnStaticBaseChange = 1;
// 	FAutoConsoleVariableRef CVarNetEnableMoveCombiningOnStaticBaseChange(
// 		TEXT("p.NetEnableMoveCombiningOnStaticBaseChange"),
// 		NetEnableMoveCombiningOnStaticBaseChange,
// 		TEXT("Whether to allow combining client moves when moving between static geometry.\n")
// 		TEXT("0: Disable, 1: Enable"),
// 		ECVF_Default);

// 	static float NetMoveCombiningAttachedLocationTolerance = 0.01f;
// 	FAutoConsoleVariableRef CVarNetMoveCombiningAttachedLocationTolerance(
// 		TEXT("p.NetMoveCombiningAttachedLocationTolerance"),
// 		NetMoveCombiningAttachedLocationTolerance,
// 		TEXT("Tolerance for relative location attachment change when combining moves. Small tolerances allow for very slight jitter due to transform updates."),
// 		ECVF_Default);

// 	static float NetMoveCombiningAttachedRotationTolerance = 0.01f;
// 	FAutoConsoleVariableRef CVarNetMoveCombiningAttachedRotationTolerance(
// 		TEXT("p.NetMoveCombiningAttachedRotationTolerance"),
// 		NetMoveCombiningAttachedRotationTolerance,
// 		TEXT("Tolerance for relative rotation attachment change when combining moves. Small tolerances allow for very slight jitter due to transform updates."),
// 		ECVF_Default);

// 	static float NetStationaryRotationTolerance = 0.1f;
// 	FAutoConsoleVariableRef CVarNetStationaryRotationTolerance(
// 		TEXT("p.NetStationaryRotationTolerance"),
// 		NetStationaryRotationTolerance,
// 		TEXT("Tolerance for GetClientNetSendDeltaTime() to remain throttled when small control rotation changes occur."),
// 		ECVF_Default);

// 	static int32 NetUseClientTimestampForReplicatedTransform = 1;
// 	FAutoConsoleVariableRef CVarNetUseClientTimestampForReplicatedTransform(
// 		TEXT("p.NetUseClientTimestampForReplicatedTransform"),
// 		NetUseClientTimestampForReplicatedTransform,
// 		TEXT("If enabled, use client timestamp changes to track the replicated transform timestamp, otherwise uses server tick time as the timestamp.\n")
// 		TEXT("Game session usually needs to be restarted if this is changed at runtime.\n")
// 		TEXT("0: Disable, 1: Enable"),
// 		ECVF_Default);

// 	static int32 ReplayUseInterpolation = 0;
// 	FAutoConsoleVariableRef CVarReplayUseInterpolation(
// 		TEXT( "p.ReplayUseInterpolation" ),
// 		ReplayUseInterpolation,
// 		TEXT( "" ),
// 		ECVF_Default);

// 	static int32 ReplayLerpAcceleration = 0;
// 	FAutoConsoleVariableRef CVarReplayLerpAcceleration(
// 		TEXT( "p.ReplayLerpAcceleration" ),
// 		ReplayLerpAcceleration,
// 		TEXT( "" ),
// 		ECVF_Default);

// 	static int32 FixReplayOverSampling = 1;
// 	FAutoConsoleVariableRef CVarFixReplayOverSampling(
// 		TEXT( "p.FixReplayOverSampling" ),
// 		FixReplayOverSampling,
// 		TEXT( "If 1, remove invalid replay samples that can occur due to oversampling (sampling at higher rate than physics is being ticked)" ),
// 		ECVF_Default);

// 	static int32 ForceJumpPeakSubstep = 1;
// 	FAutoConsoleVariableRef CVarForceJumpPeakSubstep(
// 		TEXT("p.ForceJumpPeakSubstep"),
// 		ForceJumpPeakSubstep,
// 		TEXT("If 1, force a jump substep to always reach the peak position of a jump, which can often be cut off as framerate lowers."),
// 		ECVF_Default);

// #if !UE_BUILD_SHIPPING

// 	int32 NetShowCorrections = 0;
// 	FAutoConsoleVariableRef CVarNetShowCorrections(
// 		TEXT("p.NetShowCorrections"),
// 		NetShowCorrections,
// 		TEXT("Whether to draw client position corrections (red is incorrect, green is corrected).\n")
// 		TEXT("0: Disable, 1: Enable"),
// 		ECVF_Cheat);

// 	float NetCorrectionLifetime = 4.f;
// 	FAutoConsoleVariableRef CVarNetCorrectionLifetime(
// 		TEXT("p.NetCorrectionLifetime"),
// 		NetCorrectionLifetime,
// 		TEXT("How long a visualized network correction persists.\n")
// 		TEXT("Time in seconds each visualized network correction persists."),
// 		ECVF_Cheat);

// #endif // !UI_BUILD_SHIPPING


// #if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)

// 	static float NetForceClientAdjustmentPercent = 0.f;
// 	FAutoConsoleVariableRef CVarNetForceClientAdjustmentPercent(
// 		TEXT("p.NetForceClientAdjustmentPercent"),
// 		NetForceClientAdjustmentPercent,
// 		TEXT("Percent of ServerCheckClientError checks to return true regardless of actual error.\n")
// 		TEXT("Useful for testing client correction code.\n")
// 		TEXT("<=0: Disable, 0.05: 5% of checks will return failed, 1.0: Always send client adjustments"),
// 		ECVF_Cheat);

// 	static float NetForceClientServerMoveLossPercent = 0.f;
// 	FAutoConsoleVariableRef CVarNetForceClientServerMoveLossPercent(
// 		TEXT("p.NetForceClientServerMoveLossPercent"),
// 		NetForceClientServerMoveLossPercent,
// 		TEXT("Percent of ServerMove calls for client to not send.\n")
// 		TEXT("Useful for testing server force correction code.\n")
// 		TEXT("<=0: Disable, 0.05: 5% of checks will return failed, 1.0: never send server moves"),
// 		ECVF_Cheat);

// 	static float NetForceClientServerMoveLossDuration = 0.f;
// 	FAutoConsoleVariableRef CVarNetForceClientServerMoveLossDuration(
// 		TEXT("p.NetForceClientServerMoveLossDuration"),
// 		NetForceClientServerMoveLossDuration,
// 		TEXT("Duration in seconds for client to drop ServerMove calls when NetForceClientServerMoveLossPercent check passes.\n")
// 		TEXT("Useful for testing server force correction code.\n")
// 		TEXT("Duration of zero means single frame loss."),
// 		ECVF_Cheat);

// 	static int32 VisualizeMovement = 0;
// 	FAutoConsoleVariableRef CVarVisualizeMovement(
// 		TEXT("p.VisualizeMovement"),
// 		VisualizeMovement,
// 		TEXT("Whether to draw in-world debug information for character movement.\n")
// 		TEXT("0: Disable, 1: Enable"),
// 		ECVF_Cheat);

// 	static int32 NetVisualizeSimulatedCorrections = 0;
// 	FAutoConsoleVariableRef CVarNetVisualizeSimulatedCorrections(
// 		TEXT("p.NetVisualizeSimulatedCorrections"),
// 		NetVisualizeSimulatedCorrections,
// 		TEXT("")
// 		TEXT("0: Disable, 1: Enable"),
// 		ECVF_Cheat);

// 	static int32 DebugTimeDiscrepancy = 0;
// 	FAutoConsoleVariableRef CVarDebugTimeDiscrepancy(
// 		TEXT("p.DebugTimeDiscrepancy"),
// 		DebugTimeDiscrepancy,
// 		TEXT("Whether to log detailed Movement Time Discrepancy values for testing")
// 		TEXT("0: Disable, 1: Enable Detection logging, 2: Enable Detection and Resolution logging"),
// 		ECVF_Cheat);
// #endif // !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
// }


// bool FSavedMove_HoopzPawn::CanCombineWith(const FSavedMovePtr& NewMovePtr, AHoopzPlayer* Character, float MaxDelta) const
// {
// 	const FSavedMove_HoopzPawn* NewMove = NewMovePtr.Get();

// 	if (bForceNoCombine || NewMove->bForceNoCombine)
// 	{
// 		return false;
// 	}

// 	if (bOldTimeStampBeforeReset)
// 	{
// 		return false;
// 	}

// 	// Cannot combine moves which contain root motion for now.
// 	// @fixme laurent - we should be able to combine most of them though, but current scheme of resetting pawn location and resimulating forward doesn't work.
// 	// as we don't want to tick montage twice (so we don't fire events twice). So we need to rearchitecture this so we tick only the second part of the move, and reuse the first part.
// 	if( (RootMotionMontage != NULL) || (NewMove->RootMotionMontage != NULL) )
// 	{
// 		return false;
// 	}

// 	if (NewMove->Acceleration.IsZero())
// 	{
// 		if (!Acceleration.IsZero())
// 		{
// 			return false;
// 		}
// 	}
// 	else
// 	{
// 		if (NewMove->DeltaTime + DeltaTime >= MaxDelta)
// 		{
// 			return false;
// 		}

// 		if (!FVector::Coincident(AccelNormal, NewMove->AccelNormal, AccelDotThresholdCombine))
// 		{
// 			return false;
// 		}	
// 	}

// 	// Don't combine moves where velocity changes to zero or from zero.
// 	if (StartVelocity.IsZero() != NewMove->StartVelocity.IsZero())
// 	{
// 		return false;
// 	}

// 	if (!FMath::IsNearlyEqual(MaxSpeed, NewMove->MaxSpeed, MaxSpeedThresholdCombine))
// 	{
// 		return false;
// 	}

// 	if ((MaxSpeed == 0.0f) != (NewMove->MaxSpeed == 0.0f))
// 	{
// 		return false;
// 	}

// 	// Don't combine on changes to/from zero JumpKeyHoldTime.
// 	if ((JumpKeyHoldTime == 0.f) != (NewMove->JumpKeyHoldTime == 0.f))
// 	{
// 		return false;
// 	}

// 	if ((bWasJumping != NewMove->bWasJumping) || (JumpCurrentCount != NewMove->JumpCurrentCount) || (JumpMaxCount != NewMove->JumpMaxCount))
// 	{
// 		return false;
// 	}
	
// 	// Don't combine on changes to/from zero.
// 	if ((JumpForceTimeRemaining == 0.f) != (NewMove->JumpForceTimeRemaining == 0.f))
// 	{
// 		return false;
// 	}
	
// 	// Compressed flags not equal, can't combine. This covers jump and crouch as well as any custom movement flags from overrides.
// 	if (GetCompressedFlags() != NewMove->GetCompressedFlags())
// 	{
// 		return false;
// 	}

// 	const UPrimitiveComponent* OldBasePtr = StartBase.Get();
// 	const UPrimitiveComponent* NewBasePtr = NewMove->StartBase.Get();
// 	const bool bDynamicBaseOld = MovementBaseUtility::IsDynamicBase(OldBasePtr);
// 	const bool bDynamicBaseNew = MovementBaseUtility::IsDynamicBase(NewBasePtr);

// 	// Change between static/dynamic requires separate moves (position sent as world vs relative)
// 	if (bDynamicBaseOld != bDynamicBaseNew)
// 	{
// 		return false;
// 	}

// 	// Only need to prevent combining when on a dynamic base that changes (unless forced off via CVar). Again, because relative location can change.
// 	const bool bPreventOnStaticBaseChange = (CharacterMovementCVars::NetEnableMoveCombiningOnStaticBaseChange == 0);
// 	if (bPreventOnStaticBaseChange || (bDynamicBaseOld || bDynamicBaseNew))
// 	{
// 		if (OldBasePtr != NewBasePtr)
// 		{
// 			return false;
// 		}

// 		if (StartBoneName != NewMove->StartBoneName)
// 		{
// 			return false;
// 		}
// 	}

// 	if (EndPackedMovementMode != NewMove->StartPackedMovementMode)
// 	{
// 		return false;
// 	}

// 	if (StartCapsuleRadius != NewMove->StartCapsuleRadius)
// 	{
// 		return false;
// 	}

// 	if (StartCapsuleHalfHeight != NewMove->StartCapsuleHalfHeight)
// 	{
// 		return false;
// 	}

// 	// No combining if attach parent changed.
// 	const USceneComponent* OldStartAttachParent = StartAttachParent.Get();
// 	const USceneComponent* OldEndAttachParent = EndAttachParent.Get();
// 	const USceneComponent* NewStartAttachParent = NewMove->StartAttachParent.Get();
// 	if (OldStartAttachParent != NewStartAttachParent || OldEndAttachParent != NewStartAttachParent)
// 	{
// 		return false;
// 	}

// 	// No combining if attach socket changed.
// 	if (StartAttachSocketName != NewMove->StartAttachSocketName || EndAttachSocketName != NewMove->StartAttachSocketName)
// 	{
// 		return false;
// 	}

// 	if (NewStartAttachParent != nullptr)
// 	{
// 		// If attached, no combining if relative location changed.
// 		const FVector RelativeLocationDelta = (StartAttachRelativeLocation - NewMove->StartAttachRelativeLocation);
// 		if (!RelativeLocationDelta.IsNearlyZero(CharacterMovementCVars::NetMoveCombiningAttachedLocationTolerance))
// 		{
// 			//UE_LOG(LogCharacterMovement, Warning, TEXT("NoCombine: DeltaLocation(%s)"), *RelativeLocationDelta.ToString());
// 			return false;
// 		}
// 		// For rotation, Yaw doesn't matter for capsules
// 		FRotator RelativeRotationDelta = StartAttachRelativeRotation - NewMove->StartAttachRelativeRotation;
// 		RelativeRotationDelta.Yaw = 0.0f;
// 		if (!RelativeRotationDelta.IsNearlyZero(CharacterMovementCVars::NetMoveCombiningAttachedRotationTolerance))
// 		{
// 			return false;
// 		}
// 	}
// 	else
// 	{
// 		// Not attached to anything. Only combine if base hasn't rotated.
// 		if (!StartBaseRotation.Equals(NewMove->StartBaseRotation))
// 		{
// 			return false;
// 		}
// 	}

// 	if (CustomTimeDilation != NewMove->CustomTimeDilation)
// 	{
// 		return false;
// 	}

// 	// Don't combine moves with overlap event changes, since reverting back and then moving forward again can cause event spam.
// 	// This catches events between movement updates; moves that trigger events already set bForceNoCombine to false.
// 	if (EndActorOverlapCounter != NewMove->StartActorOverlapCounter)
// 	{
// 		return false;
// 	}

// 	return true;
// }

// void FSavedMove_HoopzPawn::CombineWith(const FSavedMove_HoopzPawn* OldMove, AHoopzPlayer* InCharacter, APlayerController* PC, const FVector& OldStartLocation)
// {
// 	UHoopzPlayerMovementComponent* CharMovement = InCharacter->GetPawnMovement();

// 	// to combine move, first revert pawn position to PendingMove start position, before playing combined move on client
// 	if (const USceneComponent* AttachParent = StartAttachParent.Get())
// 	{
// 		CharMovement->UpdatedComponent->SetRelativeLocationAndRotation(StartAttachRelativeLocation, StartAttachRelativeRotation, false, nullptr, CharMovement->GetTeleportType());
// 	}
// 	else
// 	{
// 		CharMovement->UpdatedComponent->SetWorldLocationAndRotation(OldStartLocation, OldMove->StartRotation, false, nullptr, CharMovement->GetTeleportType());
// 	}
	
// 	CharMovement->Velocity = OldMove->StartVelocity;

// 	CharMovement->SetBase(OldMove->StartBase.Get(), OldMove->StartBoneName);
// 	CharMovement->CurrentFloor = OldMove->StartFloor;

// 	// Now that we have reverted to the old position, prepare a new move from that position,
// 	// using our current velocity, acceleration, and rotation, but applied over the combined time from the old and new move.

// 	// Combine times for both moves
// 	DeltaTime += OldMove->DeltaTime;

// 	// Roll back jump force counters. SetInitialPosition() below will copy them to the saved move.
// 	// Changes in certain counters like JumpCurrentCount don't allow move combining, so no need to roll those back (they are the same).
// 	InCharacter->JumpForceTimeRemaining = OldMove->JumpForceTimeRemaining;
// 	InCharacter->JumpKeyHoldTime = OldMove->JumpKeyHoldTime;
// }
// 