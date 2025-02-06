// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueManager.h"
#include "GameplayCueManagerBase.generated.h"

/**
 * 
 */
UCLASS()
class GAS_UTILITY_API UGameplayCueManagerBase : public UGameplayCueManager
{
	GENERATED_BODY()
	virtual void HandleGameplayCue(AActor* TargetActor, FGameplayTag GameplayCueTag, EGameplayCueEvent::Type EventType, const FGameplayCueParameters& Parameters, EGameplayCueExecutionOptions Options) override;
};
