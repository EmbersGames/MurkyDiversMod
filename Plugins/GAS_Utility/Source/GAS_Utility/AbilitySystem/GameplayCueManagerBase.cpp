// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/GameplayCueManagerBase.h"

#include "AbilitySystemComponentBase.h"
#include "AbilitySystemInterface.h"

void UGameplayCueManagerBase::HandleGameplayCue(AActor* TargetActor, FGameplayTag GameplayCueTag, EGameplayCueEvent::Type EventType, const FGameplayCueParameters& Parameters, EGameplayCueExecutionOptions Options)
{
	Super::HandleGameplayCue(TargetActor, GameplayCueTag, EventType, Parameters, Options);
	if(EventType==EGameplayCueEvent::WhileActive )
	{
		if(!GIsServer)
		{
			if (const auto CastOwner = Cast<IAbilitySystemInterface>(TargetActor))
			{
				if (const auto GasComp = CastOwner->GetAbilitySystemComponent())
				{
					if (const auto CastedGasComp = Cast<UAbilitySystemComponentBase>(GasComp))
					{
						CastedGasComp->OnTagUpdate.Broadcast(GameplayCueTag,CastedGasComp->HasGameplayTag(GameplayCueTag));
					}
				}
			}
		}
	}
}
