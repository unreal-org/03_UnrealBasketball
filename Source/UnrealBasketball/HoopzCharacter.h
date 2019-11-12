// Cache$ Copyrights

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HoopzCharacter.generated.h"

class UHoopzCharacterMovementComponent;
class USplineComponent;
class AStaticMeshActor;
class UCameraComponent;

UCLASS()
class UNREALBASKETBALL_API AHoopzCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AHoopzCharacter();
	AHoopzCharacter(const FObjectInitializer& ObjectInitializer);

	UFUNCTION()
	void MoveForward(float Throw);

	UFUNCTION()
	void MoveRight(float Throw);

	bool PivotMode = false;
	int32 PivotInputKey = -1;

	FVector BasketLocation;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:	
	UHoopzCharacterMovementComponent* HoopzCharacterMovementComponent = nullptr;
	USplineComponent* PivotComponent = nullptr;
	AStaticMeshActor* Basket = nullptr;
	UCameraComponent* Camera = nullptr;
	

	void Pivot();
	FVector PivotForward;
	FVector PivotRight;
};
