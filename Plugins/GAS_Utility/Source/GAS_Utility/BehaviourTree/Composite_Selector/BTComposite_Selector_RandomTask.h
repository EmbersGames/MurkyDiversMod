// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Composites/BTComposite_Selector.h"
#include "BTComposite_Selector_RandomTask.generated.h"


struct FBTSelector_RandomTaskMemory : public FBTCompositeMemory
{
	TArray<int32> ChildrenID;
};

/**
 * 
 */
UCLASS()
class GAS_UTILITY_API UBTComposite_Selector_RandomTask : public UBTComposite_Selector
{
	GENERATED_UCLASS_BODY()
	
public: 
	virtual int32 GetNextChildHandler(FBehaviorTreeSearchData& SearchData, int32 PrevChild, EBTNodeResult::Type LastResult) const override;
	virtual uint16 GetInstanceMemorySize() const override;
	
	virtual void NotifyNodeActivation(FBehaviorTreeSearchData& SearchData) const override;
};
