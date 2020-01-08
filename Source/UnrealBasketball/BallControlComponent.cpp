// Cache$ Copyrights


#include "BallControlComponent.h"
#include "BasketBall.h"
#include "EngineUtils.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Engine/EngineTypes.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

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
	
	SkelMesh = GetOwner()->FindComponentByClass<USkeletalMeshComponent>();

	BallHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	BallTrace = FCollisionQueryParams(TraceTag, false);
    BallTrace.AddIgnoredComponent(Cast<UPrimitiveComponent>(SkelMesh));
    BallTrace.AddIgnoredActor(Cast<AActor>(GetOwner()));

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

	if (BallHandle->GrabbedComponent)
	{
		// Move GrabbedComponent
		BallHandle->SetTargetLocation(SkelMesh->GetSocketLocation(FName(TEXT("ball_socket_r_end"))));
		// UE_LOG(LogTemp, Warning, TEXT("Ball moving."))
	}
}

void UBallControlComponent::AttachBall()
{
	// For Testing - Set Ball Location
	if (!ensure(BasketBall)) { return; }
	BasketBall->SetActorLocation(SkelMesh->GetSocketLocation(FName(TEXT("ball_socket_r_end"))), false);

	// Use Physics Handle
	FHitResult HitResult(ForceInit);

	GetWorld()->LineTraceSingleByObjectType(
		HitResult,
		SkelMesh->GetSocketLocation(FName(TEXT("ball_socket_r_start"))),
		SkelMesh->GetSocketLocation(FName(TEXT("ball_socket_r_end"))),
		FCollisionObjectQueryParams(ECollisionChannel::ECC_WorldDynamic),
		BallTrace
	);

	if (HitResult.GetComponent()){
		BallHandle->GrabComponentAtLocation(HitResult.GetComponent(), NAME_None, HitResult.GetComponent()->GetOwner()->GetActorLocation());
	}

	// TODO : Implement Release

}