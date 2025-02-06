// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlueprintBase.h"
#include "AbilitySystemComponentBase.h"
#include "BT_Task_Blueprint_Base.generated.h"

/**
 * 
 */
UCLASS()
class GAS_UTILITY_API UBT_Task_Blueprint_Base : public UBTTask_BlueprintBase
{
	GENERATED_UCLASS_BODY()
	UFUNCTION(BlueprintCallable, BlueprintPure, Category=Editor)
	FString GetCurrentNodeName() const { return NodeName; }

	UFUNCTION(BlueprintCallable, Category=Editor)
	void SetNodeName(FString NewName);

	UFUNCTION(BlueprintNativeEvent, Category=Editor)
	FString GetCustomDescription();

	UFUNCTION(BlueprintNativeEvent, Category=Editor)
	void OnNodeCreatedBP();

	UFUNCTION(BlueprintNativeEvent, Category=Editor)
	FName GetNodeIconBpName() const;

	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual FString GetStaticDescription() const;

	UPROPERTY(BlueprintReadWrite,Category=BehaviourTree)
	UBT_Composite_Sequence_Blueprint* Saved_Aether_Action_SequenceNode;
	
	UFUNCTION(BlueprintCallable,BlueprintPure,Category=BehaviourTree)
	UBT_Composite_Sequence_Blueprint* GetActionNode();
public:
#if WITH_EDITOR
	virtual void OnNodeCreated() override;
	virtual FName GetNodeIconName() const override;
#endif
	UPROPERTY(BlueprintReadWrite, Category=Abort)
	bool bAbortLatentActionWhenReceiveAbort;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintPure, Category="BehaviourTree")
	bool GetForceThisTask(UAbilitySystemComponentBase* Comp);

	UFUNCTION(BlueprintSetter, Category="BehaviourTree")
	void SetForceThisTask(const bool NewValue);
	
	UPROPERTY(BlueprintReadWrite,BlueprintSetter=SetForceThisTask, Category="BehaviourTree")
	bool bForceThisTask;

	
};
