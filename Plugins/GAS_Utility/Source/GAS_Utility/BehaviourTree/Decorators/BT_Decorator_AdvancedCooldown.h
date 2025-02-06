// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_Cooldown.h"
#include "BT_Decorator_AdvancedCooldown.generated.h"

/**
 * 
 */
UCLASS()
class GAS_UTILITY_API UBT_Decorator_AdvancedCooldown : public UBTDecorator_Cooldown
{
	GENERATED_UCLASS_BODY()
	public :
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

	void InitDefaultValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);
#if WITH_EDITOR
	virtual FString GetStaticDescription() const override;
#endif
	void RefreshNodeName();
	
	UFUNCTION()
	UObject* GetInstanceParentNode() const;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category=Settings)
	bool bInCooldownByDefault;
	
	UPROPERTY(Category=Decorator, EditAnywhere)
	float CoolDownTimeAtStart;

	UPROPERTY(Category=Decorator,EditAnywhere,BlueprintReadOnly)
	FGameplayTag GameplayTag;
	
	virtual void SetOwner(AActor* ActorOwner) override;
	UPROPERTY()
	AActor* Owner;

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual void OnNodeDeactivation(FBehaviorTreeSearchData& SearchData, EBTNodeResult::Type NodeResult) override;
};
