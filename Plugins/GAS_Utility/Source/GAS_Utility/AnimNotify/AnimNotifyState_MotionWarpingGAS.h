// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimNotifyState_MotionWarping.h"
#include "Components/SkeletalMeshComponent.h"
#include "AnimNotifyState_MotionWarpingGAS.generated.h"

/**
 * 
 */
UCLASS()
class GAS_UTILITY_API UAnimNotifyState_MotionWarpingGAS : public UAnimNotifyState_MotionWarping
{
	GENERATED_BODY()
public:

	
#if WITH_EDITOR
	virtual void OnAnimNotifyCreatedInEditor(FAnimNotifyEvent& ContainingAnimNotifyEvent) override;;
#endif
	
	virtual void NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, const FAnimNotifyEventReference& EventReference) override;
	
};
