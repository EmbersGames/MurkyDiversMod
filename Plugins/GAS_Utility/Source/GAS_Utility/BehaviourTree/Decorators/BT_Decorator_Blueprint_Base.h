// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_BlueprintBase.h"
#include "BehaviourTree/Tasks/BT_Task_Blueprint_Base.h"
#include "BT_Decorator_Blueprint_Base.generated.h"

/**
 * 
 */
UCLASS()
class GAS_UTILITY_API UBT_Decorator_Blueprint_Base : public UBTDecorator_BlueprintBase
{
	GENERATED_BODY()
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;

public:
	UFUNCTION(BlueprintNativeEvent,Category=Editor)
	void InitializeInBlueprint(AActor* Controller, UObject* InParentNode);

	UFUNCTION(BlueprintCallable,Category=BehaviourTree)
	UObject* GetFirstParentNodeDecoratorOfClass(UClass* ParentClass);

	UFUNCTION(BlueprintCallable,Category=BehaviourTree)
	UObject* GetMyParentNode();

	UFUNCTION(BlueprintCallable,Category=BehaviourTree)
	TArray<UBTDecorator*> GetOtherDecorators() const;

	void CollectDescendantTasks(UBTCompositeNode* CompositeNode, TArray<UBT_Task_Blueprint_Base*>& DescendantTasks);
	
	UFUNCTION(BlueprintCallable,Category=BehaviourTree)
	bool GetDecoratorTestValue(AAIController* Controller);

	// Display Editor Functions
	UFUNCTION(BlueprintCallable, BlueprintPure, Category=Editor)
	FString GetCurrentNodeName() const { return NodeName; }

	UFUNCTION(BlueprintCallable, Category=Editor)
	void SetNodeName(FString NewName);
#if WITH_EDITOR
	virtual FString GetStaticDescription() const override;
#endif
	UFUNCTION(BlueprintNativeEvent, Category=Editor)
	FString GetCustomDescription();
	// ==========

	UFUNCTION(BlueprintCallable, BlueprintPure,Category=BehaviourTree)
	UObject* GetInstanceParentNode();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category=FlowControl)
	TEnumAsByte<EBTFlowAbortMode::Type> GetFlowAbortMode() const { return FlowAbortMode; }

	UObject* RecursiveSearchParentNodeDecoratorOfClass(UBTCompositeNode* Node, UBTCompositeNode* CurParentNode, UClass* ParentClass);

	UFUNCTION(BlueprintCallable,BlueprintPure,Category=BehaviourTree)
	UBT_Composite_Sequence_Blueprint* GetActionNode();
	
	UPROPERTY(BlueprintReadWrite,Category=BehaviourTree)
	UBT_Composite_Sequence_Blueprint* Saved_Aether_Action_SequenceNode;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable,Category=BehaviourTree)
	void GetForceActionData( bool& bThereIsAnActionToForce, bool& bForceThisAction);

};
