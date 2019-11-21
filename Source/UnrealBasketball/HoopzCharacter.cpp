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
	SpringArm = FindComponentByClass<USpringArmComponent>();

	PlayerRotation = CapsuleComponent->GetComponentRotation();
	TargetPlayerRotation = PlayerRotation;
	// SpringArmRotation = SpringArm->GetComponentRotation();
	// TargetSpringArmRotation = SpringArmRotation;
	
	//LandedDelegate.AddDynamic(this, &AHoopzCharacter::JumpLanded);

	for (TActorIterator<AStaticMeshActor> It(GetWorld()); It; ++It)
	{
		AStaticMeshActor* Mesh = *It;
		if (Mesh && Mesh->GetName() == FString("Basket")) {
			Basket = Mesh;
			BasketLocation = Basket->GetActorLocation();
			SpringArmTarget = BasketLocation;
			SpringArmTarget.Z = 68;
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
	
	// TODO: Fix Jerk

	if (ensure(Camera)) { Camera->SetWorldRotation(UKismetMathLibrary::FindLookAtRotation(Camera->GetComponentLocation(), BasketLocation), false);}
	if (ensure(SpringArm)) { SpringArm->SetWorldRotation(UKismetMathLibrary::FindLookAtRotation(SpringArm->GetComponentLocation(), SpringArmTarget), false); }

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
	PlayerInputComponent->BindAction("DashOrShot", IE_Pressed, this, &AHoopzCharacter::DashOrShot);
	PlayerInputComponent->BindAction("Floater", IE_Pressed, this, &AHoopzCharacter::Floater);

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

// Todo: Implement while moving one leg jumps
void AHoopzCharacter::JumpPressed()
{
	JumpHeldTime = GetWorld()->GetTimeSeconds();
	CapsuleDip = true;
}

void AHoopzCharacter::JumpReleased()
{
	JumpHeldTime -= GetWorld()->GetTimeSeconds();
	if (JumpHeldTime < -.3) {
		if (PivotMode == true) {
			CapsuleComponent->DetachFromComponent(DetachRules); // detach from pivotpoint
			// bUseControllerRotationYaw = true;
			// SpringArm->bInheritYaw = false;
			PivotDetached = true;
		}
		bPressedJump = true;
	}
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
	FName Foot;
	if (PivotKey == false) {
		Foot = FName(TEXT("foot_r"));
	} else {
		Foot = FName(TEXT("foot_l"));
	}

	FVector TargetPivotPointLocation = GetMesh()->GetSocketLocation(Foot);
	TargetPivotPointLocation.Z = CapsuleComponent->GetScaledCapsuleHalfHeight();
	PivotPoint->SetActorLocation(TargetPivotPointLocation, false);
	PivotPoint->SetActorRotation(CapsuleComponent->GetComponentRotation(), ETeleportType::None);
	PlayerRotation = PivotPoint->GetActorRotation();
	TargetPlayerRotation = PlayerRotation;
	CapsuleComponent->AttachToComponent(PivotPoint->GetRootComponent(), AttachRules);
	// HoopzCharacter->bUseControllerRotationYaw = false;
	// HoopzCharacter->SpringArm->bInheritYaw = true;
	PivotDetached = false;
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

// void AHoopzCharacter::SpringArmLerp(float DeltaTime)   
// {
//     SpringArmTurnTime = 0;
// 	TargetSpringArmRotation = UKismetMathLibrary::FindLookAtRotation(SpringArm->GetComponentLocation(), SpringArmTarget);

//     if (SpringArmTurnTime < SpringArmTurnDuration)
//     {
//         SpringArmTurnTime += DeltaTime;
//         SpringArmRotation = FMath::Lerp(SpringArmRotation, TargetSpringArmRotation, SpringArmTurnTime / SpringArmTurnDuration);
// 		SpringArm->SetWorldRotation(SpringArmRotation, false);
//     }
// }

void AHoopzCharacter::OnJumped_Implementation()
{
	if (!ensure(MainAnimInstance)) { return; }

	Jumped = true;

	// change animation transition state
	MainAnimInstance->Jumped = true;
	PivotInputKey = -1;
	//PivotMode = false;
}
void AHoopzCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

	if (!ensure(MainAnimInstance)) { return; }
	MainAnimInstance->Jumped = false;
	Jumped = false;
	PivotMode = true;  // if HasBall
	CanChangeShot = true;
	ShotKey = 0;
	EstablishPivot = false;
	PivotSet = false;
	PivotKey = false;
}

void AHoopzCharacter::DashOrShot()
{
	if (Jumped == true)
	{
		ShotKey = 1;
	}
	else {
		// dash
	}
}

void AHoopzCharacter::Floater()
{
	if (MainAnimInstance->HasBall == true) {
		MainAnimInstance->HasBall = false;
	} else {
		MainAnimInstance->HasBall = true;
	}
	
}