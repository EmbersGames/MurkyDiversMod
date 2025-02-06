// Embers 2024


#include "AsyncTasks/AbilityAsync_WaitGameplayTags.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilitySystemLog.h"


void UAbilityAsync_WaitGameplayTags::Activate()
{
	Super::Activate();

	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (ASC && ShouldBroadcastDelegates())
	{
		for (auto Tag : Tags)
		{
			auto MyHandle = ASC->RegisterGameplayTagEvent(Tag, EGameplayTagEventType::AnyCountChange).AddUObject(this, &UAbilityAsync_WaitGameplayTags::GameplayTagCallback);
			MyHandles.Add(Tag, MyHandle);
			GameplayTagsOldCount.Add(Tag, ASC->GetTagCount(Tag));
		}
	}
	else
	{
		ABILITY_LOG(Warning, TEXT("%s: AbilitySystemComponent is nullptr! Could not register for gameplay tag event with Tags."), *GetName());
		EndAction();
	}
}

void UAbilityAsync_WaitGameplayTags::GameplayTagCallback(const FGameplayTag InTag, int32 NewCount)
{
	if (ShouldBroadcastDelegates())
	{
		UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
		BroadcastDelegate(InTag, NewCount, *GameplayTagsOldCount.Find(InTag));
	}
	else
	{
		EndAction();
	}
}


void UAbilityAsync_WaitGameplayTags::EndAction()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (ASC)
	{
		for (auto Tag : Tags)
		{
			if (MyHandles.Contains(Tag))
				ASC->RegisterGameplayTagEvent(Tag).Remove(*MyHandles.Find(Tag));
		}
	}
	Super::EndAction();
}


UAbilityAsync_WaitGameplayTags* UAbilityAsync_WaitGameplayTags::WaitGameplayTagsChangeOnActor(AActor* TargetActor, FGameplayTagContainer Tags)
{
	UAbilityAsync_WaitGameplayTags* MyObj = NewObject<UAbilityAsync_WaitGameplayTags>();
	MyObj->SetAbilityActor(TargetActor);
	MyObj->Tags = Tags;
	return MyObj;
}

void UAbilityAsync_WaitGameplayTags::BroadcastDelegate(FGameplayTag Tag, int32 NewCount, int32 OldCount)
{
	ChangeEvent.Broadcast(Tag, NewCount, OldCount);
	GameplayTagsOldCount.Add(Tag, NewCount);
}


UAbilityTask_WaitGameplayTags::UAbilityTask_WaitGameplayTags(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	UseExternalTarget = false;
}

void UAbilityTask_WaitGameplayTags::OnDestroy(bool AbilityIsEnding)
{
	UAbilitySystemComponent* ASC = GetTargetASC();
	if (ASC)
	{
		for (auto Tag : Tags)
		{
			if (MyHandles.Contains(Tag))
				ASC->RegisterGameplayTagEvent(Tag).Remove(*MyHandles.Find(Tag));
		}
	}

	Super::OnDestroy(AbilityIsEnding);
}
void UAbilityTask_WaitGameplayTags::SetExternalTarget(AActor* Actor)
{
	if (Actor)
	{
		UseExternalTarget = true;
		OptionalExternalTarget = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor);
	}
}

UAbilitySystemComponent* UAbilityTask_WaitGameplayTags::GetTargetASC()
{
	if (UseExternalTarget)
	{
		return OptionalExternalTarget;
	}

	return AbilitySystemComponent.Get();
}

UAbilityTask_WaitGameplayTags* UAbilityTask_WaitGameplayTags::WaitGameplayTagsChange(UGameplayAbility* OwningAbility, FGameplayTagContainer Tags, AActor* InOptionalExternalTarget)
{
	UAbilityTask_WaitGameplayTags* MyObj = NewAbilityTask<UAbilityTask_WaitGameplayTags>(OwningAbility);
	MyObj->Tags = Tags;
	MyObj->SetExternalTarget(InOptionalExternalTarget);

	return MyObj;
}

void UAbilityTask_WaitGameplayTags::Activate()
{
	UAbilitySystemComponent* ASC = GetTargetASC();
	if (ASC)
	{
		for (auto Tag : Tags)
		{
			auto MyHandle = ASC->RegisterGameplayTagEvent(Tag, EGameplayTagEventType::AnyCountChange).AddUObject(this, &UAbilityTask_WaitGameplayTags::GameplayTagCallback);
			MyHandles.Add(Tag, MyHandle);
			GameplayTagsOldCount.Add(Tag, ASC->GetTagCount(Tag));
		}
	}
	else
	{
		ABILITY_LOG(Warning, TEXT("%s: AbilitySystemComponent is nullptr! Could not register for gameplay tag event with Tags."), *GetName());
	}

	Super::Activate();
}

void UAbilityTask_WaitGameplayTags::GameplayTagCallback(const FGameplayTag InTag, int32 NewCount)
{
	UAbilitySystemComponent* ASC = GetTargetASC();
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		BroadcastDelegate(InTag, NewCount, *GameplayTagsOldCount.Find(InTag));
	}
	else
	{
		EndTask();
	}
}

void UAbilityTask_WaitGameplayTags::BroadcastDelegate(FGameplayTag Tag, int32 NewCount, int32 OldCount)
{
	ChangeEvent.Broadcast(Tag, NewCount, OldCount);
	GameplayTagsOldCount.Add(Tag, NewCount);
}
