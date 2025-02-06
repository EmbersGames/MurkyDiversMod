// Fill out your copyright notice in the Description page of Project Settings.


#include "BehaviourTree/Composite_Selector/BTComposite_Selector_RandomTask.h"

UBTComposite_Selector_RandomTask::UBTComposite_Selector_RandomTask(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NodeName = "Random Task Selector";
	INIT_COMPOSITE_NODE_NOTIFY_FLAGS();
}

int32 UBTComposite_Selector_RandomTask::GetNextChildHandler(FBehaviorTreeSearchData& SearchData, int32 PrevChild, EBTNodeResult::Type LastResult) const
{
	// success = quit
	int32 NextChildIdx = BTSpecialChild::ReturnToParent;
	if (GetChildrenNum() == 0)
		return NextChildIdx;
	FBTSelector_RandomTaskMemory* Memory = GetNodeMemory<FBTSelector_RandomTaskMemory>(SearchData);

	int RandomIndex = FMath::RandRange(0, Memory->ChildrenID.Num() - 1);

	if (PrevChild == BTSpecialChild::NotInitialized)
	{
		// newly activated: start from first
		//NextChildIdx = FMath::RandRange(0, GetChildrenNum() - 1);
		NextChildIdx = Memory->ChildrenID[RandomIndex];
	}
	else if (LastResult == EBTNodeResult::Failed)
	{
		Memory->ChildrenID.Remove(PrevChild);
		if (!Memory->ChildrenID.IsEmpty())
		{
			RandomIndex = FMath::RandRange(0, Memory->ChildrenID.Num() - 1);
			NextChildIdx = Memory->ChildrenID[RandomIndex];
			/*for (int i = 0; i < GetChildrenNum(); i++)
			{
				Memory->ChildrenID.Add(i);
			}*/ 
		}

		/*
		// failed = choose next child
		NextChildIdx = FMath::RandRange(0, GetChildrenNum() - 1);
		if (NextChildIdx == PrevChild)
		{
			NextChildIdx++;
			if (NextChildIdx >= GetChildrenNum())
			{
				NextChildIdx = 0;
			}
		}*/
		//PrevChild = NextChildIdx + 1;
	}
	//UE_LOG(LogTemp, Warning, TEXT("Result = %d  et PrevChild = %d and LastResult = %d"), NextChildIdx,PrevChild,LastResult);

	return NextChildIdx;
}

uint16 UBTComposite_Selector_RandomTask::GetInstanceMemorySize() const
{
	return sizeof(FBTSelector_RandomTaskMemory);
}

void UBTComposite_Selector_RandomTask::NotifyNodeActivation(FBehaviorTreeSearchData& SearchData) const
{
	FBTSelector_RandomTaskMemory* Memory = GetNodeMemory<FBTSelector_RandomTaskMemory>(SearchData);
	Memory->ChildrenID.Empty();
	for (int i = 0; i < GetChildrenNum(); i++)
	{
		Memory->ChildrenID.Add(i);
	}
}
