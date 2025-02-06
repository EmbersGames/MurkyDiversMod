// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AttributeChanger.h"
#include "AbilitySystem/GameplayChangerBase.h"

float UAttributeChanger::GetCumulativeMultiplier_Implementation()
{
	return GetCumulativeMultiplier_imp();
}

void UAttributeChanger::SetOperation(TEnumAsByte<EAttributeChanger::Type> NewOperation)
{
	if (Operation != NewOperation)
	{
		Operation = NewOperation;
		TriggerRefreshAttribute();
	}
}

TEnumAsByte<EAttributeChanger::Type> UAttributeChanger::GetOperation()
{
	return Operation;
}

void UAttributeChanger::SetPriorityValue(float NewValue)
{
	if (Priority != NewValue)
	{
		Priority = NewValue;
		TriggerRefreshAttribute();
	}
}

float UAttributeChanger::GetPriorityValue()
{
	return Priority;
}

void UAttributeChanger::SetFloatValue(float NewValue)
{
	if (FloatValue != NewValue)
	{
		FloatValue = NewValue;
		TriggerRefreshAttribute();
	}
}

float UAttributeChanger::GetFloatValue()
{
	return FloatValue;
}

void UAttributeChanger::TriggerRefreshAttribute()
{
	if (!bPreventTriggerRefreshAttribute)
	{
		if (GameplayChangerParent.IsValid())
		{
			GameplayChangerParent->TriggerRefreshAttribute(GameplayAttribute);
		}
	}
}

float UAttributeChanger::GetCumulativeMultiplier_imp()
{
	return FloatValue;
}

float UAttributeChanger::ExecuteChangeOperation(const float AttributeCalculationValue, UAbilitySystemComponent* AttributeOwnerComp)
{
	switch (Operation)
	{
	case EAttributeChanger::Additive:
		return AttributeCalculationValue + FloatValue;
	case EAttributeChanger::Multiplicative:
		return AttributeCalculationValue * FloatValue;
	case EAttributeChanger::CumulativeMultiplier:
		return AttributeCalculationValue;
	case EAttributeChanger::Division:
		return AttributeCalculationValue / (FloatValue == 0.f ? FloatValue + 0.001f : FloatValue);
	case EAttributeChanger::SetValue:
		return FloatValue;
	case EAttributeChanger::Base:
		return FloatValue;
	case EAttributeChanger::Disabled:
		return AttributeCalculationValue;
	}
	return AttributeCalculationValue;
}
