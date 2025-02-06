// Fill out your copyright notice in the Description page of Project Settings.


#include "BehaviourTree/Composite_Sequences/BT_Composite_Sequence_Blueprint.h"

#include "AbilitySystemComponentBase.h"
#include "AbilitySystemInterface.h"
#include "GameplayAbilityBase.h"
#include "BehaviourTree/Tasks/BT_Task_Blueprint_Base.h"
#include "GameFramework/Character.h"

UBT_Composite_Sequence_Blueprint::UBT_Composite_Sequence_Blueprint(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NodeName = "Combo Sequence";
	bUseNodeDeactivationNotify = true;
	bForceThisAction = false;
}

bool UBT_Composite_Sequence_Blueprint::CanNotifyDecoratorsOnFailedActivation(FBehaviorTreeSearchData& SearchData,
                                                                             int32 ChildIdx, EBTNodeResult::Type& NodeResult) const
{
	return false;
}

void UBT_Composite_Sequence_Blueprint::NotifyNodeDeactivation(FBehaviorTreeSearchData& SearchData, EBTNodeResult::Type& NodeResult) const
{
	//UE_LOG(LogTemp,Warning,TEXT(" NODE DEACTIVATION %s"),*NodeName);
}

void UBT_Composite_Sequence_Blueprint::GetForceActionData_Implementation(ACharacter* Me, bool& bThereIsAnActionToForce, bool& bLocalForceThisAction)
{
	bLocalForceThisAction = false;
	if (bForceThisAction)
	{
		bThereIsAnActionToForce = true;
		bLocalForceThisAction = true;
	}
	bThereIsAnActionToForce = false;
	bForceThisAction = false;
	if (Me)
	{
		if (const IAbilitySystemInterface* GAS_Interface = Cast<IAbilitySystemInterface>(Me))
		{
			if (auto GasComp = GAS_Interface->GetAbilitySystemComponent())
			{
				if (auto GAS_Utility_Comp = Cast<UAbilitySystemComponentBase>(GasComp))
				{
					bThereIsAnActionToForce = GAS_Utility_Comp->IsThereAbilityBehaviourTaskToForce();
					for (int i = 0; i < GetChildrenNum(); i++)
					{
						UBTNode* Child = GetChildNode(i);
						if (Child)
						{
							if (UBT_Task_Blueprint_Base* Task = Cast<UBT_Task_Blueprint_Base>(const_cast<UBTNode*>(Child)))
							{
								bLocalForceThisAction = bLocalForceThisAction || Task->GetForceThisTask(GAS_Utility_Comp);
							}
						}
					}
				}
			}
		}
	}
}

FName UBT_Composite_Sequence_Blueprint::GetNodeIconBpName_Implementation() const
{
	return FName();
}

FString UBT_Composite_Sequence_Blueprint::GetStaticDescription() const
{
	return const_cast<UBT_Composite_Sequence_Blueprint*>(this)->GetCustomDescription();
}

void UBT_Composite_Sequence_Blueprint::SetNodeName(FString NewName)
{
	NodeName = NewName;
}

UBehaviorTreeComponent* UBT_Composite_Sequence_Blueprint::GetOwnerComp() const
{
	if (IsValid(GetOuter()))
	{
		return Cast<UBehaviorTreeComponent>(GetOuter());
	}
	return SavedOwnerComp;
}

#if WITH_EDITOR
FName UBT_Composite_Sequence_Blueprint::GetNodeIconName() const
{
	const FName CustomIconName = GetNodeIconBpName();
	if (!(CustomIconName.IsNone()))
		return CustomIconName;
	return Super::GetNodeIconName();
}
#endif

FString UBT_Composite_Sequence_Blueprint::GetCustomDescription_Implementation()
{
	if (!ActionName.IsNone())
		SetNodeName(ActionName.ToString());
	return Super::GetStaticDescription();
}

void UBT_Composite_Sequence_Blueprint::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);
	InitializeInBlueprint();
}

void UBT_Composite_Sequence_Blueprint::InitializeInBlueprint_Implementation()
{
}

int32 UBT_Composite_Sequence_Blueprint::GetNextChildHandler(FBehaviorTreeSearchData& SearchData, int32 PrevChild, EBTNodeResult::Type LastResult) const
{
	// failure = quit
	int32 NextChildIdx = BTSpecialChild::ReturnToParent;

	if (PrevChild == BTSpecialChild::NotInitialized)
	{
		// newly activated: start from first
		NextChildIdx = 0;
	}
	else if ((LastResult == EBTNodeResult::Succeeded || LastResult == EBTNodeResult::Failed) && (PrevChild + 1) < GetChildrenNum())
	{
		// success = choose next child
		NextChildIdx = PrevChild + 1;
	}

	return NextChildIdx;
}
