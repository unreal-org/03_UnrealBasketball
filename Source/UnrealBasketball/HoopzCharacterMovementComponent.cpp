// Cache$ Copyrights


#include "HoopzCharacterMovementComponent.h"
#include "Engine/World.h"
#include "HoopzCharacter.h"

void UHoopzCharacterMovementComponent::InitializeComponent()
{
    Super::InitializeComponent();

}
	
void UHoopzCharacterMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    UE_LOG(LogTemp, Warning, TEXT("%i"), *HasRootMotionSources())
}

void UHoopzCharacterMovementComponent::CallFunction()
{
    UE_LOG(LogTemp, Warning, TEXT("Custom CharacterMovementComponent called."))
}

// TODO : Set MovementMode

// void UHoopzCharacterMovementComponent::OnRootMotionSourceBeingApplied(const FRootMotionSource * Source)
// {
//     Super::OnRootMotionSourceBeingApplied(Source);
//     UE_LOG(LogTemp, Warning, TEXT("Root Motion applied."))
// }