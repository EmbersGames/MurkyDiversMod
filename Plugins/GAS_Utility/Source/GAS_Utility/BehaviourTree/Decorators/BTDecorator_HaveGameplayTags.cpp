// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_HaveGameplayTags.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemInterface.h"
#include "AIController.h"
#include "GameplayTagAssetInterface.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"

UBTDecorator_HaveGameplayTags::UBTDecorator_HaveGameplayTags() : Super()
{
	TagsToMatch = EGameplayContainerMatchType::All;
	bAllowAbortNone = true;
	bAllowAbortLowerPri = true;
	bAllowAbortChildNodes = true;
	//bNotifyTick = true;
	bCreateNodeInstance = true;
	bNotifyBecomeRelevant = true;
	bNotifyCeaseRelevant = true;
	SavedOwnerComp = nullptr;
}

bool UBTDecorator_HaveGameplayTags::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (BlackboardComp == NULL)
	{
		return false;
	}
	UObject* Object = BlackboardComp->GetValue<UBlackboardKeyType_Object>(ActorToCheck.GetSelectedKeyID());
		if (!Object)
		return false;
	IGameplayTagAssetInterface* GameplayTagAssetInterface = Cast<IGameplayTagAssetInterface>(Object);
	if (GameplayTagAssetInterface == NULL)
	{
		IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(Object);
		if (AbilitySystemInterface == NULL)
			return false;
		UAbilitySystemComponent* TagComp = AbilitySystemInterface->GetAbilitySystemComponent();
		if (TagComp)
			GameplayTagAssetInterface = Cast<IGameplayTagAssetInterface>(TagComp);

		if (GameplayTagAssetInterface == NULL)
			return false;
	}
int i =0;
	switch (TagsToMatch)
	{
	case EGameplayContainerMatchType::All:
		return GameplayTagAssetInterface->HasAllMatchingGameplayTags(GameplayTags) && !GameplayTagAssetInterface->HasAnyMatchingGameplayTags(ExcludedTags);

	case EGameplayContainerMatchType::Any:
		if (!ExcludedTags.IsEmpty())
			return GameplayTagAssetInterface->HasAnyMatchingGameplayTags(GameplayTags) || (!GameplayTagAssetInterface->HasAnyMatchingGameplayTags(ExcludedTags));

		return GameplayTagAssetInterface->HasAnyMatchingGameplayTags(GameplayTags);

	default:
		{
			UE_LOG(LogBehaviorTree, Warning, TEXT("Invalid value for TagsToMatch (EGameplayContainerMatchType) %d.  Should only be Any or All."), static_cast<int32>(TagsToMatch));
			return false;
		}
	}
}

#if WITH_EDITOR
FString UBTDecorator_HaveGameplayTags::GetStaticDescription() const
{
	return "";
}

FName UBTDecorator_HaveGameplayTags::GetNodeIconName() const
{
	return "GraphEditor.Macro.IsValid_16x";
	return Super::GetNodeIconName();
}
#endif

void UBTDecorator_HaveGameplayTags::GameplayTagCallback(const FGameplayTag Tag, int32 NewCount)
{
	if(SavedOwnerComp && IsValid(SavedOwnerComp))
	SavedOwnerComp->RequestExecution(this);
}

void UBTDecorator_HaveGameplayTags::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);
	
}

void UBTDecorator_HaveGameplayTags::OnInstanceCreated(UBehaviorTreeComponent& OwnerComp)
{
	SavedOwnerComp = &OwnerComp;
	Super::OnInstanceCreated(OwnerComp);
}

void UBTDecorator_HaveGameplayTags::OnInstanceDestroyed(UBehaviorTreeComponent& OwnerComp)
{
	if(!IsValid(OwnerComp.GetAIOwner()))
		return;
	AActor* OwnerActor = OwnerComp.GetAIOwner()->GetPawn();

	if (OwnerActor)
	{
		UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwnerActor);

		if (AbilitySystemComponent)
		{
			for (auto Element : Delegates)
			{
				AbilitySystemComponent->RegisterGameplayTagEvent(Element.Key).Remove(Element.Value);
			}
			Delegates.Empty();
		}
	}
	Super::OnInstanceDestroyed(OwnerComp);
}

void UBTDecorator_HaveGameplayTags::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);
	
	if(!IsValid(OwnerComp.GetAIOwner()))
		return;
	AActor* OwnerActor = OwnerComp.GetAIOwner()->GetPawn();
	
	if (OwnerActor)
	{
		UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwnerActor);

		if (AbilitySystemComponent)
		{
			for (auto Element : ExcludedTags)
			{
				auto Handler = AbilitySystemComponent->RegisterGameplayTagEvent(Element).AddUObject(this, &UBTDecorator_HaveGameplayTags::GameplayTagCallback);
				Delegates.Add(Element,Handler);
			}
			
			for (auto Element : GameplayTags)
			{
				auto Handler = AbilitySystemComponent->RegisterGameplayTagEvent(Element).AddUObject(this, &UBTDecorator_HaveGameplayTags::GameplayTagCallback);
				Delegates.Add(Element,Handler);
			}
		}
	}
	
}

void UBTDecorator_HaveGameplayTags::OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnCeaseRelevant(OwnerComp, NodeMemory);
	
	if(!IsValid(OwnerComp.GetAIOwner()))
		return;
	AActor* OwnerActor = OwnerComp.GetAIOwner()->GetPawn();

	if (OwnerActor)
	{
		UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwnerActor);

		if (AbilitySystemComponent)
		{
			for (auto Element : Delegates)
			{
				AbilitySystemComponent->RegisterGameplayTagEvent(Element.Key).Remove(Element.Value);
			}
			Delegates.Empty();
		}
	}
	Super::OnInstanceDestroyed(OwnerComp);
}

void UBTDecorator_HaveGameplayTags::OnNodeActivation(FBehaviorTreeSearchData& SearchData)
{
	Super::OnNodeActivation(SearchData);
}
