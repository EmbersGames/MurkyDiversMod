// Embers 2024

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Async/AbilityAsync.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityAsync_WaitGameplayTags.generated.h"

UCLASS()
class GAS_UTILITY_API UAbilityAsync_WaitGameplayTags : public UAbilityAsync
{
	GENERATED_BODY()

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FAsyncWaitGameplayTagDelegate, FGameplayTag, Tag, int32, NewCount, int32, OldCount);

	UPROPERTY(BlueprintAssignable)
	FAsyncWaitGameplayTagDelegate ChangeEvent;

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (DefaultToSelf = "TargetActor", BlueprintInternalUseOnly = "TRUE"))
	static UAbilityAsync_WaitGameplayTags* WaitGameplayTagsChangeOnActor(AActor* TargetActor, FGameplayTagContainer Tags);

protected:
	virtual void Activate() override;
	virtual void EndAction() override;

	virtual void GameplayTagCallback(const FGameplayTag Tag, int32 NewCount);
	virtual void BroadcastDelegate(FGameplayTag Tag, int32 NewCount, int32 OldCount);

	FGameplayTagContainer Tags;

	TMap<FGameplayTag,FDelegateHandle> MyHandles;
	
	TMap<FGameplayTag,int32> GameplayTagsOldCount;
};

UCLASS()
class GAS_UTILITY_API UAbilityTask_WaitGameplayTags : public UAbilityTask
{
	GENERATED_UCLASS_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FAsyncWaitGameplayTagDelegate, FGameplayTag, Tag, int32, NewCount, int32, OldCount);

	UPROPERTY(BlueprintAssignable)
	FAsyncWaitGameplayTagDelegate	ChangeEvent;

	
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UAbilityTask_WaitGameplayTags* WaitGameplayTagsChange(UGameplayAbility* OwningAbility, FGameplayTagContainer Tags, AActor* InOptionalExternalTarget=nullptr);

	void SetExternalTarget(AActor* Actor);

	UAbilitySystemComponent* GetTargetASC();
protected:
	virtual void Activate() override;

	virtual void OnDestroy(bool AbilityEnding) override;
	
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> OptionalExternalTarget;
	
	bool UseExternalTarget;	
	
	virtual void GameplayTagCallback(const FGameplayTag Tag, int32 NewCount);
	virtual void BroadcastDelegate(FGameplayTag Tag, int32 NewCount, int32 OldCount);
	
	FGameplayTagContainer Tags;

	TMap<FGameplayTag,FDelegateHandle> MyHandles;
	
	TMap<FGameplayTag,int32> GameplayTagsOldCount;
	
};