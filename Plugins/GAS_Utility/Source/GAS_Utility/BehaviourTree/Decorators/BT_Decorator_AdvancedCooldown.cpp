// Fill out your copyright notice in the Description page of Project Settings.


#include "BehaviourTree/Decorators/BT_Decorator_AdvancedCooldown.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemComponentBase.h"
#include "AbilitySystemInterface.h"
#include "BehaviorTree/BTCompositeNode.h"
#include "BehaviourTree/Composite_Sequences/BT_Composite_Sequence_Blueprint.h"
#include "GameFramework/Character.h"


UBT_Decorator_AdvancedCooldown::UBT_Decorator_AdvancedCooldown(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bCreateNodeInstance = true;
	bInCooldownByDefault = false;
	CoolDownTimeAtStart = -1;
}

UObject* UBT_Decorator_AdvancedCooldown::GetInstanceParentNode() const
{
	UBTCompositeNode* ResultVar = static_cast<UBTCompositeNode*>(const_cast<UBTNode*>(GetMyNode()));
	return ResultVar;
}

void UBT_Decorator_AdvancedCooldown::SetOwner(AActor* ActorOwner)
{
	Super::SetOwner(ActorOwner);
	Owner = ActorOwner;
}

void UBT_Decorator_AdvancedCooldown::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	FBTCooldownDecoratorMemory* DecoratorMemory = CastInstanceNodeMemory<FBTCooldownDecoratorMemory>(NodeMemory);
	if (IsValid(GetInstanceParentNode()))
	{
		UBT_Composite_Sequence_Blueprint* AetherParentNode = Cast<UBT_Composite_Sequence_Blueprint>(GetInstanceParentNode());
		if (IsValid(AetherParentNode))
		{
			bool bThereIsAnActionToForce;
			bool bForceThisAction;
			ACharacter* Me = nullptr;
			if (IsValid(Owner))
			{
				const AController* ActorOwner = Cast<AController>(Owner);
				if (IsValid(ActorOwner->GetPawn()))
				{
					Me = Cast<ACharacter>(ActorOwner->GetPawn());
				}
			}
			AetherParentNode->GetForceActionData(Me, bThereIsAnActionToForce, bForceThisAction);
			if (bThereIsAnActionToForce && bForceThisAction)
			{
				DecoratorMemory->bRequestedRestart = true;
				OwnerComp.RequestExecution(this);
			}
		}
	}
	if (!DecoratorMemory->bRequestedRestart)
	{
		if (GameplayTag.IsValid() && IsValid(Owner))
		{
			if (const auto CastOwner = Cast<IAbilitySystemInterface>(Owner))
			{
				if (const auto GasComp = CastOwner->GetAbilitySystemComponent())
				{
					if (const auto CastedGasComp = Cast<UAbilitySystemComponentBase>(GasComp))
					{
						if (CastedGasComp->GetCooldownFromTag(GameplayTag) <= 0)
						{
							DecoratorMemory->bRequestedRestart = true;
							OwnerComp.RequestExecution(this);
						}
					}
				}
			}
		}
		const float TimePassed = (OwnerComp.GetWorld()->GetTimeSeconds() - DecoratorMemory->LastUseTimestamp);
		if (TimePassed >= CoolDownTime)
		{
			DecoratorMemory->bRequestedRestart = true;
			OwnerComp.RequestExecution(this);
		}
	}
}

void UBT_Decorator_AdvancedCooldown::OnNodeDeactivation(FBehaviorTreeSearchData& SearchData, EBTNodeResult::Type NodeResult)
{
	FBTCooldownDecoratorMemory* DecoratorMemory = GetNodeMemory<FBTCooldownDecoratorMemory>(SearchData);
	if (GameplayTag.IsValid() && IsValid(Owner))
	{
		if (const auto CastOwner = Cast<IAbilitySystemInterface>(Owner))
		{
			if (const auto GasComp = CastOwner->GetAbilitySystemComponent())
			{
				if (const auto CastedGasComp = Cast<UAbilitySystemComponentBase>(GasComp))
				{
					CastedGasComp->AddCooldownTag(GameplayTag, CoolDownTime);
				}
			}
		}
	}
	Super::OnNodeDeactivation(SearchData, NodeResult);
}

bool UBT_Decorator_AdvancedCooldown::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	if (IsValid(GetInstanceParentNode()))
	{
		UBT_Composite_Sequence_Blueprint* AetherParentNode = Cast<UBT_Composite_Sequence_Blueprint>(GetInstanceParentNode());
		if (IsValid(AetherParentNode))
		{
			bool bThereIsAnActionToForce;
			bool bForceThisAction;
			ACharacter* Me = nullptr;
			if (IsValid(Owner))
			{
				const AController* ActorOwner = Cast<AController>(Owner);
				if (IsValid(ActorOwner->GetPawn()))
				{
					Me = Cast<ACharacter>(ActorOwner->GetPawn());
				}
			}
			AetherParentNode->GetForceActionData(Me, bThereIsAnActionToForce, bForceThisAction);
			if (bThereIsAnActionToForce)
				return bForceThisAction;
		}
	}

	const_cast<UBT_Decorator_AdvancedCooldown*>(this)->InitDefaultValue(OwnerComp, NodeMemory);
	if (GameplayTag.IsValid() && IsValid(Owner))
	{
		if (const auto CastOwner = Cast<IAbilitySystemInterface>(Owner))
		{
			if (const auto GasComp = CastOwner->GetAbilitySystemComponent())
			{
				if (const auto CastedGasComp = Cast<UAbilitySystemComponentBase>(GasComp))
				{
					return CastedGasComp->GetCooldownFromTag(GameplayTag) <= 0;
				}
			}
		}
	}
	return Super::CalculateRawConditionValue(OwnerComp, NodeMemory);
}

void UBT_Decorator_AdvancedCooldown::InitDefaultValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FlowAbortMode = EBTFlowAbortMode::LowerPriority;
	if (bInCooldownByDefault)
	{
		FBTCooldownDecoratorMemory* DecoratorMemory = CastInstanceNodeMemory<FBTCooldownDecoratorMemory>(NodeMemory);
		if (DecoratorMemory && DecoratorMemory->LastUseTimestamp == DBL_MIN)
		{
			if (GameplayTag.IsValid() && IsValid(Owner))
			{
				if (const auto CastOwner = Cast<IAbilitySystemInterface>(Owner))
				{
					if (const auto GasComp = CastOwner->GetAbilitySystemComponent())
					{
						if (const auto CastedGasComp = Cast<UAbilitySystemComponentBase>(GasComp))
						{
							float Cooldown = CoolDownTime;
							if (CoolDownTimeAtStart >= 0.f)
								Cooldown = CoolDownTimeAtStart;
							CastedGasComp->AddCooldownTag(GameplayTag, Cooldown);
						}
					}
				}
			}

			float Offset = 0.f;
			if (CoolDownTimeAtStart >= 0.f)
				Offset = CoolDownTime - CoolDownTimeAtStart;
			DecoratorMemory->LastUseTimestamp = OwnerComp.GetWorld()->GetTimeSeconds() - Offset;
			DecoratorMemory->bRequestedRestart = false;
		}
	}
}
#if WITH_EDITOR
FString UBT_Decorator_AdvancedCooldown::GetStaticDescription() const
{
	const_cast<UBT_Decorator_AdvancedCooldown*>(this)->RefreshNodeName();
	return "";
}
#endif
void UBT_Decorator_AdvancedCooldown::RefreshNodeName()
{
	NodeName = "Cooldown : " + FString::SanitizeFloat(CoolDownTime, 1) + " sec";
}
