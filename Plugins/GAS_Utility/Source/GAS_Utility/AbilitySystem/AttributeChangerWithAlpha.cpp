// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AttributeChangerWithAlpha.h"

UAttributeChangerWithAlpha::UAttributeChangerWithAlpha(): UAttributeChanger()
{
	AlphaValue = 1.f;
}

void UAttributeChangerWithAlpha::SetAlphaValue(float NewValue)
{
	if (NewValue != AlphaValue)
	{
		AlphaValue = NewValue;
		TriggerRefreshAttribute();
	}
}

float UAttributeChangerWithAlpha::GetAlphaValue() const
{
	return AlphaValue;
}

float UAttributeChangerWithAlpha::ExecuteChangeOperation(const float AttributeCalculationValue, UAbilitySystemComponent* AttributeOwnerComp)
{
	const float TempFloatValue = FMath::Lerp(1.0, FloatValue, AlphaValue);

	switch (Operation)
	{
	case EAttributeChanger::Additive:
		return AttributeCalculationValue + (FloatValue * AlphaValue);
	case EAttributeChanger::Multiplicative:
		return AttributeCalculationValue * TempFloatValue;
	case EAttributeChanger::CumulativeMultiplier:
		return AttributeCalculationValue;
	case EAttributeChanger::Division:
		return AttributeCalculationValue / (TempFloatValue == 0.f ? TempFloatValue + 0.001f : TempFloatValue);
	case EAttributeChanger::SetValue:
		return FMath::Lerp(AttributeCalculationValue, FloatValue, AlphaValue);
	case EAttributeChanger::Base:
		return FMath::Lerp(AttributeCalculationValue, FloatValue, AlphaValue);
	case EAttributeChanger::Disabled:
		return AttributeCalculationValue;
	}
	return AttributeCalculationValue;
}

float UAttributeChangerWithAlpha::GetCumulativeMultiplier_imp()
{
	return Super::GetCumulativeMultiplier() * AlphaValue;
}
