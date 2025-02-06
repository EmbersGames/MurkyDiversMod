// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/GameplayChangerWithAlpha.h"

#include "AbilitySystemComponentBase.h"
#include "AbilitySystem/AttributeChangerWithAlpha.h"
#include "AbilitySystem/AttributeChanger.h"

UGameplayChangerWithAlpha::UGameplayChangerWithAlpha(): Super()
{
	AlphaValue = 1.f;
	bIsBlendingOut = false;
	bIsBlendingIn = false;
	bForceTickEnableByTick = false;
	bRemoveGameplayChangerAfterBlendOut = false;
	BlendOutDuration = 1.f;
	BlendInDuration = 1.f;
}

void UGameplayChangerWithAlpha::SetAlphaValue(float NewValue)
{
	if (AlphaValue != NewValue)
	{
		AlphaValue = NewValue;
		for (auto AttributeChangerKeyValue : AttributeChangers)
		{
			FAttributeChangerArray AttributeChangerArray = AttributeChangerKeyValue.Value;
			for (TObjectPtr<UAttributeChanger> AttributeChanger : AttributeChangerArray.ArrayList)
			{
				if (AttributeChanger)
				{
					UAttributeChangerWithAlpha* AttributeChangerWithAlpha = Cast<UAttributeChangerWithAlpha>(AttributeChanger.Get());
					if (AttributeChangerWithAlpha)
					{
						AttributeChangerWithAlpha->SetAlphaValue(AlphaValue);
					}
				}
			}
		}
	}
}

float UGameplayChangerWithAlpha::GetAlphaValue() const
{
	return AlphaValue;
}

void UGameplayChangerWithAlpha::TickGameplayChanger(const float DeltaTime)
{
	Super::TickGameplayChanger(DeltaTime);
	if (bIsBlendingIn || bIsBlendingOut)
		HandleBlendTick(DeltaTime);
}

void UGameplayChangerWithAlpha::HandleBlendTick(const float DeltaTime)
{
	if (bIsBlendingIn)
	{
		const float Speed = 1.f / FMath::Max(BlendInDuration, 0.016);
		BlendingAlpha += (Speed * DeltaTime);
		if (BlendingAlpha >= 1.f)
		{
			BlendingAlpha = 1.f;
			bIsBlendingIn = false;
			OnBlendingInEnd.Broadcast();
			if (bForceTickEnableByTick)
			{
				bTickEnable = false;
				bForceTickEnableByTick = false;
			}
			SetAlphaValue(BlendingAlpha);
		}
		else
		{
			SetAlphaValue(GetBlendInAlphaValue(BlendingAlpha));
		}
	}
	else if (bIsBlendingOut)
	{
		const float Speed = 1.f / FMath::Max(BlendOutDuration, 0.016);
		BlendingAlpha -= (Speed * DeltaTime);
		if (BlendingAlpha <= 0.f)
		{
			BlendingAlpha = 0.f;
			bIsBlendingOut = false;
			OnBlendingOutEnd.Broadcast();
			if (bRemoveGameplayChangerAfterBlendOut)
			{
				if (AbilitySystemComponentBase.IsValid())
				{
					AbilitySystemComponentBase->RemoveGameplayChanger(this);
					return;
				}
			}
			if (bForceTickEnableByTick)
			{
				bTickEnable = false;
				bForceTickEnableByTick = false;
			}
			SetAlphaValue(BlendingAlpha);
		}
		else
		{
			SetAlphaValue(GetBlendOutAlphaValue(BlendingAlpha));
		}
	}
}

void UGameplayChangerWithAlpha::BlendIn(const float Duration)
{
	bIsBlendingIn = true;
	bIsBlendingOut = false;
	if (AlphaValue == 1.f)
		AlphaValue = 0.f;
	BlendingAlpha = AlphaValue;
	if (Duration > 0)
		BlendInDuration = Duration;
	if (!bTickEnable)
	{
		bTickEnable = true;
		bForceTickEnableByTick = true;
	}
}

void UGameplayChangerWithAlpha::BlendOut(const float Duration, const bool RemoveGameplayChangerAtBlendOutEnd)
{
	bIsBlendingOut = true;
	bIsBlendingIn = false;
	if (AlphaValue == 0.f)
		AlphaValue = 1.f;
	BlendingAlpha = AlphaValue;
	if (Duration > 0)
		BlendOutDuration = Duration;
	if (!bTickEnable)
	{
		bTickEnable = true;
		bForceTickEnableByTick = true;
	}
	bRemoveGameplayChangerAfterBlendOut = RemoveGameplayChangerAtBlendOutEnd;
}

float UGameplayChangerWithAlpha::GetBlendOutAlphaValue_Implementation(float BlendAlpha)
{
	return BlendAlpha;
}

float UGameplayChangerWithAlpha::GetBlendInAlphaValue_Implementation(float BlendAlpha)
{
	return BlendAlpha;
}
