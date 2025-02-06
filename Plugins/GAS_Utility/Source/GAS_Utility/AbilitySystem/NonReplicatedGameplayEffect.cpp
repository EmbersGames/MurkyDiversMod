// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/NonReplicatedGameplayEffect.h"

UNonReplicatedGameplayEffect::UNonReplicatedGameplayEffect()
{
	
}

void UNonReplicatedGameplayEffect::GetReplicatedCustomConditionState(FCustomPropertyConditionState& OutActiveState) const
{
	Super::GetReplicatedCustomConditionState(OutActiveState);
}

void UNonReplicatedGameplayEffect::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}
