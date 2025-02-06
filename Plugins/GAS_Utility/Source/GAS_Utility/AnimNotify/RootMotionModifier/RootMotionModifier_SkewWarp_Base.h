// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RootMotionModifier_SkewWarp.h"
#include "RootMotionModifier_SkewWarp_Base.generated.h"

/**
 * 
 */
UCLASS()
class GAS_UTILITY_API URootMotionModifier_SkewWarp_Base : public URootMotionModifier_SkewWarp
{
	GENERATED_BODY()
	

public:

	URootMotionModifier_SkewWarp_Base(const FObjectInitializer& ObjectInitializer);

	
	virtual void Update(const FMotionWarpingUpdateContext& Context) override;
	virtual FTransform ProcessRootMotion(const FTransform& InRootMotion, float DeltaSeconds) override;
	static FTransform CalculateRootTransformRelativeToWarpSocketAtTime(const ACharacter& Character, const UAnimSequenceBase* Animation, float Time, const FName& WarpPointBoneName);

	virtual FQuat WarpRotationOverride(const FTransform& RootMotionDelta, const FTransform& RootMotionTotal, float DeltaSeconds);
	FQuat GetTargetRotationOverride() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config", meta = (EditCondition = "WarpPointAnimProvider == EWarpPointAnimProvider::Bone"))
	FVector InWarpPointAnimBoneOffset;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config", meta = (EditCondition = "WarpPointAnimProvider == EWarpPointAnimProvider::Bone && bOrientInWarpPointAnimToDirectionMeshToTarget"))
	FRotator InWarpPointAnimBoneOrientation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config", meta = (EditCondition = "WarpPointAnimProvider == EWarpPointAnimProvider::Bone"))
	bool bOrientInWarpPointAnimToDirectionMeshToTarget;

	UPROPERTY()
	UAnimNotifyState_MotionWarping* Notify;

};


