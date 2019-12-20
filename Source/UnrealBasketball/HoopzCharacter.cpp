// Cache$ Copyrights


#include "HoopzCharacter.h"
#include "HoopzCharacterMovementComponent.h"
#include "Components/InputComponent.h"
#include "Components/SplineComponent.h"
#include "EngineUtils.h"
#include "Engine/StaticMeshActor.h"
#include "Kismet/KismetMathLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/EngineTypes.h"
#include "MainAnimInstance.h"
#include "GameFramework/SpringArmComponent.h"

/////////////////////////////// Constructors //////////////////////////////////////////
AHoopzCharacter::AHoopzCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

AHoopzCharacter::AHoopzCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UHoopzCharacterMovementComponent>(AHoopzCharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;
}


///////////////////////////// Called when the game starts or when spawned //////////////////////////
void AHoopzCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Character References
	HoopzCharacterMovementComponent = Cast<UHoopzCharacterMovementComponent>(GetCharacterMovement());
	PivotComponent = FindComponentByClass<USplineComponent>();
	Camera = FindComponentByClass<UCameraComponent>();
	CapsuleComponent = FindComponentByClass<UCapsuleComponent>();
	MainAnimInstance = dynamic_cast<UMainAnimInstance*>(GetMesh()->GetAnimInstance());
	
	// Spring Arm
	SpringArm = FindComponentByClass<USpringArmComponent>();
	SpringArmRotation = SpringArm->GetComponentRotation();
	TargetSpringArmRotation = SpringArmRotation;
	

	for (TActorIterator<AStaticMeshActor> It(GetWorld()); It; ++It)
	{
		AStaticMeshActor* Target = *It;
		if (Target && Target->GetName() == FString("Basket")) {
			Basket = Target;
			BasketLocation = Basket->GetActorLocation();
			SpringArmTarget = BasketLocation;
			SpringArmTarget.Z = 90;
		}
	}

	// Pivot Point Actor Reference
	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		AActor* Target = *It;
		if (Target && Target->GetName() == FString("PivotPoint")) {
			PivotPoint = Target;  
		}
	}

	// Pivot Point Rotation
	PivotPointRotation = CapsuleComponent->GetComponentRotation();

	// Pivot Point Spline Components Reference
	FootPivotPoints = dynamic_cast<USplineComponent*>(PivotPoint->GetRootComponent()->GetChildComponent(0));
	CapsulePivotPoints = dynamic_cast<USplineComponent*>(PivotPoint->GetRootComponent()->GetChildComponent(1));
}


////////////////////////////// Called every frame //////////////////////////////
void AHoopzCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// switch (CurrentState)
    // {
    //     case 0: // Idle
    //         break;
    //     case 1: // IdlePivot
	// 		if (!ensure(SpringArm)) { SpringArmLerp(DeltaTime); }
	// 		Pivot();
    //         break;
    //     case 3: // Dribble  
    //         break;
    //     case 4: // Jump (Ball)   
    //         break;
    //     case 6: // IdleOffense
    //         // spring arm static at rotation key
	// 		if (!ensure(SpringArm)) { SpringArmLerp(DeltaTime); }
    //         break;
    //     default:
    //         return;
    // }

	// Spring arm points and locks camera towards given target (Basket, Ball, or Player)
	if (ensure(SpringArm)) { SpringArmLerp(DeltaTime); }

	// Called during Pivot State (1)
	if (CurrentState == 1) { Pivot(); }

	// Called to lower and raise Capsule to adjust stance of Character
	CapsuleDipper();

	// UE_LOG(LogTemp, Warning, TEXT("Current State : %i"), CurrentState)
}


////////////////////// Called to bind functionality to input ///////////////////////////////////
void AHoopzCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Directional Input
	PlayerInputComponent->BindAxis("IntendMoveForward", this, &AHoopzCharacter::MoveForward);
	PlayerInputComponent->BindAxis("IntendMoveRight", this, &AHoopzCharacter::MoveRight);

	// Face Buttons
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AHoopzCharacter::JumpPressed);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AHoopzCharacter::JumpReleased);
	PlayerInputComponent->BindAction("TurnLeft", IE_Pressed, this, &AHoopzCharacter::TurnLeft);
	PlayerInputComponent->BindAction("TurnRight", IE_Pressed, this, &AHoopzCharacter::TurnRight);
	PlayerInputComponent->BindAction("DashOrShot", IE_Pressed, this, &AHoopzCharacter::DashOrShot);

	// Shoulder Buttons
	PlayerInputComponent->BindAction("Dribble", IE_Pressed, this, &AHoopzCharacter::Dribble);

	// For Testing - DPad
	PlayerInputComponent->BindAction("TogglePivot", IE_Pressed, this, &AHoopzCharacter::TogglePivot);
	PlayerInputComponent->BindAction("ToggleOffense", IE_Pressed, this, &AHoopzCharacter::ToggleOffense);
	
}


////////////////////////// Helper Functions ////////////////////////////
void AHoopzCharacter::OnTurnTimerExpire()
{
    CanTurn = true;
}


////////////////////// Directional Input ///////////////////////////////
void AHoopzCharacter::MoveForward(float Throw)
{
	// FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::X);
	if (CurrentState == 0) { return; }

	FVector ForwardDirection = Camera->GetForwardVector();
	// FVector Direction = GetActorForwardVector();
	ForwardThrow = ForwardDirection;

	if (PivotMode == true) {
        PivotForward = ForwardDirection * Throw * 40;
	} else {
		AddMovementInput(ForwardDirection, Throw, false);
	}
}

void AHoopzCharacter::MoveRight(float Throw)
{
	// FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::Y);
	if (CurrentState == 0) { return; }

	FVector RightDirection = Camera->GetRightVector();
	// FVector Direction = GetActorRightVector();
	RightThrow = RightDirection;

	if (PivotMode == true) {
        PivotRight = RightDirection * Throw * 40;
	} else {
		AddMovementInput(RightDirection, Throw, false);
	}
}

//////////////////////////// Movement Mode Change Functions (Jump) ///////////////////////////////////
void AHoopzCharacter::OnJumped_Implementation()    // Called on Jump
{
	if (!ensure(MainAnimInstance)) { return; }

	Jumped = true;   // Makes character Jump
	MainAnimInstance->Jumped = true;   // Change animation transition state
}

void AHoopzCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)   // Called on Landing
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

	if (!ensure(MainAnimInstance)) { return; }
	MainAnimInstance->Jumped = false;     // Change transition variable for state change
	Jumped = false;

	if (MainAnimInstance->HasBall == true) {     // If character has ball
		PivotMode = true;                        // TODO : check for double dribble
	} else {			 // If  not - move to offense state (without ball)
		PivotMode = false;
		// MainAnimInstance->Offense = true;
	}

	CanChangeShot = true;   // Allows for one shot selection
	ShotKey = 0;            // Initial Jump State Pose Key

	EstablishPivot = false; // Reset Pivot variables
	PivotKey = false;
	PivotInputKey = -1;
	PivotAttached = false;
}


///////////////////// Action Input //////////////////////////////////
void AHoopzCharacter::JumpPressed()     // Face Button bottom press
{
	if (CurrentState == 0) { return; }    // Todo: Implement while moving one leg jumps

	JumpHeldTime = GetWorld()->GetTimeSeconds();
	CapsuleDip = true;
}

void AHoopzCharacter::JumpReleased()    // Face Button bottom release
{
	if (CurrentState == 0) { return; }

	JumpHeldTime -= GetWorld()->GetTimeSeconds();
	if (JumpHeldTime < -.3) {
		bPressedJump = true;
	}
	CapsuleDip = false;
}

// TODO : Implement Post up turn (NULL)
// TODO : Fix Multiple Input Problem
void AHoopzCharacter::TurnLeft()     // Face Button top
{
	if (CurrentState == 0) { return; }

	if (CanTurn) {
		CanTurn = false;
		if (PivotMode == true) {
			if (EstablishPivot == false || PivotAttached == false) {
				PivotKey = true;  // right moves, plant left
				SetPivot();
				EstablishPivot = true;
			}
			PivotPointRotation.Yaw -= 45;
			TotalRotation.Yaw -= 45;
			PivotPoint->SetActorRotation(PivotPointRotation, ETeleportType::None);
			PivotTurn = true;
			PivotTurnLeft = true;
		} else {   // For Turning without ball
			TotalRotation.Yaw -= 45;
		}
	}

	// Set CanTurn = true on timer expire
	FTimerHandle TurnTimer;
	GetWorld()->GetTimerManager().SetTimer(TurnTimer, this, &AHoopzCharacter::OnTurnTimerExpire, TurnDelay, false);
	return; 
}

void AHoopzCharacter::TurnRight()     // Face Button right
{
	if (CurrentState == 0) { return; }

	if (CanTurn == true) {
		CanTurn = false;
		if (PivotMode == true) {
			if (EstablishPivot == false || PivotAttached == false) {
				PivotKey = false; // left moves, plant right
				SetPivot();
				EstablishPivot = true;
			}
			PivotPointRotation.Yaw += 45;
			TotalRotation.Yaw += 45;
			PivotPoint->SetActorRotation(PivotPointRotation, ETeleportType::None);
			PivotTurn = true;
			PivotTurnRight = true;

		} else {
			TotalRotation.Yaw += 45;
		}
	}
	
	FTimerHandle TurnTimer;
	GetWorld()->GetTimerManager().SetTimer(TurnTimer, this, &AHoopzCharacter::OnTurnTimerExpire, TurnDelay, false);
	return; 
}

void AHoopzCharacter::DashOrShot()   // Face Button Left 
{
	if (CurrentState == 0) { return; }

	if (Jumped == true) { ShotKey = 1; }
	else {
		// dash
	}
}

// TODO : Consider Shoulder trigger axis for variable height dribble
void AHoopzCharacter::Dribble()    // Shoulder Trigger right 
{
	if (CurrentState == 0) { return; }

	if (MainAnimInstance->Dribble == true) {
		MainAnimInstance->Dribble = false;
	} else {
		MainAnimInstance->Dribble = true;
	}
}


///////////////////// Called During Pivot - State 1 //////////////////////////
void AHoopzCharacter::Pivot()
{
	if ((PivotForward + PivotRight).Size2D() < 5) { return; }
	if (!ensure(PivotComponent)) { return; }
	
    FVector PivotDirection = GetActorLocation() + PivotForward + PivotRight;
    PivotInputKey = PivotComponent->FindInputKeyClosestToWorldLocation(PivotDirection);

	if (EstablishPivot == false && PivotInputKey != -1)
	{
		if (PivotInputKey <= 4) { PivotKey = false; } // Left Foot
		else { PivotKey = true; } // Right Foot
		SetPivot();
		EstablishPivot = true;
	}
}

void AHoopzCharacter::SetPivot()
{
	FName Foot;
	if (PivotKey == false) {  // Left moves, Plant Right
		Foot = FName(TEXT("foot_r"));
	} else {
		Foot = FName(TEXT("foot_l"));
	}

	FVector CapsulePivotTurnAnchor = GetMesh()->GetSocketLocation(Foot);
	PivotPoint->SetActorLocation(CapsulePivotTurnAnchor, false);
	PivotPoint->SetActorRotation(CapsuleComponent->GetComponentRotation(), ETeleportType::None);
	PivotPointRotation = PivotPoint->GetActorRotation();
	MainAnimInstance->FootPlanted = false;
	PivotAttached = true;
}


//////////////////// Capsule Dipper /////////////////////////////
void AHoopzCharacter::SetCapsuleHalfHeight(float MaxValue, float MinValue)
{
	MaxCapsuleHalfHeight = MaxValue;
	MinCapsuleHalfHeight = MinValue;
}

void AHoopzCharacter::CapsuleDipper()
{
	if (!ensure(CapsuleComponent)) { return; }

	float Height = CapsuleComponent->GetScaledCapsuleHalfHeight();
	if (CapsuleDip == true && Height > MinCapsuleHalfHeight) {
		CapsuleComponent->SetCapsuleHalfHeight(Height - 1, false);
	} 
	else if (CapsuleDip == false && Height < MaxCapsuleHalfHeight) {
		CapsuleComponent->SetCapsuleHalfHeight(Height + 1, false);
	}
	else if (CapsuleDip == false && Height > MaxCapsuleHalfHeight) {
		CapsuleComponent->SetCapsuleHalfHeight(Height - 1, false);
	}
}


/////////////////////////// Lerp Functions //////////////////////////////
void AHoopzCharacter::SpringArmLerp(float DeltaTime)   
{
    SpringArmTurnTime = 0;

	// TODO : Allow for runtime switching of Spring arm Target
	TargetSpringArmRotation = UKismetMathLibrary::FindLookAtRotation(SpringArm->GetComponentLocation(), SpringArmTarget);
	
    if (SpringArmTurnTime < SpringArmTurnDuration)
    {
        SpringArmTurnTime += DeltaTime;
        SpringArmRotation = FMath::Lerp(SpringArmRotation, TargetSpringArmRotation, SpringArmTurnTime / SpringArmTurnDuration);
		SpringArm->SetWorldRotation(SpringArmRotation, false);
    }
}


///////////// Testing ////////////////
void AHoopzCharacter::TogglePivot()
{
	if (MainAnimInstance->HasBall == true) {
		MainAnimInstance->HasBall = false;
		PivotMode = false;
	} else {
		MainAnimInstance->HasBall = true;
		PivotMode = true;
	}
}

void AHoopzCharacter::ToggleOffense()
{
	if (MainAnimInstance->Offense == true) {
		MainAnimInstance->Offense = false;
	} else {
		MainAnimInstance->Offense = true;
	}
}