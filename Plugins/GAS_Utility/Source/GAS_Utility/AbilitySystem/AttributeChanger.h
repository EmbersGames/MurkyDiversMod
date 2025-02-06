// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "UObject/NoExportTypes.h"
#include "AttributeChanger.generated.h"


class UGameplayChangerBase;
UENUM(BlueprintType)
namespace EAttributeChanger
{
	/** Defines the ways that mods will modify attributes. Numeric ones operate on the existing value, override ignores it */
	enum Type
	{
		/** Numeric. */
		Additive = 0 UMETA(DisplayName="Add"),
		/** Numeric. */
		Multiplicative UMETA(DisplayName = "Multiply"),
		/** Gather all Cumulative Multiplier at the same priority, add +1 and multiply to the current value. */
		CumulativeMultiplier UMETA(DisplayName = "Cumulative Multiply"),
		/** Numeric. */
		Division UMETA(DisplayName = "Divide"),

		/** Set the Current value of the Attribute and Ignore lower priority attribute changer*/
		SetValue UMETA(DisplayName="SetValue"),
		/** Set the Base value of the Attribute*/
		Base UMETA(DisplayName="SetBase"),
		/*No Effect */
		Disabled UMETA(DisplayName="Disabled")
	};
}

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable, EditInlineNew,meta=(ScriptName="UAttributeChanger"))
class GAS_UTILITY_API UAttributeChanger : public UObject
{
	GENERATED_BODY()

public :
	UAttributeChanger()
	{
		Operation = EAttributeChanger::Additive;
		FloatValue = 0.f;
		Priority = 0.0f;
		GameplayChangerParent = nullptr;
		bPreventTriggerRefreshAttribute = false;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GAS_Utility")
	FGameplayTag UniqueAttributeChangerTag;

	UPROPERTY(EditAnywhere, Category="GAS_Utility", BlueprintSetter="SetOperation", BlueprintGetter="GetOperation")
	TEnumAsByte<EAttributeChanger::Type> Operation;

	UPROPERTY(EditAnywhere, Category="GAS_Utility", BlueprintSetter="SetFloatValue", BlueprintGetter="GetFloatValue")
	float FloatValue;

	/* First Base value operation 
	 * Then Order Operation by Priority sort ascent (high priority done at end)
	 * Set value Force ignore lower Priority 
	 */
	UPROPERTY(EditAnywhere, Category="GAS_Utility", BlueprintSetter="SetPriorityValue", BlueprintGetter="GetPriorityValue")
	float Priority;

	UFUNCTION(BlueprintCallable,BlueprintSetter, Category="GAS_Utility")
	void SetOperation(TEnumAsByte<EAttributeChanger::Type> NewOperation);

	UFUNCTION(BlueprintGetter, BlueprintPure, Category="GAS_Utility")
	TEnumAsByte<EAttributeChanger::Type> GetOperation();

	UFUNCTION(BlueprintCallable,BlueprintSetter, Category="GAS_Utility")
	void SetPriorityValue(float NewValue);

	UFUNCTION(BlueprintGetter, BlueprintPure, Category="GAS_Utility")
	float GetPriorityValue();

	UFUNCTION(BlueprintCallable,BlueprintSetter, Category="GAS_Utility")
	void SetFloatValue(float NewValue);

	UFUNCTION(BlueprintGetter, BlueprintPure, Category="GAS_Utility")
	float GetFloatValue();

	UFUNCTION(BlueprintCallable, Category="GAS_Utility")
	void TriggerRefreshAttribute();

	UPROPERTY(BlueprintReadWrite, Category="GAS_Utility")
	bool bPreventTriggerRefreshAttribute;

	UFUNCTION(BlueprintPure, BlueprintNativeEvent, Category="GAS_Utility")
	float GetCumulativeMultiplier();

	virtual float GetCumulativeMultiplier_imp();
	
	UFUNCTION(BlueprintCallable, Category="GAS_Utility")
	virtual float ExecuteChangeOperation(const float AttributeCalculationValue, UAbilitySystemComponent* AttributeOwnerComp);

	bool operator<(const UAttributeChanger& Other) const
	{
		return Priority < Other.Priority;
	}

	bool operator<(const UAttributeChanger* Other) const
	{
		if (!Other)
			return false;
		return Priority < Other->Priority;
	}

	UPROPERTY(BlueprintReadOnly, Category="GAS_Utility")
	TWeakObjectPtr<UGameplayChangerBase> GameplayChangerParent;

	UPROPERTY(BlueprintReadOnly, Category="GAS_Utility")
	FGameplayAttribute GameplayAttribute;
};
