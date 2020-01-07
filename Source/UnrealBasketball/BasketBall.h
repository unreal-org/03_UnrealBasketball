// Cache$ Copyrights

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BasketBall.generated.h"

/*******
 * BasketBall will have properties like bounce and such (TBD)
*******/
UCLASS()
class UNREALBASKETBALL_API ABasketBall : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABasketBall();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

private: 

};
