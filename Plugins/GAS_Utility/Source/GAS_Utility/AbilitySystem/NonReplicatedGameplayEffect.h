// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "NonReplicatedGameplayEffect.generated.h"

/**
 * 
 */
UCLASS()
class GAS_UTILITY_API UNonReplicatedGameplayEffect : public UGameplayEffect
{
	GENERATED_BODY()
	UNonReplicatedGameplayEffect();

	virtual void GetReplicatedCustomConditionState(FCustomPropertyConditionState& OutActiveState) const override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
