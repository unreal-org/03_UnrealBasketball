// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Animation/AnimMontage.h"

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "HoopzPlayer.generated.h"

class UPlayerCapsuleComponent;   // Root Component
class UHoopzPlayerMovementComponent;

UCLASS()
class UNREALBASKETBALL_API AHoopzPlayer : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AHoopzPlayer();

private:
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
};

// 	/** Returns CharacterMovement subobject **/
// 	FORCEINLINE class UHoopzPlayerMovementComponent* GetPawnMovement() const { return PawnMovement; }

// 	/** Returns CapsuleComponent subobject **/
// 	FORCEINLINE class UPlayerCapsuleComponent* GetCapsuleComponent() const { return PlayerCapsuleComponent; }

// 	/**
//      * Tracks the current number of jumps performed.
//      * This is incremented in CheckJumpInput, used in CanJump_Implementation, and reset in OnMovementModeChanged.
//      * When providing overrides for these methods, it's recommended to either manually
//      * increment / reset this value, or call the Super:: method.
//      */
//     UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category=Character)
//     int32 JumpCurrentCount;

// 	/** Accessor for BasedMovement */
// 	FORCEINLINE const FBasedMovementInfo& GetBasedMovement() const { return BasedMovement; }

// 	/** Incremented every time there is an Actor overlap event (start or stop) on this actor. */
// 	uint32 NumActorOverlapEventsCounter;

// 	/** When true, player wants to jump */
// 	UPROPERTY(BlueprintReadOnly, Category=Character)
// 	uint32 bPressedJump:1;

// 	/** Tracks whether or not the character was already jumping last frame. */
// 	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Transient, Category=Character)
// 	uint32 bWasJumping : 1;

// 	/** 
// 	 * Jump key Held Time.
// 	 * This is the time that the player has held the jump key, in seconds.
// 	 */
// 	UPROPERTY(Transient, BlueprintReadOnly, VisibleInstanceOnly, Category=Character)
// 	float JumpKeyHoldTime;

// 	/** Amount of jump force time remaining, if JumpMaxHoldTime > 0. */
// 	UPROPERTY(Transient, BlueprintReadOnly, VisibleInstanceOnly, Category=Character)
// 	float JumpForceTimeRemaining;

// 	/**
//      * The max number of jumps the character can perform.
//      * Note that if JumpMaxHoldTime is non zero and StopJumping is not called, the player
//      * may be able to perform and unlimited number of jumps. Therefore it is usually
//      * best to call StopJumping() when jump input has ceased (such as a button up event).
//      */
//     UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category=Character)
//     int32 JumpMaxCount;

// 	/** 
// 	 * For LocallyControlled Autonomous clients. 
// 	 * During a PerformMovement() after root motion is prepared, we save it off into this and
// 	 * then record it into our SavedMoves.
// 	 * During SavedMove playback we use it as our "Previous Move" SavedRootMotion which includes
// 	 * last received root motion from the Server
// 	 */
// 	UPROPERTY(Transient)
// 	FRootMotionSourceGroup SavedRootMotion;

// 	/** If server disagrees with root motion state, client has to resimulate root motion from last AckedMove. */
// 	UPROPERTY(Transient)
// 	uint32 bClientResimulateRootMotionSources:1;

// 	/** For LocallyControlled Autonomous clients. Saved root motion data to be used by SavedMoves. */
// 	UPROPERTY(Transient)
// 	FRootMotionMovementParams ClientRootMotionParams;

// 	/** Get FAnimMontageInstance playing RootMotion */
// 	FAnimMontageInstance * GetRootMotionAnimMontageInstance() const;

// 	/** Sets the component the Character is walking on, used by CharacterMovement walking movement to be able to follow dynamic objects. */
// 	virtual void SetBase(UPrimitiveComponent* NewBase, const FName BoneName = NAME_None, bool bNotifyActor=true);

// 	/** Flag that we are receiving replication of the based movement. */
// 	UPROPERTY()
// 	bool bInBaseReplication;

// 	/** Save a new relative location in BasedMovement and a new rotation with is either relative or absolute. */
// 	void SaveRelativeBasedMovement(const FVector& NewRelativeLocation, const FRotator& NewRotation, bool bRelativeRotation);

// private:
// 	/** Movement component used for movement logic in various movement modes (walking, falling, etc), containing relevant settings and functions to control movement. */
// 	UPROPERTY(Category=Character, VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
// 	UHoopzPlayerMovementComponent* PawnMovement;

// 	/** Movement component used for movement logic in various movement modes (walking, falling, etc), containing relevant settings and functions to control movement. */
// 	UPROPERTY(Category=Character, VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
// 	UHoopzPlayerMovementComponent* PawnMovement;

// 	UPlayerCapsuleComponent* PlayerCapsuleComponent = nullptr;

// protected:
// 	// Called when the game starts or when spawned
// 	virtual void BeginPlay() override;

// 	// Called every frame
// 	virtual void Tick(float DeltaTime) override;

// 	// Called to bind functionality to input
// 	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
// 	/** Info about our current movement base (object we are standing on). */
// 	UPROPERTY()
// 	struct FBasedMovementInfo BasedMovement;

// 	/** Event called after actor's base changes (if SetBase was requested to notify us with bNotifyPawn). */
// 	virtual void BaseChange();
// };

// /** MovementBaseUtility provides utilities for working with movement bases, for which we may need relative positioning info. */
// namespace MovementBaseUtility
// {
// 	/** Determine whether MovementBase can possibly move. */
// 	ENGINE_API bool IsDynamicBase(const UPrimitiveComponent* MovementBase);

// 	/** Determine if we should use relative positioning when based on a component (because it may move). */
// 	FORCEINLINE bool UseRelativeLocation(const UPrimitiveComponent* MovementBase)
// 	{
// 		return IsDynamicBase(MovementBase);
// 	}

// 	/** Ensure that BasedObjectTick ticks after NewBase */
// 	ENGINE_API void AddTickDependency(FTickFunction& BasedObjectTick, UPrimitiveComponent* NewBase);

// 	/** Remove tick dependency of BasedObjectTick on OldBase */
// 	ENGINE_API void RemoveTickDependency(FTickFunction& BasedObjectTick, UPrimitiveComponent* OldBase);

// 	/** Get the velocity of the given component, first checking the ComponentVelocity and falling back to the physics velocity if necessary. */
// 	ENGINE_API FVector GetMovementBaseVelocity(const UPrimitiveComponent* MovementBase, const FName BoneName);

// 	/** Get the tangential velocity at WorldLocation for the given component. */
// 	ENGINE_API FVector GetMovementBaseTangentialVelocity(const UPrimitiveComponent* MovementBase, const FName BoneName, const FVector& WorldLocation);

// 	/** Get the transforms for the given MovementBase, optionally at the location of a bone. Returns false if MovementBase is nullptr, or if BoneName is not a valid bone. */
// 	ENGINE_API bool GetMovementBaseTransform(const UPrimitiveComponent* MovementBase, const FName BoneName, FVector& OutLocation, FQuat& OutQuat);
// }


// /** Struct to hold information about the "base" object the character is standing on. */
// USTRUCT()
// struct FBasedMovementInfo
// {
// 	GENERATED_USTRUCT_BODY()

// 	/** Component we are based on */
// 	UPROPERTY()
// 	UPrimitiveComponent* MovementBase;

// 	/** Bone name on component, for skeletal meshes. NAME_None if not a skeletal mesh or if bone is invalid. */
// 	UPROPERTY()
// 	FName BoneName;

// 	/** Location relative to MovementBase. Only valid if HasRelativeLocation() is true. */
// 	UPROPERTY()
// 	FVector_NetQuantize100 Location;

// 	/** Rotation: relative to MovementBase if HasRelativeRotation() is true, absolute otherwise. */
// 	UPROPERTY()
// 	FRotator Rotation;

// 	/** Whether the server says that there is a base. On clients, the component may not have resolved yet. */
// 	UPROPERTY()
// 	bool bServerHasBaseComponent;

// 	/** Whether rotation is relative to the base or absolute. It can only be relative if location is also relative. */
// 	UPROPERTY()
// 	bool bRelativeRotation;

// 	/** Whether there is a velocity on the server. Used for forcing replication when velocity goes to zero. */
// 	UPROPERTY()
// 	bool bServerHasVelocity;

// 	/** Is location relative? */
// 	FORCEINLINE bool HasRelativeLocation() const
// 	{
// 		return MovementBaseUtility::UseRelativeLocation(MovementBase);
// 	}

// 	/** Is rotation relative or absolute? It can only be relative if location is also relative. */
// 	FORCEINLINE bool HasRelativeRotation() const
// 	{
// 		return bRelativeRotation && HasRelativeLocation();
// 	}

// 	/** Return true if the client should have MovementBase, but it hasn't replicated (possibly component has not streamed in). */
// 	FORCEINLINE bool IsBaseUnresolved() const
// 	{
// 		return (MovementBase == nullptr) && bServerHasBaseComponent;
// 	}
// };

