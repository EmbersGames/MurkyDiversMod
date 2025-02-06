// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/AttributeChanger.h"
#include "AttributeChangerWithAlpha.generated.h"

/**
 * 
 */
UCLASS()
class GAS_UTILITY_API UAttributeChangerWithAlpha : public UAttributeChanger
{
	GENERATED_BODY()

public:
	UAttributeChangerWithAlpha();

	UFUNCTION(BlueprintCallable, BlueprintSetter, Category="GAS_Utility")
	void SetAlphaValue(float NewValue);

	UFUNCTION(BlueprintGetter, BlueprintPure, Category="GAS_Utility")
	float GetAlphaValue() const;

	UPROPERTY(EditAnywhere, Category="GAS_Utility", BlueprintSetter="SetAlphaValue", BlueprintGetter="GetAlphaValue")
	float AlphaValue;
	
	virtual float ExecuteChangeOperation(const float AttributeCalculationValue, UAbilitySystemComponent* AttributeOwnerComp) override;

	virtual float GetCumulativeMultiplier_imp() override;
};
