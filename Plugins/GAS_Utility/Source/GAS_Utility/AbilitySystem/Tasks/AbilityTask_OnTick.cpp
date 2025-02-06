// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Tasks/AbilityTask_OnTick.h"


UAbilityTask_OnTick::UAbilityTask_OnTick(const FObjectInitializer& ObjectInitializer)
{
	bTickingTask = true;
}

UAbilityTask_OnTick* UAbilityTask_OnTick::AbilityTaskOnTick(UGameplayAbility* OwningAbility, FName TaskInstanceName)
{
	UAbilityTask_OnTick* MyObj = NewAbilityTask<UAbilityTask_OnTick>(OwningAbility);
	return MyObj;
}

void UAbilityTask_OnTick::Activate()
{
	Super::Activate();
}

void UAbilityTask_OnTick::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);
	OnTick.Broadcast(DeltaTime);
}