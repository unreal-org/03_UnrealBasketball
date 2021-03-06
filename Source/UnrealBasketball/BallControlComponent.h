// Cache$ Copyrights

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BallControlComponent.generated.h"

class ABasketBall;
class UPhysicsHandleComponent;
class USkeletalMeshComponent;

struct FAttachmentTransformRules;
struct FDetachmentTransformRules;

/*******
 * Ball Control Component calculates Dribbling & Shot Trajectory
 * Ball Atttachment & Detachment
*******/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UNREALBASKETBALL_API UBallControlComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBallControlComponent();

	// Attach Ball
	void AttachBall();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	ABasketBall* BasketBall = nullptr;
	USkeletalMeshComponent* SkelMesh = nullptr;

	// Physics Handle
	UPhysicsHandleComponent* BallHandle = nullptr;
	FName TraceTag = FName(TEXT("TraceTag"));
	FCollisionQueryParams BallTrace;

	// FAttachmentTransformRules AttachmentRule = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, true);
	// FDetachmentTransformRules DetachmentRule = FDetachmentTransformRules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, false);
		
};
