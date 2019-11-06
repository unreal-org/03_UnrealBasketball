// Cache$ Copyrights

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "PivotAnimNotifyState.generated.h"

/**
 * 
 */
UCLASS()
class UNREALBASKETBALL_API UPivotAnimNotifyState : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
  	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;

	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime) override;

	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

private:
	bool IK = false;
	bool CanMove = true;

};
