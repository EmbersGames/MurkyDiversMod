// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/GameplayChangerBase.h"
#include "GameplayChangerWithAlpha.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEndBlendDelegate);

UCLASS()
class GAS_UTILITY_API UGameplayChangerWithAlpha : public UGameplayChangerBase
{
	GENERATED_BODY()
public:
	UGameplayChangerWithAlpha();


	UFUNCTION(BlueprintCallable, BlueprintSetter, Category="GAS_Utility")
	void SetAlphaValue(float NewValue);

	UFUNCTION(BlueprintGetter, BlueprintPure, Category="GAS_Utility")
	float GetAlphaValue() const;

	UPROPERTY(EditAnywhere, Category="GAS_Utility", BlueprintSetter="SetAlphaValue", BlueprintGetter="GetAlphaValue")
	float AlphaValue;

	virtual void TickGameplayChanger(const float DeltaTime) override;
	virtual void HandleBlendTick(const float DeltaTime);

	UFUNCTION(BlueprintCallable, Category="Blend")
	void BlendIn(const float Duration = -1.f);

	UFUNCTION(BlueprintCallable, Category="Blend")
	void BlendOut(const float Duration = -1.f, const bool RemoveGameplayChangerAtBlendOutEnd = false);

	UFUNCTION(BlueprintPure, BlueprintNativeEvent, Category="Blend")
	float GetBlendInAlphaValue(float BlendAlpha);
	
	UFUNCTION(BlueprintPure, BlueprintNativeEvent, Category="Blend")
	float GetBlendOutAlphaValue(float BlendAlpha);

public:
	UPROPERTY(BlueprintReadWrite, Category="Blend")
	float BlendingAlpha;

	UPROPERTY(BlueprintReadWrite, Category="Blend")
	bool bIsBlendingOut;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Blend")
	float BlendOutDuration;

	UPROPERTY(BlueprintReadWrite, Category="Blend")
	bool bIsBlendingIn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Blend")
	float BlendInDuration;

	UPROPERTY(BlueprintReadWrite, Category="Blend")
	bool bRemoveGameplayChangerAfterBlendOut;

	UPROPERTY(BlueprintAssignable, Category="Blend")
	FEndBlendDelegate OnBlendingInEnd;
	
	UPROPERTY(BlueprintAssignable, Category="Blend")
	FEndBlendDelegate OnBlendingOutEnd;
	
	bool bForceTickEnableByTick;
};
