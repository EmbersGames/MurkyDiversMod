// Fill out your copyright notice in the Description page of Project Settings.


#include "BehaviourTree/Decorators/BT_Decorator_Blueprint_Base.h"

#include "AbilitySystemComponentBase.h"
#include "AbilitySystemInterface.h"
#include "AIController.h"
#include "BehaviorTree/BTCompositeNode.h"
#include "BehaviourTree/Composite_Sequences/BT_Composite_Sequence_Blueprint.h"
#include "BehaviourTree/Tasks/BT_Task_Blueprint_Base.h"
#include "GameFramework/Character.h"


void UBT_Decorator_Blueprint_Base::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);
	UBTCompositeNode* Parent = GetParentNode();
	if (IsValid(Parent))
	{
		InitializeInBlueprint(ActorOwner, Parent);
		Saved_Aether_Action_SequenceNode = GetActionNode();
	}
}

UObject* UBT_Decorator_Blueprint_Base::GetFirstParentNodeDecoratorOfClass(UClass* ParentClass)
{
	UBTCompositeNode* Parent = GetParentNode();
	UBTDecorator* ResultVar = nullptr;
	if (IsValid(Parent))
	{
		UBTCompositeNode* ParentParent = Parent->GetParentNode();
		if (GetChildIndex() != MAX_uint8)
		{
			TArray<UBTDecorator*> Decorators = Parent->Children[GetChildIndex()].Decorators;
			for (UBTDecorator* Element : Decorators)
			{
				if (!ResultVar && IsValid(Element) && Element->GetClass() == ParentClass)
				{
					ResultVar = Element;
				}
			}
		}
		if (!ResultVar && IsValid(ParentParent))
		{
			ResultVar = static_cast<UBTDecorator*>(RecursiveSearchParentNodeDecoratorOfClass(Parent, ParentParent, ParentClass));
		}
	}


	const UBehaviorTreeComponent* OwnerComp = Cast<UBehaviorTreeComponent>(GetOuter());
	if (!IsValid(OwnerComp) || !IsValid(Parent))
		return ResultVar;
	const int32 InstanceIdx = OwnerComp->FindInstanceContainingNode(GetParentNode());
	if (InstanceIdx == INDEX_NONE)
		return ResultVar;
	if (IsValid(ResultVar))
	{
		uint8* NodeMemory = OwnerComp->GetNodeMemory(ResultVar, InstanceIdx);
		UBTDecorator* NodeOb = static_cast<UBTDecorator*>(ResultVar->GetNodeInstance(*OwnerComp, NodeMemory));
		if (IsValid(NodeOb))
			return NodeOb;
	}
	return ResultVar;
}

void UBT_Decorator_Blueprint_Base::SetNodeName(FString NewName)
{
	NodeName = NewName;
}
#if WITH_EDITOR
FString UBT_Decorator_Blueprint_Base::GetStaticDescription() const
{
	return const_cast<UBT_Decorator_Blueprint_Base*>(this)->GetCustomDescription();
}
#endif
FString UBT_Decorator_Blueprint_Base::GetCustomDescription_Implementation()
{
#if WITH_EDITOR
	return Super::GetStaticDescription();
#endif
	return "";
}

UObject* UBT_Decorator_Blueprint_Base::GetInstanceParentNode()
{
	UBTCompositeNode* ResultVar = static_cast<UBTCompositeNode*>(const_cast<UBTNode*>(GetMyNode()));
	return ResultVar;
}

UObject* UBT_Decorator_Blueprint_Base::RecursiveSearchParentNodeDecoratorOfClass(UBTCompositeNode* Node, UBTCompositeNode* CurParentNode, UClass* ParentClass)
{
	if (IsValid(Node) && IsValid(CurParentNode) && (GetChildIndex() != MAX_uint8))
	{
		const int32 ResIndex = CurParentNode->GetChildIndex(*Node);
		if (ResIndex >= 0)
		{
			TArray<UBTDecorator*> Decorators = CurParentNode->Children[ResIndex].Decorators;
			for (UBTDecorator* Element : Decorators)
			{
				if (IsValid(Element))
				{
					if (Element->GetClass() == ParentClass)
					{
						return Element;
					}
				}
			}
			return RecursiveSearchParentNodeDecoratorOfClass(CurParentNode, CurParentNode->GetParentNode(), ParentClass);
		}
	}
	return nullptr;
}

UBT_Composite_Sequence_Blueprint* UBT_Decorator_Blueprint_Base::GetActionNode()
{
	if (IsValid(Saved_Aether_Action_SequenceNode))
		return Saved_Aether_Action_SequenceNode;
	UObject* NodeParent = GetInstanceParentNode();
	if (IsValid(NodeParent) && IsValid(ActorOwner))
	{
		UBT_Composite_Sequence_Blueprint* SequenceNode = Cast<UBT_Composite_Sequence_Blueprint>(NodeParent);
		const auto Controller = Cast<AController>(ActorOwner);

		if (IsValid(SequenceNode) && IsValid(Controller))
		{
			Saved_Aether_Action_SequenceNode = SequenceNode;
			SequenceNode->InitialiseCompositeNode(Controller->GetPawn());
			return SequenceNode;
		}
	}
	return nullptr;
}

void UBT_Decorator_Blueprint_Base::GetForceActionData_Implementation(bool& bThereIsAnActionToForce, bool& bForceThisAction)
{
	bThereIsAnActionToForce = false;
	bForceThisAction = false;
	UBT_Composite_Sequence_Blueprint* NodeAction = GetActionNode();

	bool bFound = false;
	if (ActorOwner)
	{
		if (const auto Controller = Cast<AController>(ActorOwner))
		{
			if (const IAbilitySystemInterface* GAS_Interface = Cast<IAbilitySystemInterface>(Controller->GetPawn()))
			{
				if (UAbilitySystemComponent* GasComp = GAS_Interface->GetAbilitySystemComponent())
				{
					if (UAbilitySystemComponentBase* GAS_Utility_Comp = Cast<UAbilitySystemComponentBase>(GasComp))
					{
						bThereIsAnActionToForce = GAS_Utility_Comp->IsThereAbilityBehaviourTaskToForce();
						//	if (bThereIsAnActionToForce)
						if (UBT_Task_Blueprint_Base* Task = Cast<UBT_Task_Blueprint_Base>(const_cast<UBTNode*>(GetMyNode())))
						{
							bForceThisAction = Task->GetForceThisTask(GAS_Utility_Comp);
						}
						else if (UBTCompositeNode* LocalParentNode = static_cast<UBTCompositeNode*>(const_cast<UBTNode*>(GetMyNode())))
						{
							TArray<UBT_Task_Blueprint_Base*> DescendantTasks;
							CollectDescendantTasks(LocalParentNode, DescendantTasks);
							for (UBT_Task_Blueprint_Base* DescendantTask : DescendantTasks)
							{
								if (DescendantTask)
								{
									if (DescendantTask->GetForceThisTask(GAS_Utility_Comp))
										bForceThisAction = true;
								}
							}
						}
						if (IsInversed())
							bForceThisAction = !bForceThisAction;
						bFound = true;
					}
				}
			}
		}
	}
	if (!bFound && IsValid(NodeAction) && IsValid(ActorOwner))
	{
		const auto Controller = Cast<AController>(ActorOwner);
		if (IsValid(Controller) && IsValid(Controller->GetPawn()))
		{
			bool bLocalThereIsAnActionToForce;
			bool bLocalForceThisAction;

			NodeAction->GetForceActionData(Cast<ACharacter>(Controller->GetPawn()), bLocalThereIsAnActionToForce, bLocalForceThisAction);
			bThereIsAnActionToForce = bLocalThereIsAnActionToForce;
			if (IsInversed())
				bForceThisAction = !bLocalForceThisAction;
			else
				bForceThisAction = bLocalForceThisAction;
		}
	}
}

void UBT_Decorator_Blueprint_Base::CollectDescendantTasks(UBTCompositeNode* CompositeNode, TArray<UBT_Task_Blueprint_Base*>& DescendantTasks)
{
	if (CompositeNode)
		for (const FBTCompositeChild& Child : CompositeNode->Children)
		{
			if (Child.ChildTask)
			{
				if (UBT_Task_Blueprint_Base* Task = Cast<UBT_Task_Blueprint_Base>(Child.ChildTask))
					DescendantTasks.Add(Task);
			}
			else if (Child.ChildComposite)
			{
				CollectDescendantTasks(Child.ChildComposite, DescendantTasks);
			}
		}
}

UObject* UBT_Decorator_Blueprint_Base::GetMyParentNode()
{
	return GetParentNode();
}

TArray<UBTDecorator*> UBT_Decorator_Blueprint_Base::GetOtherDecorators() const
{
	UBTCompositeNode* Parent = GetParentNode();
	TArray<UBTDecorator*> Decos = Parent->Children[GetChildIndex()].Decorators;
	TArray<UBTDecorator*> DecosResult;
	UBehaviorTreeComponent* OwnerComp = Cast<UBehaviorTreeComponent>(GetOuter());
	if (!IsValid(OwnerComp) || !IsValid(Parent))
		return DecosResult;
	const int32 InstanceIdx = OwnerComp->FindInstanceContainingNode(GetParentNode());
	if (InstanceIdx == INDEX_NONE)
		return DecosResult;

	for (UBTDecorator* Element : Decos)
	{
		uint8* NodeMemory = OwnerComp->GetNodeMemory(Element, InstanceIdx);
		UBTDecorator* NodeOb = static_cast<UBTDecorator*>(Element->GetNodeInstance(*OwnerComp, NodeMemory));
		if (IsValid(NodeOb))
			DecosResult.Add(NodeOb);
	}
	return DecosResult;
}


bool UBT_Decorator_Blueprint_Base::GetDecoratorTestValue(AAIController* Controller)
{
	if (IsValid(Controller))
		return PerformConditionCheckAI(Controller, Controller->GetPawn());
	return false;
}

void UBT_Decorator_Blueprint_Base::InitializeInBlueprint_Implementation(AActor* Controller, UObject* InParentNode)
{
}
