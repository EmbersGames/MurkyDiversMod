// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Composites/BTComposite_Sequence.h"
#include "BT_Composite_Sequence_Blueprint.generated.h"

/**
 * 
 */
UCLASS()
class GAS_UTILITY_API UBT_Composite_Sequence_Blueprint : public UBTComposite_Sequence
{
	GENERATED_UCLASS_BODY()
	/** initialize any asset related data */
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;

	UFUNCTION(BlueprintNativeEvent,Category=Editor)
	void InitializeInBlueprint();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable,Category=Editor)
	void InitialiseCompositeNode(APawn* Owner);

	virtual int32 GetNextChildHandler(struct FBehaviorTreeSearchData& SearchData, int32 PrevChild, EBTNodeResult::Type LastResult) const override;

	virtual bool CanNotifyDecoratorsOnFailedActivation(FBehaviorTreeSearchData& SearchData, int32 ChildIdx, EBTNodeResult::Type& NodeResult) const override;

	virtual void NotifyNodeDeactivation(FBehaviorTreeSearchData& SearchData, EBTNodeResult::Type& NodeResult) const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable,Category=BehaviourTree)
	void GetForceActionData(ACharacter* Me, bool& bThereIsAnActionToForce, bool& bLocalForceThisAction);

	UFUNCTION(BlueprintNativeEvent,Category=Editor)
	FName GetNodeIconBpName() const;
     
public:
	/** @return string containing description of this node with all setup values */
	virtual FString GetStaticDescription() const override;

	UFUNCTION(BlueprintNativeEvent, Category=Editor)
	FString GetCustomDescription();


	UFUNCTION(BlueprintCallable, Category=Editor)
	void SetNodeName(FString NewName);

	UFUNCTION(BlueprintCallable,Category=BehaviourTree)
	UBehaviorTreeComponent* GetOwnerComp() const;

	UPROPERTY()
	UBehaviorTreeComponent* SavedOwnerComp;

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere,Category=BehaviourTree)
	FName ActionName;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category=BehaviourTree)
	bool bForceThisAction;
	
#if WITH_EDITOR
	virtual FName GetNodeIconName() const override;
#endif
	
};
