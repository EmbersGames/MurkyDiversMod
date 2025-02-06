// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_CheckGameplayTagsOnActor.h"
#include "BTDecorator_HaveGameplayTags.generated.h"

/**
 * 
 */
UCLASS()
class GAS_UTILITY_API UBTDecorator_HaveGameplayTags : public UBTDecorator_CheckGameplayTagsOnActor
{
	GENERATED_BODY()
public:
	UBTDecorator_HaveGameplayTags();

	UPROPERTY(EditAnywhere, Category=GameplayTagCheck)
	FGameplayTagContainer ExcludedTags;

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
#if WITH_EDITOR
	virtual FString GetStaticDescription() const override;
	virtual FName GetNodeIconName() const override;
#endif
	virtual void GameplayTagCallback(const FGameplayTag Tag, int32 NewCount);
protected:
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	virtual void OnInstanceCreated(UBehaviorTreeComponent& OwnerComp) override;
	
	/** called when node instance is removed from tree */
	virtual void OnInstanceDestroyed(UBehaviorTreeComponent& OwnerComp) override;
	
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnNodeActivation(FBehaviorTreeSearchData& SearchData) override;
	TMap<FGameplayTag,FDelegateHandle> Delegates;
	UPROPERTY()
	UBehaviorTreeComponent* SavedOwnerComp;
	
};
