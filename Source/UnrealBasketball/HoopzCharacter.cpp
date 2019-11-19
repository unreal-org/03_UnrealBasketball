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
	MainAnimInstance = dynamic_cast<UMainAnimInstance*>(GetMesh()->GetAnimInstance());

	PlayerRotation = CapsuleComponent->GetComponentRotation();
	TargetPlayerRotation = PlayerRotation;

	for (TActorIterator<AStaticMeshActor> It(GetWorld()); It; ++It)
	{
		AStaticMeshActor* Mesh = *It;
		if (Mesh && Mesh->GetName() == FString("Basket")) {
			Basket = Mesh;
			BasketLocation = Basket->GetActorLocation();
		}
	}

	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		AActor* Mesh = *It;
		if (Mesh && Mesh->GetName() == FString("PivotPoint")) {
			PivotPoint = Mesh;
		}
	}
}

// Called every frame
void AHoopzCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// TODO: Make Camera line up behind player and basket
	if (ensure(Camera)) { Camera->SetWorldRotation(UKismetMathLibrary::FindLookAtRotation(Camera->GetComponentLocation(), BasketLocation), false);}
	if (PivotMode == true) { Pivot(); }
	TurnLerp(DeltaTime);
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
	if ((PivotForward + PivotRight).Size2D() < 5) { return; }
	if (!ensure(PivotComponent)) { return; }
    FVector PivotDirection = GetActorLocation() + PivotForward + PivotRight;
    PivotInputKey = PivotComponent->FindInputKeyClosestToWorldLocation(PivotDirection);

	if (EstablishPivot == false && PivotInputKey != -1)
	{
		if (PivotInputKey <= 4) { PivotKey = false; } // Left Foot
		else { PivotKey = true; } // Right Foot
		EstablishPivot = true;
	}
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

void AHoopzCharacter::OnTurnTimerExpire()
{
    CanTurn = true;
}


// TODO: Implement dribble turn & Post up turn (NULL)
void AHoopzCharacter::TurnLeft()
{
	if (CanTurn) {
		CanTurn = false;
		
		if (PivotMode == true) {
			if (EstablishPivot == false || PivotSet == false) {
				PivotSet = true;
				PivotKey = true;  // right moves, plant left
				EstablishPivot = true;

				SetPivot();
			}

			TargetPlayerRotation.Yaw -= 45;

			FTimerHandle TurnTimer;
	    	GetWorld()->GetTimerManager().SetTimer(TurnTimer, this, &AHoopzCharacter::OnTurnTimerExpire, TurnDelay, false);
        	return; 
		}
	}
	// else {
    //    
    // }
	// Set Pivot Component while no rootmotion
}

void AHoopzCharacter::TurnRight()
{
	if (CanTurn) {
		CanTurn = false;

		if (PivotMode == true) {
			if (EstablishPivot == false || PivotSet == false) {
				PivotSet = true;
				PivotKey = false; // left moves, plant right
				EstablishPivot = true;

				SetPivot();
			}

			TargetPlayerRotation.Yaw += 45;

			FTimerHandle TurnTimer;
			GetWorld()->GetTimerManager().SetTimer(TurnTimer, this, &AHoopzCharacter::OnTurnTimerExpire, TurnDelay, false);
			return; 
		}
	}
	// else {
	//
    // }
	// Set Pivot Component while no rootmotion
}

void AHoopzCharacter::SetPivot()
{
	// attach component code
	if (PivotKey == false) {
		FName Foot = FName(TEXT("foot_r"));
	} else {
		FName Foot = FName(TEXT("foot_l"));
	}

	FVector TargetPivotPointLocation = GetMesh()->GetSocketLocation(Foot);
	TargetPivotPointLocation.Z = CapsuleComponent->GetScaledCapsuleHalfHeight();
	PivotPoint->SetActorLocation(TargetPivotPointLocation, false);
	PivotPoint->SetActorRotation(CapsuleComponent->GetComponentRotation(), ETeleportType::None);
	PlayerRotation = PivotPoint->GetActorRotation();
	TargetPlayerRotation = PlayerRotation;
	CapsuleComponent->AttachToComponent(PivotPoint->GetRootComponent(), AttachRules);	
}

void AHoopzCharacter::TurnLerp(float DeltaTime)   
{
    TurnTime = 0;

    if (TurnTime < TurnDuration)
    {
        TurnTime += DeltaTime;
        PlayerRotation = FMath::Lerp(PlayerRotation, TargetPlayerRotation, TurnTime / TurnDuration);
		PivotPoint->SetActorRotation(PlayerRotation, ETeleportType::None);
    }
}

void AHoopzCharacter::OnJumped_Implementation()
{
	if (!ensure(MainAnimInstance)) { return; }

	// detach from pivotpoint
	CapsuleComponent->DetachFromComponent(DetachRules);

	// change animation transition state
	MainAnimInstance->Jumped = true;
	PivotInputKey = -1;
	PivotMode = false;

}