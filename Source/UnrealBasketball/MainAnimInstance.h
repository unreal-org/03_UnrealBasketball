// ChoiBoi Copyrights

#pragma once

#include "Animation/AnimInstanceProxy.h"

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MainAnimInstance.generated.h"

class USkeletalMeshComponent;

USTRUCT( Blueprintable, BlueprintType )
struct FMainAnimInstanceProxy : public FAnimInstanceProxy
{
    GENERATED_BODY()

    FMainAnimInstanceProxy()
	: FAnimInstanceProxy()
    {}

    FMainAnimInstanceProxy(UAnimInstance* Instance);

public:
    // UPROPERTY(Transient, BlueprintReadWrite, EditAnywhere, Category = "Example")
    // float MovementAngle;

    // UPROPERTY(Transient, BlueprintReadWrite, EditAnywhere, Category = "Example")
    // float HorizontalSpeed;

	// Body Angle
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "JointAngles")
	FRotator PelvisRotation;

	// IK Locations
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "JointAngles")
	FVector RightFootLocation;   // world space
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "JointAngles")
	FVector LeftFootLocation;    // world space
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "JointAngles")
	FVector RightJointTargetLocation;   // world space
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "JointAngles")
	FVector LeftJointTargetLocation;    // world space

	// Movement
	void SetZRotation(float ZThrow);
	void SetFootTargetLocation(FVector AddToDirection);

private:
	FName Root = FName(TEXT("root_socket"));;
	FName RightFoot = FName(TEXT("foot_target_r"));
	FName LeftFoot = FName(TEXT("foot_target_l"));
	FName Pelvis = FName(TEXT("pelvis_socket"));;;
	FName RightJointTarget = FName(TEXT("joint_target_r"));
	FName LeftJointTarget = FName(TEXT("joint_target_l"));

	// Lerp Time
	float LerpTime;
	float LerpDuration = .5;
	
	// Target Positions
	FRotator PelvisTargetRotation;
	FVector RightFootTargetLocation;
	FVector LeftFootTargetLocation;

	// Foot Trace
	FName TraceTag = FName(TEXT("TraceTag"));;
	FCollisionQueryParams TraceParameters;
	float IKFootTrace(FName Foot);

	void TurnBody(float DeltaTimeX);
	void SetRightFoot();
	void SetLeftFoot();

	float MaxReach = 10;
	bool RightFootFree = true;
	bool LeftFootFree = false;

protected:
	virtual void Update(float DeltaSeconds) override;
	virtual void Initialize(UAnimInstance* InAnimInstance) override;

	UPROPERTY(BluePrintReadOnly)
	USkeletalMeshComponent* PlayerSkeletalMesh = nullptr;

	UObject* MainAnimInstance = nullptr;
};

/**
 * Controls Upper Body and Passes Feet variables to adjust legs in SubAnimInstance
 */
UCLASS( transient, Blueprintable, hideCategories = AnimInstance, BlueprintType )
class UNREALBASKETBALL_API UMainAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	// // Body Angle
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "JointAngles")
	// FRotator PelvisRotation;

	// // Feet
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "JointAngles")
	// FVector RightFootLocation;   // world space
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "JointAngles")
	// FVector LeftFootLocation;    // world space
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "JointAngles")
	// FVector RightJointTargetLocation;   // world space
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "JointAngles")
	// FVector LeftJointTargetLocation;    // world space
	
	// Constructor
	UMainAnimInstance(const FObjectInitializer& ObjectInitializer);

	// // Movement
	// void SetZRotation(float ZThrow);
	// void SetFootTargetLocation(FVector AddToDirection);
	
private:
	friend struct FMainAnimInstanceProxy;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Proxy", meta = (AllowPrivateAccess = "true"))
    FMainAnimInstanceProxy Proxy;
	
	// overriden to return custom proxy instance
    virtual FAnimInstanceProxy* CreateAnimInstanceProxy() override { return &Proxy; }

    virtual void DestroyAnimInstanceProxy(FAnimInstanceProxy* InProxy) override {}

	// FName Root;
	// FName RightFoot;
	// FName LeftFoot;
	// FName Pelvis;
	// FName RightJointTarget;
	// FName LeftJointTarget;
	// FName RightHeel;
	// FName LeftHeel;

	// // Lerp Time
	// float LerpTime;
	// float LerpDuration = .5;
	
	// FRotator PelvisTargetRotation;
	// FVector RightFootTargetLocation;
	// FVector LeftFootTargetLocation;

	// Foot Trace
	// FName TraceTag;
	// FCollisionQueryParams TraceParameters;
	// float IKFootTrace(FName Foot);
	
	// void TurnBody(float DeltaTimeX);
	// void SetRightFoot();
	// void SetLeftFoot();

	// float MaxReach = 10;
	// bool RightFootFree = true;
	// bool LeftFootFree = false;
	
protected:
	// Native initialization override point
	// virtual void NativeInitializeAnimation() override;

	// Tick
	// virtual void NativeUpdateAnimation(float DeltaTimeX) override;

	// UPROPERTY(BluePrintReadOnly)
	// USkeletalMeshComponent* PlayerSkeletalMesh = nullptr;

};
