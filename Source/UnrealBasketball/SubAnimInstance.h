// Cache$ Copyrights

#pragma once

#include "Animation/AnimInstanceProxy.h"

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SubAnimInstance.generated.h"

/**
 * To be used in Post Process Animation BP
 * Diable in Main Anim BP before previewing Animations
 */

class USkeletalMeshComponent;
class UPlayerCapsuleComponent;

USTRUCT( Blueprintable, BlueprintType )
struct FMainAnimInstanceProxy : public FAnimInstanceProxy
{
    GENERATED_BODY()

    FMainAnimInstanceProxy()
	: FAnimInstanceProxy()
    {}

    FMainAnimInstanceProxy(UAnimInstance* Instance);

public:
	// Movement
	void SetZRotation();
	void SetFootTargetLocation(FVector AddToDirection);
	void UpdateRightFootTargetLocation(float Angle);
	void UpdateLeftFootTargetLocation(float Angle);

private:
	// TODO : Dont hardcode FNames
	FName IKFootRoot = FName(TEXT("ik_foot_root"));
	FName RightFoot = FName(TEXT("foot_r"));
	FName LeftFoot = FName(TEXT("foot_l"));
	FName Pelvis = FName(TEXT("pelvis_socket"));
	FName RightJointTarget = FName(TEXT("joint_target_r"));
	FName LeftJointTarget = FName(TEXT("joint_target_l"));
	
	// Target Positions
	FRotator IKFootRootTargetRotation;
	FRotator PelvisTargetRotation;
	FVector RightFootTargetLocation;
	FVector LeftFootTargetLocation;
	FVector RightJointTargetPos;
	FVector LeftJointTargetPos;
	FVector RightFootOriginal;
	FVector LeftFootOriginal;

	// Foot Trace
	// FName TraceTag = FName(TEXT("TraceTag"));;
	// FCollisionQueryParams TraceParameters;
	// float IKFootTrace(FName Foot);

	// Foot Placement Variables
	float MaxReach = 30;
	bool RightFootFree = false;
	bool LeftFootFree = false;
	bool CanMove = true;
	float FootMoveStart;
	float FootMoveEnd = .5;

protected:
	virtual void Initialize(UAnimInstance* InAnimInstance) override;
	virtual void Update(float DeltaSeconds) override;
	virtual void PostUpdate(UAnimInstance* InAnimInstance) const override;

	USkeletalMeshComponent* PlayerSkeletalMesh = nullptr;
	UPlayerCapsuleComponent* PlayerCapsuleComponent = nullptr;
	USubAnimInstance* SubAnimInstance = nullptr;
};

UCLASS( Transient, Blueprintable, hideCategories = AnimInstance, BlueprintType )
class UNREALBASKETBALL_API USubAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	// Constructor
	USubAnimInstance(const FObjectInitializer& ObjectInitializer);

	// Body Angle
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "JointAngles")
	FRotator IKFootRoot;    // world space
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "JointAngles")
	FRotator PelvisRotation;  // world space
	
	// Feet
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "JointAngles")
	FVector RightFootLocation;   // world space
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "JointAngles")
	FVector LeftFootLocation;    // world space
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "JointAngles")
	FVector RightJointTargetLocation;   // world space
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "JointAngles")
	FVector LeftJointTargetLocation;    // world space

private:
	// Anim Instance Proxy
	friend struct FMainAnimInstanceProxy;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Proxy", meta = (AllowPrivateAccess = "true"))
    FMainAnimInstanceProxy Proxy;
	
    virtual FAnimInstanceProxy* CreateAnimInstanceProxy() override { return &Proxy; }

    virtual void DestroyAnimInstanceProxy(FAnimInstanceProxy* InProxy) override {}

	// Target Locations for Interp / Lerp
	FVector RightFootInterpTo;
	FVector LeftFootInterpTo;
	FRotator PelvisLerpTo;

	// Lerp Time
	float PelvisLerpTime;
	float PelvisLerpDuration = .5;
	float RightFootLerpTime;
	float RightFootLerpDuration = .1;
	float LeftFootLerpTime;
	float LeftFootLerpDuration = .1;

	void TurnBody(float DeltaTimeX);
	void SetRightFoot(float DeltaTimeX);
	void SetLeftFoot(float DeltaTimeX);

protected:
	// Initialize
	// virtual void NativeInitializeAnimation() override;

	// Tick
	virtual void NativeUpdateAnimation(float DeltaTimeX) override;

};
