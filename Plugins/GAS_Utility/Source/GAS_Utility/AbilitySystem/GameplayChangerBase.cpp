// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/GameplayChangerBase.h"

#include "AbilitySystemComponentBase.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

UGameplayChangerBase::UGameplayChangerBase(const FGameplayTag Tag): Super()
{
	bTickEnable = false;
	if (Tag.IsValid())
	{
		GameplayChangerTag = Tag;
		LocalTagContainer.AddTag(Tag);
	}
	else
	{
		if (GameplayChangerTag.IsValid())
		{
			LocalTagContainer.AddTag(GameplayChangerTag);
		}
	}
	InitAttributeChangers();
}

UGameplayChangerBase::UGameplayChangerBase() : Super()
{
	bTickEnable = false;
	if (GameplayChangerTag.IsValid())
	{
		LocalTagContainer.AddTag(GameplayChangerTag);
	}
	InitAttributeChangers();
}

void UGameplayChangerBase::InitAttributeChangers()
{
	for (auto Element : AttributeChangers)
	{
		for (auto AttributeChanger : Element.Value.ArrayList)
		{
			if (AttributeChanger)
			{
				AttributeChanger->GameplayChangerParent = this;
				AttributeChanger->GameplayAttribute = Element.Key;
			}
		}
	}
}

TMap<FGameplayAttribute, FAttributeChangerArray> UGameplayChangerBase::GetAttributeChangers()
{
	return AttributeChangers;
}

void UGameplayChangerBase::SetAttributeChangers(TMap<FGameplayAttribute, FAttributeChangerArray> Attributes)
{
	for (auto Element : AttributeChangers)
	{
		for (auto AttributeChanger : Element.Value.ArrayList)
		{
			if (AttributeChanger)
			{
				AttributeChanger->GameplayChangerParent = nullptr;
			}
		}
	}
	AttributeChangers.Empty();
	AttributeChangers = Attributes;
	InitAttributeChangers();
}

void UGameplayChangerBase::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	TagContainer.AppendTags(LocalTagContainer);
}

bool UGameplayChangerBase::HasMatchingGameplayTag(FGameplayTag TagToCheck) const
{
	return LocalTagContainer.HasTag(TagToCheck);
}

bool UGameplayChangerBase::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	for (const auto Tag : TagContainer)
	{
		if (!LocalTagContainer.HasTag(Tag))
			return false;
	}
	return true;
}

bool UGameplayChangerBase::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	for (const auto Tag : TagContainer)
	{
		if (LocalTagContainer.HasTag(Tag))
			return true;
	}
	return false;
}

void UGameplayChangerBase::TriggerRefreshAttribute(const FGameplayAttribute Attribute)
{
	if(AbilitySystemComponentBase.IsValid())
	{
		AbilitySystemComponentBase->RefreshAttribute(Attribute);
	}
}

void UGameplayChangerBase::TickGameplayChanger(const float DeltaTime)
{
	if(bTickEnable)
	{
		Tick(DeltaTime);
	}
}

void UGameplayChangerBase::OnAddedToGAS_Comp(UAbilitySystemComponentBase* Parent)
{
	
	if(AbilitySystemComponentBase.IsValid())
	{
		AbilitySystemComponentBase->AddGameplayTags(TagToAddDuringGameplayChanger);
	}
	OnAdded(Parent);
	
}

void UGameplayChangerBase::OnRemovedToGAS_Comp(UAbilitySystemComponentBase* Parent)
{
	
	if(AbilitySystemComponentBase.IsValid())
	{
		AbilitySystemComponentBase->RemoveGameplayTags(TagToAddDuringGameplayChanger);
	}
	
	OnRemoved(Parent);
	
}

ACharacter* UGameplayChangerBase::GetPlayerCharacter()
{
	if(AbilitySystemComponentBase.IsValid())
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(AbilitySystemComponentBase.Get(), 0);
		return PC ? Cast<ACharacter>(PC->GetPawn()) : nullptr;
	}
	return nullptr;
}

void UGameplayChangerBase::PreprocessGameplayEvent_Implementation(UPARAM(ref) FGameplayEventData& Context, FGameplayEventData& Result)
{
	Result = Context;
	return ;
}
