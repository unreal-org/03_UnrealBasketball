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

// Sets default values
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

// Called when the game starts or when spawned
void AHoopzCharacter::BeginPlay()
{
	Super::BeginPlay();

	HoopzCharacterMovementComponent = Cast<UHoopzCharacterMovementComponent>(GetCharacterMovement());
	PivotComponent = FindComponentByClass<USplineComponent>();
	Camera = FindComponentByClass<UCameraComponent>();
	CapsuleComponent = FindComponentByClass<UCapsuleComponent>();
	
	for (TActorIterator<AStaticMeshActor> It(GetWorld()); It; ++It)
	{
		AStaticMeshActor* Mesh = *It;
		if (Mesh && Mesh->GetName() == FString("Basket"))
		{
			Basket = Mesh;
			BasketLocation = Basket->GetActorLocation();
			break;
		}
	}
}

// Called every frame
void AHoopzCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (ensure(Camera)) { Camera->SetWorldRotation(UKismetMathLibrary::FindLookAtRotation(Camera->GetComponentLocation(), BasketLocation), false);}
	if (PivotMode == true) { Pivot(); }
	CapsuleDipper();

}

// Called to bind functionality to input
void AHoopzCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("IntendMoveForward", this, &AHoopzCharacter::MoveForward);
	PlayerInputComponent->BindAxis("IntendMoveRight", this, &AHoopzCharacter::MoveRight);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AHoopzCharacter::JumpPressed);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AHoopzCharacter::JumpReleased);
	PlayerInputComponent->BindAction("TurnLeft", IE_Pressed, this, &AHoopzCharacter::TurnLeft);
	PlayerInputComponent->BindAction("TurnRight", IE_Pressed, this, &AHoopzCharacter::TurnRight);
}

void AHoopzCharacter::MoveForward(float Throw)
{
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::X);

	if (PivotMode == true) {
        PivotForward = Direction * Throw * 40;
	} else {
		AddMovementInput(Direction, Throw, false);
	}
}

void AHoopzCharacter::MoveRight(float Throw)
{
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::Y);

	if (PivotMode == true) {
        PivotRight = Direction * Throw * 40;
	} else {
		AddMovementInput(Direction, Throw, false);
	}
}

void AHoopzCharacter::Pivot()
{
	/*
	if (PivotSet == false)
	if (!ensure(CapsulePivotBig)) { return; }
	PivotComponent->SetWorldLocation(GetComponentLocation());
	PivotComponent->SetWorldRotation(GetComponentRotation());
	*/

	if ((PivotForward + PivotRight).Size2D() < 1) { return; }
	if (!ensure(PivotComponent)) { return; }
    FVector PivotDirection = GetActorLocation() + PivotForward + PivotRight;
    PivotInputKey = PivotComponent->FindInputKeyClosestToWorldLocation(PivotDirection);
}

// Todo: Decrease Capsule Half Height
void AHoopzCharacter::JumpPressed()
{
	JumpHeldTime = GetWorld()->GetTimeSeconds();
	CapsuleDip = true;
}

void AHoopzCharacter::JumpReleased()
{
	JumpHeldTime -= GetWorld()->GetTimeSeconds();
	if (JumpHeldTime < -.3) { bPressedJump = true; }
	CapsuleDip = false;
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
}

void AHoopzCharacter::TurnLeft()
{
	if (CanTurn) { PivotPos += 1; }
	// Set Pivot Component while no rootmotion
	// Lock temporarily
}

void AHoopzCharacter::TurnRight()
{
	if (CanTurn) { PivotPos -= 1; }
	// Set Pivot Component while no rootmotion
	// Lock temporarily
}