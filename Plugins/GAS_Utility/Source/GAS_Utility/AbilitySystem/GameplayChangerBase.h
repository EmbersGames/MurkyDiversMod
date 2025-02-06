// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeChanger.h"
#include "AttributeSet.h"
#include "GameplayTagAssetInterface.h"
#include "GameplayChangerBase.generated.h"

class UAbilitySystemComponentBase;

USTRUCT(BlueprintType)
struct FAttributeChangerArray
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category="AttributeChanger")
	TArray<TObjectPtr<UAttributeChanger>> ArrayList;
};

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class GAS_UTILITY_API UGameplayChangerBase : public UObject, public IGameplayTagAssetInterface
{
	GENERATED_BODY()

public:
	UGameplayChangerBase(const FGameplayTag Tag);
	UGameplayChangerBase();

	void InitAttributeChangers();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AttributeChanger")
	FGameplayTag GameplayChangerTag;

	FGameplayTagContainer LocalTagContainer;

	UPROPERTY(EditAnywhere, Category="AttributeChanger")
	TMap<FGameplayAttribute, FAttributeChangerArray> AttributeChangers;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "AttributeChanger")
	TMap<FGameplayAttribute, FAttributeChangerArray> GetAttributeChangers();

	UFUNCTION(BlueprintCallable, Category = "AttributeChanger")
	void SetAttributeChangers(TMap<FGameplayAttribute, FAttributeChangerArray> Attributes);

	UFUNCTION(BlueprintCallable, Category = GameplayTags)
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;

	UFUNCTION(BlueprintCallable, Category=GameplayTags)
	virtual bool HasMatchingGameplayTag(FGameplayTag TagToCheck) const override;

	UFUNCTION(BlueprintCallable, Category=GameplayTags)
	virtual bool HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;

	UFUNCTION(BlueprintCallable, Category=GameplayTags)
	virtual bool HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;

	UFUNCTION(BlueprintCallable, Category="GAS_Utility")
	void TriggerRefreshAttribute(FGameplayAttribute Attribute);

	virtual void TickGameplayChanger(const float DeltaTime);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category=Tick)
	void Tick(const float DeltaTime);


	void OnAddedToGAS_Comp(UAbilitySystemComponentBase* Parent);
	void OnRemovedToGAS_Comp(UAbilitySystemComponentBase* Parent);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category=GameplayChanger)
	void OnAdded(UAbilitySystemComponentBase* Parent);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category=GameplayChanger)
	void OnRemoved(UAbilitySystemComponentBase* Parent);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GAS_Utility")
	bool bTickEnable;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="GAS_Utility")
	void PreprocessGameplayEvent(UPARAM(ref) FGameplayEventData& Context, FGameplayEventData& Result);

	UPROPERTY(BlueprintReadOnly, Category="GAS_Utility")
	TWeakObjectPtr<UAbilitySystemComponentBase> AbilitySystemComponentBase;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="GAS_Utility")
	FGameplayTagContainer TagToAddDuringGameplayChanger;

	UFUNCTION(BlueprintCallable,BlueprintPure, Category="GAS_Utility")
	ACharacter* GetPlayerCharacter();
};

UINTERFACE(BlueprintType)
class UHandlePreProcessGameplayEvent : public UInterface
{
	GENERATED_BODY()
};

class GAS_UTILITY_API IHandlePreProcessGameplayEvent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "GAS_Utility")
	void AddPreProcessGameplayEvent(FGameplayTag GameplayTag, UGameplayChangerBase* Changer);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "GAS_Utility")
	void RemovePreProcessGameplayEvent(FGameplayTag GameplayTag, UGameplayChangerBase* Changer);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "GAS_Utility")
	void ClearPreProcessGameplayEvent(FGameplayTag GameplayTag, UGameplayChangerBase* Changer);
};
