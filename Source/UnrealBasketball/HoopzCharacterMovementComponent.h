// Cache$ Copyrights

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HoopzCharacterMovementComponent.generated.h"

/**
 * 
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UNREALBASKETBALL_API UHoopzCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	void CallFunction();

	
protected:
	
	//Init
	virtual void InitializeComponent() override;
	
	//Tick
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

private:
	//virtual void OnRootMotionSourceBeingApplied(const FRootMotionSource * Source) override;

};
