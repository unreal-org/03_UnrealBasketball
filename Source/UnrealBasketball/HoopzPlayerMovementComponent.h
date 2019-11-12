// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/RootMotionSource.h"
#include "Interfaces/NetworkPredictionInterface.h"

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "HoopzPlayerMovementComponent.generated.h"

class UPlayerCapsuleComponent;  // For Capsule Component Input/Movement
class USkeletalMeshComponent;
class AHoopzPlayer;

DECLARE_DELEGATE_RetVal_TwoParams(FTransform, FOnProcessRootMotion, const FTransform&, UHoopzPlayerMovementComponent*)

/**
 * Controls Hoopz Player Movement:
 * 		1. Movement
 * 		2. Jumping
 * 		3. 
 * Note: Capsule is set to simulate physics but locked in X & Y Axes.
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UNREALBASKETBALL_API UHoopzPlayerMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()
	
public:
	///// Capsule Component Setup
	UFUNCTION(BluePrintCallable, Category = "Setup")      
	void Initialize(UPlayerCapsuleComponent* PlayerCapsuleComponentToSet);
	
	///// Capsule Movement
	UFUNCTION(BluePrintCallable, Category = "Input")
	void IntendMoveForward(float Throw);
	UFUNCTION(BluePrintCallable, Category = "Input")
	void IntendMoveRight(float Throw);
	UFUNCTION(BluePrintCallable, Category = "Input")
	void IntendJumpCalled();
	UFUNCTION(BluePrintCallable, Category = "Input")
	void IntendJumpReleased();

	UFUNCTION(BluePrintCallable, Category = "Input")
	void TurnLeft();
	UFUNCTION(BluePrintCallable, Category = "Input")
	void TurnRight();

private:
	UPlayerCapsuleComponent* PlayerCapsuleComponent = nullptr;
	
	///// Timer
	float JumpCalledTime = 0;
	float JumpPressedTime = 0;

	/// Capsule Pivot
	bool HasBall = false;
	bool Pivot = false;

protected:
	//virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	/** Accessed in HasRootMotionSources() */
	// AHoopzPlayer* HoopzPlayer = nullptr; // characterowner
	// USkeletalMeshComponent* PlayerSkeletalMesh = nullptr;
};

// 	/** Pawn Root Motion 

// 	/** Root Motion Group containing active root motion sources being applied to movement */
// 	UPROPERTY(Transient)
// 	FRootMotionSourceGroup CurrentRootMotion;  // .HasActiveRootMotionSources()

// 	/** Returns true if we have Root Motion from any source to use in PerformMovement() physics. */
// 	bool HasRootMotionSources() const;

// 	/** Apply a RootMotionSource to current root motion 
// 	 *  @return LocalID for this Root Motion Source */
// 	uint16 ApplyRootMotionSource(FRootMotionSource* SourcePtr);

// 	/** Called during ApplyRootMotionSource call, useful for project-specific alerts for "something is about to be altering our movement" */
// 	virtual void OnRootMotionSourceBeingApplied(const FRootMotionSource* Source);

// 	/** Get a RootMotionSource from current root motion by name */
// 	TSharedPtr<FRootMotionSource> GetRootMotionSource(FName InstanceName);

// 	/** Get a RootMotionSource from current root motion by ID */
// 	TSharedPtr<FRootMotionSource> GetRootMotionSourceByID(uint16 RootMotionSourceID);

// 	/** Remove a RootMotionSource from current root motion by name */
// 	void RemoveRootMotionSource(FName InstanceName);

// 	/** Remove a RootMotionSource from current root motion by ID */
// 	void RemoveRootMotionSourceByID(uint16 RootMotionSourceID);

// 	/** Root Motion movement params. Holds result of anim montage root motion during PerformMovement(), and is overridden
// 	*   during autonomous move playback to force historical root motion for MoveAutonomous() calls */
// 	UPROPERTY(Transient)
// 	FRootMotionMovementParams RootMotionParams;

// 	/** Velocity extracted from RootMotionParams when there is anim root motion active. Invalid to use when HasAnimRootMotion() returns false. */
// 	UPROPERTY(Transient)
// 	FVector AnimRootMotionVelocity;

// 	/** Returns true if we have Root Motion from animation to use in PerformMovement() physics. 
// 		Not valid outside of the scope of that function. Since RootMotion is extracted and used in it. */
// 	bool HasAnimRootMotion() const { return RootMotionParams.bHasRootMotion; }

// 	// Takes component space root motion and converts it to world space
// 	FTransform ConvertLocalRootMotionToWorld(const FTransform& InLocalRootMotion);

// 	// Delegate for modifying root motion pre conversion from component space to world space.
// 	FOnProcessRootMotion ProcessRootMotionPreConvertToWorld;
	
// 	// Delegate for modifying root motion post conversion from component space to world space.
// 	FOnProcessRootMotion ProcessRootMotionPostConvertToWorld;

// 	/**
// 	 * Calculate velocity from anim root motion.
// 	 * @param RootMotionDeltaMove	Change in location from root motion.
// 	 * @param DeltaSeconds			Elapsed time
// 	 * @param CurrentVelocity		Non-root motion velocity at current time, used for components of result that may ignore root motion.
// 	 * @see ConstrainAnimRootMotionVelocity
// 	 */
// 	virtual FVector CalcAnimRootMotionVelocity(const FVector& RootMotionDeltaMove, float DeltaSeconds, const FVector& CurrentVelocity) const;

// 	/**
// 	 * Constrain components of root motion velocity that may not be appropriate given the current movement mode (e.g. when falling Z may be ignored).
// 	 */
// 	virtual FVector ConstrainAnimRootMotionVelocity(const FVector& RootMotionVelocity, const FVector& CurrentVelocity) const;

// 	/** Get prediction data for a client game. Should not be used if not running as a client. Allocates the data on demand and can be overridden to allocate a custom override if desired. Result must be a FNetworkPredictionData_Client_Character. */
// 	virtual class FNetworkPredictionData_Client* GetPredictionData_Client() const; // override?
// 	/** Get prediction data for a server game. Should not be used if not running as a server. Allocates the data on demand and can be overridden to allocate a custom override if desired. Result must be a FNetworkPredictionData_Server_Character. */
// 	virtual class FNetworkPredictionData_Server* GetPredictionData_Server() const; // override?

// 	class FNetworkPredictionData_Client_HoopzPawn* GetPredictionData_Client_HoopzPawn() const;
// 	class FNetworkPredictionData_Server_HoopzPawn* GetPredictionData_Server_HoopzPawn() const;

// 	/** Round acceleration, for better consistency and lower bandwidth in networked games. */
// 	virtual FVector RoundAcceleration(FVector InAccel) const;

// 	/** If true, try to crouch (or keep crouching) on next update. If false, try to stop crouching on next update. */
// 	UPROPERTY(Category="Character Movement (General Settings)", VisibleInstanceOnly, BlueprintReadOnly)
// 	uint8 bWantsToCrouch:1;

// 	/** Ignores size of acceleration component, and forces max acceleration to drive character at full velocity. */
// 	UPROPERTY()
// 	uint8 bForceMaxAccel:1;

// 	virtual uint8 PackNetworkMovementMode() const;

// 	/**
// 	 * Actor's current movement mode (walking, falling, etc).
// 	 *    - walking:  Walking on a surface, under the effects of friction, and able to "step up" barriers. Vertical velocity is zero.
// 	 *    - falling:  Falling under the effects of gravity, after jumping or walking off the edge of a surface.
// 	 *    - flying:   Flying, ignoring the effects of gravity.
// 	 *    - swimming: Swimming through a fluid volume, under the effects of gravity and buoyancy.
// 	 *    - custom:   User-defined custom movement mode, including many possible sub-modes.
// 	 * This is automatically replicated through the Character owner and for client-server movement functions.
// 	 * @see SetMovementMode(), CustomMovementMode
// 	 */
// 	UPROPERTY(Category="Character Movement: MovementMode", BlueprintReadOnly)
// 	TEnumAsByte<enum EMovementMode> MovementMode;

// 	/**
// 	 * Current custom sub-mode if MovementMode is set to Custom.
// 	 * This is automatically replicated through the Character owner and for client-server movement functions.
// 	 * @see SetMovementMode()
// 	 */
// 	UPROPERTY(Category="Character Movement: MovementMode", BlueprintReadOnly)
// 	uint8 CustomMovementMode;

// 	/** Temporarily holds launch velocity when pawn is to be launched so it happens at end of movement. */
// 	UPROPERTY()
// 	FVector PendingLaunchVelocity;

// 	/** Information about the floor the Character is standing on (updated only during walking movement). */
// 	UPROPERTY(Category="Character Movement: Walking", VisibleInstanceOnly, BlueprintReadOnly)
// 	FFindFloorResult CurrentFloor;

// 	/** 
// 	 * When moving the character, we should inform physics as to whether we are teleporting.
// 	 * This allows physics to avoid injecting forces into simulations from client corrections (etc.)
// 	 */
// 	ETeleportType GetTeleportType() const;

// 	/** Used by movement code to determine if a change in position is based on normal movement or a teleport. If not a teleport, velocity can be recomputed based on the change in position. */
// 	UPROPERTY(Category="Character Movement (General Settings)", Transient, VisibleInstanceOnly, BlueprintReadWrite)
// 	uint8 bJustTeleported:1;

// 	/** Flag indicating the client correction was larger than NetworkLargeClientCorrectionThreshold. */
// 	uint8 bNetworkLargeClientCorrection:1;

// 	/** Update the base of the character, which is the PrimitiveComponent we are standing on. */
// 	virtual void SetBase(UPrimitiveComponent* NewBase, const FName BoneName = NAME_None, bool bNotifyActor=true);

// 	/** Set on clients when server's movement mode is NavWalking */
// 	uint8 bIsNavWalkingOnServer : 1;

// 	/** Update OldBaseLocation and OldBaseQuat if there is a valid movement base, and store the relative location/rotation if necessary. Ignores bDeferUpdateBasedMovement and forces the update. */
// 	virtual void SaveBaseLocation();

// 	/** Return true if we have a valid CharacterOwner and UpdatedComponent. */
// 	virtual bool HasValidData() const;

// 	/**
// 	 * Whether the character ignores changes in rotation of the base it is standing on.
// 	 * If true, the character maintains current world rotation.
// 	 * If false, the character rotates with the moving base.
// 	 */
// 	UPROPERTY(Category="Character Movement: Walking", EditAnywhere, BlueprintReadWrite)
// 	uint8 bIgnoreBaseRotation:1;

// 	/** Post-physics tick function for this character */
// 	UPROPERTY()
// 	struct FPawnMovementComponentPostPhysicsTickFunction PostPhysicsTickFunction;

// 	/** Force this pawn to bounce off its current base, which isn't an acceptable base for it. */
// 	virtual void JumpOff(AActor* MovementBaseActor);

// 	/** Used to prevent reentry of JumpOff() */
// 	UPROPERTY()
// 	uint8 bPerformingJumpOff:1;

// 	/** Can be overridden to choose to jump based on character velocity, base actor dimensions, etc. */
// 	virtual FVector GetBestDirectionOffActor(AActor* BaseActor) const; // Calculates the best direction to go to "jump off" an actor.

// 	/**
// 	 * Generate a random angle in degrees that is approximately equal between client and server.
// 	 * Note that in networked games this result changes with low frequency and has a low period,
// 	 * so should not be used for frequent randomization.
// 	 */
// 	virtual float GetNetworkSafeRandomAngleDegrees() const;

// 	virtual bool HasPredictionData_Client() const; // override?
// 	virtual bool HasPredictionData_Server() const; // override?

// 	/** Initial velocity (instantaneous vertical acceleration) when jumping. */
// 	UPROPERTY(Category="Character Movement: Jumping / Falling", EditAnywhere, BlueprintReadWrite, meta=(DisplayName="Jump Z Velocity", ClampMin="0", UIMin="0"))
// 	float JumpZVelocity;

// 	/** Fraction of JumpZVelocity to use when automatically "jumping off" of a base actor that's not allowed to be a base for a character. (For example, if you're not allowed to stand on other players.) */
// 	UPROPERTY(Category="Character Movement: Jumping / Falling", EditAnywhere, BlueprintReadWrite, AdvancedDisplay, meta=(ClampMin="0", UIMin="0"))
// 	float JumpOffJumpZFactor;

// 	/**
// 	 * Change movement mode.
// 	 *
// 	 * @param NewMovementMode	The new movement mode
// 	 * @param NewCustomMode		The new custom sub-mode, only applicable if NewMovementMode is Custom.
// 	 */
// 	UFUNCTION(BlueprintCallable, Category="Pawn|Components|CharacterMovement")
// 	virtual void SetMovementMode(EMovementMode NewMovementMode, uint8 NewCustomMode = 0);

// 	/** Get Navigation data for the Character. Returns null if there is no associated nav data. */
// 	const class INavigationDataInterface* GetNavData() const;

// 	/** Switch collision settings for NavWalking mode (ignore world collisions) */
// 	virtual void SetNavWalkingPhysics(bool bEnable);

// 	/**
// 	 * Default movement mode when not in water. Used at player startup or when teleported.
// 	 * @see DefaultWaterMovementMode
// 	 * @see bRunPhysicsWithNoController
// 	 */
// 	UPROPERTY(Category="Character Movement (General Settings)", EditAnywhere, BlueprintReadWrite)
// 	TEnumAsByte<enum EMovementMode> DefaultLandMovementMode;

// 	/** Returns true if the character is in the 'Walking' movement mode. */
// 	UFUNCTION(BlueprintCallable, Category="Pawn|Components|CharacterMovement")
// 	bool IsWalking() const;

// 	/** 
// 	 * Checks to see if the current location is not encroaching blocking geometry so the character can leave NavWalking.
// 	 * Restores collision settings and adjusts character location to avoid getting stuck in geometry.
// 	 * If it's not possible, MovementMode change will be delayed until character reach collision free spot.
// 	 * @return True if movement mode was successfully changed
// 	 */
// 	virtual bool TryToLeaveNavWalking();

// 	/**
// 	 * If true, crouching should keep the base of the capsule in place by lowering the center of the shrunken capsule. If false, the base of the capsule moves up and the center stays in place.
// 	 * The same behavior applies when the character uncrouches: if true, the base is kept in the same location and the center moves up. If false, the capsule grows and only moves up if the base impacts something.
// 	 * By default this variable is set when the movement mode changes: set to true when walking and false otherwise. Feel free to override the behavior when the movement mode changes.
// 	 */
// 	UPROPERTY(Category="Character Movement (General Settings)", VisibleInstanceOnly, BlueprintReadWrite, AdvancedDisplay)
// 	uint8 bCrouchMaintainsBaseLocation:1;

// 	/**
// 	 * Sweeps a vertical trace to find the floor for the capsule at the given location. Will attempt to perch if ShouldComputePerchResult() returns true for the downward sweep result.
// 	 * No floor will be found if collision is disabled on the capsule!
// 	 *
// 	 * @param CapsuleLocation		Location where the capsule sweep should originate
// 	 * @param OutFloorResult		[Out] Contains the result of the floor check. The HitResult will contain the valid sweep or line test upon success, or the result of the sweep upon failure.
// 	 * @param bCanUseCachedLocation If true, may use a cached value (can be used to avoid unnecessary floor tests, if for example the capsule was not moving since the last test).
// 	 * @param DownwardSweepResult	If non-null and it contains valid blocking hit info, this will be used as the result of a downward sweep test instead of doing it as part of the update.
// 	 */
// 	virtual void FindFloor(const FVector& CapsuleLocation, FFindFloorResult& OutFloorResult, bool bCanUseCachedLocation, const FHitResult* DownwardSweepResult = NULL) const;

// 	/** Last valid projected hit result from raycast to geometry from navmesh */
// 	FHitResult CachedProjectedNavMeshHitResult;

// 	UPROPERTY(Transient)
// 	float NavMeshProjectionTimer;

// 	/** How often we should raycast to project from navmesh to underlying geometry */
// 	UPROPERTY(Category="Character Movement: NavMesh Movement", EditAnywhere, BlueprintReadWrite, meta=(editcondition = "bProjectNavMeshWalking"))
// 	float NavMeshProjectionInterval;


// private:
// 	UPlayerCapsuleComponent* PlayerCapsuleComponent = nullptr;
	
// 	///// Timer
// 	float JumpCalledTime = 0;
// 	float JumpPressedTime = 0;

// 	/// Capsule Pivot
// 	bool HasBall = false;
// 	bool Pivot = false;

// 	/**
// 	 * Ground movement mode to switch to after falling and resuming ground movement.
// 	 * Only allowed values are: MOVE_Walking, MOVE_NavWalking.
// 	 * @see SetGroundMovementMode(), GetGroundMovementMode()
// 	 */
// 	UPROPERTY(Transient)
// 	TEnumAsByte<enum EMovementMode> GroundMovementMode;

// 	/** Saved location of object we are standing on, for UpdateBasedMovement() to determine if base moved in the last frame, and therefore pawn needs an update. */
// 	FQuat OldBaseQuat;

// 	/** Saved location of object we are standing on, for UpdateBasedMovement() to determine if base moved in the last frame, and therefore pawn needs an update. */
// 	FVector OldBaseLocation;
	
// protected:
// 	//virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

// 	/** Accessed in HasRootMotionSources() */
// 	AHoopzPlayer* HoopzPlayer = nullptr; // characterowner
// 	USkeletalMeshComponent* PlayerSkeletalMesh = nullptr;

//  	/** Restores Velocity to LastPreAdditiveVelocity during Root Motion Phys*() function calls */
// 	void RestorePreAdditiveRootMotionVelocity();

// 	/** Applies root motion from root motion sources to velocity (override and additive) */
// 	void ApplyRootMotionToVelocity(float deltaTime);

// 	class FNetworkPredictionData_Client_HoopzPawn* ClientPredictionData;
// 	class FNetworkPredictionData_Server_HoopzPawn* ServerPredictionData;

// 	FRandomStream RandomStream;

// 	/** Called after MovementMode has changed. Base implementation does special handling for starting certain modes, then notifies the CharacterOwner. */
// 	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode);
// };

// FORCEINLINE_DEBUGGABLE bool UHoopzPlayerMovementComponent::IsWalking() const
// {
// 	return IsMovingOnGround();
// }


// USTRUCT()
// struct FPawnMovementComponentPostPhysicsTickFunction : public FTickFunction
// {
// 	GENERATED_USTRUCT_BODY()

// 	/** CharacterMovementComponent that is the target of this tick **/
// 	class UHoopzPawnMovementComponent* Target;

// 	/** 
// 	 * Abstract function actually execute the tick. 
// 	 * @param DeltaTime - frame time to advance, in seconds
// 	 * @param TickType - kind of tick for this frame
// 	 * @param CurrentThread - thread we are executing on, useful to pass along as new tasks are created
// 	 * @param MyCompletionGraphEvent - completion event for this task. Useful for holding the completion of this task until certain child tasks are complete.
// 	 **/
// 	virtual void ExecuteTick(float DeltaTime, enum ELevelTick TickType, ENamedThreads::Type CurrentThread, const FGraphEventRef& MyCompletionGraphEvent) override;

// 	/** Abstract function to describe this tick. Used to print messages about illegal cycles in the dependency graph **/
// 	virtual FString DiagnosticMessage() override;
// 	/** Function used to describe this tick for active tick reporting. **/
// 	virtual FName DiagnosticContext(bool bDetailed) override;
// };


// /** Data about the floor for walking movement, used by CharacterMovementComponent. */
// USTRUCT(BlueprintType)
// struct ENGINE_API FFindFloorResult
// {
// 	GENERATED_USTRUCT_BODY()

// 	/**
// 	* True if there was a blocking hit in the floor test that was NOT in initial penetration.
// 	* The HitResult can give more info about other circumstances.
// 	*/
// 	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category=CharacterFloor)
// 	uint32 bBlockingHit:1;

// 	/** True if the hit found a valid walkable floor. */
// 	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category=CharacterFloor)
// 	uint32 bWalkableFloor:1;

// 	/** True if the hit found a valid walkable floor using a line trace (rather than a sweep test, which happens when the sweep test fails to yield a walkable surface). */
// 	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category=CharacterFloor)
// 	uint32 bLineTrace:1;

// 	/** The distance to the floor, computed from the swept capsule trace. */
// 	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category=CharacterFloor)
// 	float FloorDist;
	
// 	/** The distance to the floor, computed from the trace. Only valid if bLineTrace is true. */
// 	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category=CharacterFloor)
// 	float LineDist;

// 	/** Hit result of the test that found a floor. Includes more specific data about the point of impact and surface normal at that point. */
// 	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category=CharacterFloor)
// 	FHitResult HitResult;


// public:

// 	FFindFloorResult()
// 		: bBlockingHit(false)
// 		, bWalkableFloor(false)
// 		, bLineTrace(false)
// 		, FloorDist(0.f)
// 		, LineDist(0.f)
// 		, HitResult(1.f)
// 	{
// 	}

// 	/** Returns true if the floor result hit a walkable surface. */
// 	bool IsWalkableFloor() const
// 	{
// 		return bBlockingHit && bWalkableFloor;
// 	}

// 	void Clear()
// 	{
// 		bBlockingHit = false;
// 		bWalkableFloor = false;
// 		bLineTrace = false;
// 		FloorDist = 0.f;
// 		LineDist = 0.f;
// 		HitResult.Reset(1.f, false);
// 	}

// 	/** Gets the distance to floor, either LineDist or FloorDist. */
// 	float GetDistanceToFloor() const
// 	{
// 		// When the floor distance is set using SetFromSweep, the LineDist value will be reset.
// 		// However, when SetLineFromTrace is used, there's no guarantee that FloorDist is set.
// 		return bLineTrace ? LineDist : FloorDist;
// 	}

// 	void SetFromSweep(const FHitResult& InHit, const float InSweepFloorDist, const bool bIsWalkableFloor);
// 	void SetFromLineTrace(const FHitResult& InHit, const float InSweepFloorDist, const float InLineDist, const bool bIsWalkableFloor);
// };



// /** FSavedMove_Character represents a saved move on the client that has been sent to the server and might need to be played back. */
// class ENGINE_API FSavedMove_HoopzPawn
// {
// public:
// 	FSavedMove_HoopzPawn();
// 	virtual ~FSavedMove_HoopzPawn();

// 	// UE_DEPRECATED_FORGAME(4.20)
// 	FSavedMove_HoopzPawn(const FSavedMove_HoopzPawn&);
// 	FSavedMove_HoopzPawn(FSavedMove_HoopzPawn&&);
// 	FSavedMove_HoopzPawn& operator=(const FSavedMove_HoopzPawn&);
// 	FSavedMove_HoopzPawn& operator=(FSavedMove_HoopzPawn&&);

// 	AHoopzPlayer* HoopzPlayer; // CharacterOwner;

// 	uint32 bPressedJump:1;
// 	uint32 bWantsToCrouch:1;
// 	uint32 bForceMaxAccel:1;

// 	/** If true, can't combine this move with another move. */
// 	uint32 bForceNoCombine:1;

// 	/** If true this move is using an old TimeStamp, before a reset occurred. */
// 	uint32 bOldTimeStampBeforeReset:1;

// 	uint32 bWasJumping:1;

// 	float TimeStamp;    // Time of this move.
// 	float DeltaTime;    // amount of time for this move
// 	float CustomTimeDilation;
// 	float JumpKeyHoldTime;
// 	float JumpForceTimeRemaining;
// 	int32 JumpMaxCount;
// 	int32 JumpCurrentCount;
	
// 	UE_DEPRECATED_FORGAME(4.20, "This property is deprecated, use StartPackedMovementMode or EndPackedMovementMode instead.")
// 	uint8 MovementMode;

// 	// Information at the start of the move
// 	uint8 StartPackedMovementMode;
// 	FVector StartLocation;
// 	FVector StartRelativeLocation;
// 	FVector StartVelocity;
// 	FFindFloorResult StartFloor;
// 	FRotator StartRotation;
// 	FRotator StartControlRotation;
// 	FQuat StartBaseRotation;	// rotation of the base component (or bone), only saved if it can move.
// 	float StartCapsuleRadius;
// 	float StartCapsuleHalfHeight;
// 	TWeakObjectPtr<UPrimitiveComponent> StartBase;
// 	FName StartBoneName;
// 	uint32 StartActorOverlapCounter;
// 	uint32 StartComponentOverlapCounter;
// 	TWeakObjectPtr<USceneComponent> StartAttachParent;
// 	FName StartAttachSocketName;
// 	FVector StartAttachRelativeLocation;
// 	FRotator StartAttachRelativeRotation;

// 	// Information after the move has been performed
// 	uint8 EndPackedMovementMode;
// 	FVector SavedLocation;
// 	FRotator SavedRotation;
// 	FVector SavedVelocity;
// 	FVector SavedRelativeLocation;
// 	FRotator SavedControlRotation;
// 	TWeakObjectPtr<UPrimitiveComponent> EndBase;
// 	FName EndBoneName;
// 	uint32 EndActorOverlapCounter;
// 	uint32 EndComponentOverlapCounter;
// 	TWeakObjectPtr<USceneComponent> EndAttachParent;
// 	FName EndAttachSocketName;
// 	FVector EndAttachRelativeLocation;
// 	FRotator EndAttachRelativeRotation;

// 	FVector Acceleration;
// 	float MaxSpeed;

// 	// Cached to speed up iteration over IsImportantMove().
// 	FVector AccelNormal;
// 	float AccelMag;

// 	TWeakObjectPtr<class UAnimMontage> RootMotionMontage;
// 	float RootMotionTrackPosition;
// 	FRootMotionMovementParams RootMotionMovement;

// 	FRootMotionSourceGroup SavedRootMotion;

// 	/** Threshold for deciding this is an "important" move based on DP with last acked acceleration. */
// 	float AccelDotThreshold;    
// 	/** Threshold for deciding is this is an important move because acceleration magnitude has changed too much */
// 	float AccelMagThreshold;	
// 	/** Threshold for deciding if we can combine two moves, true if cosine of angle between them is <= this. */
// 	float AccelDotThresholdCombine;
// 	/** Client saved moves will not combine if the result of GetMaxSpeed() differs by this much between moves. */
// 	float MaxSpeedThresholdCombine;
	
// 	/** Clear saved move properties, so it can be re-used. */
// 	virtual void Clear();

// 	/** Called to set up this saved move (when initially created) to make a predictive correction. */
// 	virtual void SetMoveFor(AHoopzPlayer* Character, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_HoopzPawn & ClientData);

// 	/** Set the properties describing the position, etc. of the moved pawn at the start of the move. */
// 	virtual void SetInitialPosition(AHoopzPlayer* CCharacter);

// 	/** Returns true if this move is an "important" move that should be sent again if not acked by the server */
// 	virtual bool IsImportantMove(const FSavedMovePtr& LastAckedMove) const;
	
// 	/** Returns starting position if we were to revert the move, either absolute StartLocation, or StartRelativeLocation offset from MovementBase's current location (since we want to try to move forward at this time). */
// 	virtual FVector GetRevertedLocation() const;

// 	enum EPostUpdateMode
// 	{
// 		PostUpdate_Record,		// Record a move after having run the simulation
// 		PostUpdate_Replay,		// Update after replaying a move for a client correction
// 	};

// 	/** Set the properties describing the final position, etc. of the moved pawn. */
// 	virtual void PostUpdate(AHoopzPlayer* Character, EPostUpdateMode PostUpdateMode);
	
// 	/** Returns true if this move can be combined with NewMove for replication without changing any behavior */
// 	virtual bool CanCombineWith(const FSavedMovePtr& NewMove, AHoopzPlayer* InCharacter, float MaxDelta) const;

// 	/** Combine this move with an older move and update relevant state. */
// 	virtual void CombineWith(const FSavedMove_HoopzPawn* OldMove, AHoopzPlayer* InCharacter, APlayerController* PC, const FVector& OldStartLocation);
	
// 	/** Called before ClientUpdatePosition uses this SavedMove to make a predictive correction	 */
// 	virtual void PrepMoveFor(AHoopzPlayer* Character);

// 	/** Returns a byte containing encoded special movement information (jumping, crouching, etc.)	 */
// 	virtual uint8 GetCompressedFlags() const;

// 	/** Compare current control rotation with stored starting data */
// 	virtual bool IsMatchingStartControlRotation(const APlayerController* PC) const;

// 	/** Packs control rotation for network transport */
// 	virtual void GetPackedAngles(uint32& YawAndPitchPack, uint8& RollPack) const;

// 	// Bit masks used by GetCompressedFlags() to encode movement information.
// 	enum CompressedFlags
// 	{
// 		FLAG_JumpPressed	= 0x01,	// Jump pressed
// 		FLAG_WantsToCrouch	= 0x02,	// Wants to crouch
// 		FLAG_Reserved_1		= 0x04,	// Reserved for future use
// 		FLAG_Reserved_2		= 0x08,	// Reserved for future use
// 		// Remaining bit masks are available for custom flags.
// 		FLAG_Custom_0		= 0x10,
// 		FLAG_Custom_1		= 0x20,
// 		FLAG_Custom_2		= 0x40,
// 		FLAG_Custom_3		= 0x80,
// 	};
// };

// /** Shared pointer for easy memory management of FSavedMove_Character, for accumulating and replaying network moves. */
// typedef TSharedPtr<class FSavedMove_HoopzPawn> FSavedMovePtr;


// // ClientAdjustPosition replication (event called at end of frame by server)
// struct ENGINE_API FClientAdjustment
// {
// public:

// 	FClientAdjustment()
// 	: TimeStamp(0.f)
// 	, DeltaTime(0.f)
// 	, NewLoc(ForceInitToZero)
// 	, NewVel(ForceInitToZero)
// 	, NewRot(ForceInitToZero)
// 	, NewBase(NULL)
// 	, NewBaseBoneName(NAME_None)
// 	, bAckGoodMove(false)
// 	, bBaseRelativePosition(false)
// 	, MovementMode(0)
// 	{
// 	}

// 	float TimeStamp;
// 	float DeltaTime;
// 	FVector NewLoc;
// 	FVector NewVel;
// 	FRotator NewRot;
// 	UPrimitiveComponent* NewBase;
// 	FName NewBaseBoneName;
// 	bool bAckGoodMove;
// 	bool bBaseRelativePosition;
// 	uint8 MovementMode;
// };

// class FHoopzPawnReplaySample
// {
// public:
// 	FHoopzPawnReplaySample() : RemoteViewPitch( 0 ), Time( 0.0f )
// 	{
// 	}

// 	friend ENGINE_API FArchive& operator<<( FArchive& Ar, FHoopzPawnReplaySample& V );

// 	FVector			Location;
// 	FRotator		Rotation;
// 	FVector			Velocity;
// 	FVector			Acceleration;
// 	uint8			RemoteViewPitch;
// 	float			Time;					// This represents time since replay started
// };


// class ENGINE_API FNetworkPredictionData_Client_HoopzPawn : public FNetworkPredictionData_Client, protected FNoncopyable
// {
// public:

// 	FNetworkPredictionData_Client_HoopzPawn(const UHoopzPlayerMovementComponent& ClientMovement);
// 	virtual ~FNetworkPredictionData_Client_HoopzPawn();

// 	/** Client timestamp of last time it sent a servermove() to the server. This is an increasing timestamp from the owning UWorld. Used for holding off on sending movement updates to save bandwidth. */
// 	float ClientUpdateTime;

// 	/** Current TimeStamp for sending new Moves to the Server. This time resets to zero at a frequency of MinTimeBetweenTimeStampResets. */
// 	float CurrentTimeStamp;

// 	/** Last World timestamp (undilated, real time) at which we received a server ack for a move. This could be either a good move or a correction from the server. */
// 	float LastReceivedAckRealTime;

// 	TArray<FSavedMovePtr> SavedMoves;		// Buffered moves pending position updates, orderd oldest to newest. Moves that have been acked by the server are removed.
// 	TArray<FSavedMovePtr> FreeMoves;		// freed moves, available for buffering
// 	FSavedMovePtr PendingMove;				// PendingMove already processed on client - waiting to combine with next movement to reduce client to server bandwidth
// 	FSavedMovePtr LastAckedMove;			// Last acknowledged sent move.

// 	int32 MaxFreeMoveCount;					// Limit on size of free list
// 	int32 MaxSavedMoveCount;				// Limit on the size of the saved move buffer

// 	/** RootMotion saved while animation is updated, so we can store it and replay if needed in case of a position correction. */
// 	FRootMotionMovementParams RootMotionMovement;

// 	uint32 bUpdatePosition:1; // when true, update the position (via ClientUpdatePosition)

// 	// Mesh smoothing variables (for network smoothing)
// 	//
// 	/** Whether to smoothly interpolate pawn position corrections on clients based on received location updates */
// 	UE_DEPRECATED(4.11, "bSmoothNetUpdates will be removed, use UCharacterMovementComponent::NetworkSmoothingMode instead.")
// 	uint32 bSmoothNetUpdates:1;

// 	/** Used for position smoothing in net games */
// 	FVector OriginalMeshTranslationOffset;

// 	/** World space offset of the mesh. Target value is zero offset. Used for position smoothing in net games. */
// 	FVector MeshTranslationOffset;

// 	/** Used for rotation smoothing in net games (only used by linear smoothing). */
// 	FQuat OriginalMeshRotationOffset;

// 	/** Component space offset of the mesh. Used for rotation smoothing in net games. */
// 	FQuat MeshRotationOffset;

// 	/** Target for mesh rotation interpolation. */
// 	FQuat MeshRotationTarget;

// 	/** Used for remembering how much time has passed between server corrections */
// 	float LastCorrectionDelta;

// 	/** Used to track time of last correction */
// 	float LastCorrectionTime;

// 	/** Max time delta between server updates over which client smoothing is allowed to interpolate. */
// 	float MaxClientSmoothingDeltaTime;

// 	/** Used to track the timestamp of the last server move. */
// 	double SmoothingServerTimeStamp;

// 	/** Used to track the client time as we try to match the server.*/
// 	double SmoothingClientTimeStamp;

// 	/** Used to track how much time has elapsed since last correction. It can be computed as World->TimeSince(LastCorrectionTime). */
// 	UE_DEPRECATED(4.11, "CurrentSmoothTime will be removed, use LastCorrectionTime instead.")
// 	float CurrentSmoothTime;

// 	/** Used to signify that linear smoothing is desired */
// 	UE_DEPRECATED(4.11, "bUseLinearSmoothing will be removed, use UCharacterMovementComponent::NetworkSmoothingMode instead.")
// 	bool bUseLinearSmoothing;

// 	/**
// 	 * Copied value from UCharacterMovementComponent::NetworkMaxSmoothUpdateDistance.
// 	 * @see UCharacterMovementComponent::NetworkMaxSmoothUpdateDistance
// 	 */
// 	float MaxSmoothNetUpdateDist;

// 	/**
// 	 * Copied value from UCharacterMovementComponent::NetworkNoSmoothUpdateDistance.
// 	 * @see UCharacterMovementComponent::NetworkNoSmoothUpdateDistance
// 	 */
// 	float NoSmoothNetUpdateDist;

// 	/** How long to take to smoothly interpolate from the old pawn position on the client to the corrected one sent by the server.  Must be >= 0. Not used for linear smoothing. */
// 	float SmoothNetUpdateTime;

// 	/** How long to take to smoothly interpolate from the old pawn rotation on the client to the corrected one sent by the server.  Must be >= 0. Not used for linear smoothing. */
// 	float SmoothNetUpdateRotationTime;

// 	/** (DEPRECATED) How long server will wait for client move update before setting position */
// 	UE_DEPRECATED(4.12, "MaxResponseTime has been renamed to MaxMoveDeltaTime for clarity in what it does and will be removed, use MaxMoveDeltaTime instead.")
// 	float MaxResponseTime;
	
// 	/** 
// 	 * Max delta time for a given move, in real seconds
// 	 * Based off of AGameNetworkManager::MaxMoveDeltaTime config setting, but can be modified per actor
// 	 * if needed.
// 	 * This value is mirrored in FNetworkPredictionData_Server, which is what server logic runs off of.
// 	 * Client needs to know this in order to not send move deltas that are going to get clamped anyway (meaning
// 	 * they'll be rejected/corrected).
// 	 * Note: This was previously named MaxResponseTime, but has been renamed to reflect what it does more accurately
// 	 */
// 	float MaxMoveDeltaTime;

// 	/** Values used for visualization and debugging of simulated net corrections */
// 	FVector LastSmoothLocation;
// 	FVector LastServerLocation;
// 	float	SimulatedDebugDrawTime;

// 	/** Array of replay samples that we use to interpolate between to get smooth location/rotation/velocity/ect */
// 	TArray< FHoopzPawnReplaySample > ReplaySamples;

// 	/** Finds SavedMove index for given TimeStamp. Returns INDEX_NONE if not found (move has been already Acked or cleared). */
// 	int32 GetSavedMoveIndex(float TimeStamp) const;

// 	/** Ack a given move. This move will become LastAckedMove, SavedMoves will be adjusted to only contain unAcked moves. */
// 	void AckMove(int32 AckedMoveIndex, UCharacterMovementComponent& CharacterMovementComponent);

// 	/** Allocate a new saved move. Subclasses should override this if they want to use a custom move class. */
// 	virtual FSavedMovePtr AllocateNewMove();

// 	/** Return a move to the free move pool. Assumes that 'Move' will no longer be referenced by anything but possibly the FreeMoves list. Clears PendingMove if 'Move' is PendingMove. */
// 	virtual void FreeMove(const FSavedMovePtr& Move);

// 	/** Tries to pull a pooled move off the free move list, otherwise allocates a new move. Returns NULL if the limit on saves moves is hit. */
// 	virtual FSavedMovePtr CreateSavedMove();

// 	/** Update CurentTimeStamp from passed in DeltaTime.
// 		It will measure the accuracy between passed in DeltaTime and how Server will calculate its DeltaTime.
// 		If inaccuracy is too high, it will reset CurrentTimeStamp to maintain a high level of accuracy.
// 		@return DeltaTime to use for Client's physics simulation prior to replicate move to server. */
// 	float UpdateTimeStampAndDeltaTime(float DeltaTime, ACharacter & CharacterOwner, class UCharacterMovementComponent & CharacterMovementComponent);

// 	/** Used for simulated packet loss in development builds. */
// 	float DebugForcedPacketLossTimerStart;
// };

// class ENGINE_API FNetworkPredictionData_Server_HoopzPawn : public FNetworkPredictionData_Server, protected FNoncopyable
// {
// public:

// 	FNetworkPredictionData_Server_HoopzPawn(const UHoopzPlayerMovementComponent& ServerMovement);
// 	virtual ~FNetworkPredictionData_Server_HoopzPawn();

// 	FClientAdjustment PendingAdjustment;

// 	/** Timestamp from the client of most recent ServerMove() processed for this player. Reset occasionally for timestamp resets (to maintain accuracy). */
// 	float CurrentClientTimeStamp;

// 	/** Timestamp of total elapsed client time. Similar to CurrentClientTimestamp but this is accumulated with the calculated DeltaTime for each move on the server. */
// 	double ServerAccumulatedClientTimeStamp;

// 	/** Last time server updated client with a move correction */
// 	float LastUpdateTime;

// 	/** Server clock time when last server move was received from client (does NOT include forced moves on server) */
// 	float ServerTimeStampLastServerMove;

// 	/** (DEPRECATED) How long server will wait for client move update before setting position */
// 	UE_DEPRECATED(4.12, "MaxResponseTime has been renamed to MaxMoveDeltaTime for clarity in what it does and will be removed, use MaxMoveDeltaTime instead.")
// 	float MaxResponseTime;
	
// 	/** 
// 	 * Max delta time for a given move, in real seconds
// 	 * Based off of AGameNetworkManager::MaxMoveDeltaTime config setting, but can be modified per actor
// 	 * if needed.
// 	 * Note: This was previously named MaxResponseTime, but has been renamed to reflect what it does more accurately
// 	 */
// 	float MaxMoveDeltaTime;

// 	/** Force client update on the next ServerMoveHandleClientError() call. */
// 	uint32 bForceClientUpdate:1;

// 	/** Accumulated timestamp difference between autonomous client and server for tracking long-term trends */
// 	float LifetimeRawTimeDiscrepancy;

// 	/** 
// 	 * Current time discrepancy between client-reported moves and time passed
// 	 * on the server. Time discrepancy resolution's goal is to keep this near zero.
// 	 */
// 	float TimeDiscrepancy;

// 	/** True if currently in the process of resolving time discrepancy */
// 	bool bResolvingTimeDiscrepancy;

// 	/** 
// 	 * When bResolvingTimeDiscrepancy is true, we are in time discrepancy resolution mode whose output is
// 	 * this value (to be used as the DeltaTime for current ServerMove)
// 	 */
// 	float TimeDiscrepancyResolutionMoveDeltaOverride;

// 	/** 
// 	 * When bResolvingTimeDiscrepancy is true, we are in time discrepancy resolution mode where we bound
// 	 * move deltas by Server Deltas. In cases where there are multiple ServerMove RPCs handled within one
// 	 * server frame tick, we need to accumulate the client deltas of the "no tick" Moves so that the next
// 	 * Move processed that the server server has ticked for takes into account those previous deltas. 
// 	 * If we did not use this, the higher the framerate of a client vs the server results in more 
// 	 * punishment/payback time.
// 	 */
// 	float TimeDiscrepancyAccumulatedClientDeltasSinceLastServerTick;

// 	/** Creation time of this prediction data, used to contextualize LifetimeRawTimeDiscrepancy */
// 	float WorldCreationTime;

// 	/** Returns time delta to use for the current ServerMove(). Takes into account time discrepancy resolution if active. */
// 	float GetServerMoveDeltaTime(float ClientTimeStamp, float ActorTimeDilation) const;

// 	/** Returns base time delta to use for a ServerMove, default calculation (no time discrepancy resolution) */
// 	float GetBaseServerMoveDeltaTime(float ClientTimeStamp, float ActorTimeDilation) const;

// };

