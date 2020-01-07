// Cache$ Copyrights


#include "BallControlComponent.h"
#include "BasketBall.h"
#include "EngineUtils.h"

// Sets default values for this component's properties
UBallControlComponent::UBallControlComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

}


// Called when the game starts
void UBallControlComponent::BeginPlay()
{
	Super::BeginPlay();

	// Get BasketBall Reference
	for (TActorIterator<ABasketBall> It(GetWorld()); It; ++It)
	{
		ABasketBall* Target = *It;
		if (Target && Target->GetName() == FString("BasketBall")) {
			BasketBall = Target;
		}
	}
	
}

// Called every frame
void UBallControlComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UBallControlComponent::AttachBall(USceneComponent* SkeletalMesh, FName Socket)
{
	BasketBall->AttachToComponent(SkeletalMesh, AttachmentRule, Socket);
	// TODO : Disable physics when attaching ball, Enable when detaching
}