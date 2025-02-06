// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "EnhancedInputComponent.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "Task_WaitEnhancedInputAction.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMkFloatMulticastDynamic, float, Value);

UCLASS()
class GAS_UTILITY_API UTask_WaitEnhancedInputAction : public UAbilityTask
{
	GENERATED_BODY()
private:
	FDelegateHandle InputDelegateHandle;

	UPROPERTY(BlueprintAssignable)
	FMkFloatMulticastDynamic OnInputPressed;

	/*
	 * Triggered is separate from Pressed (although depending on Modifier they may occur at the same time).
	 */
	UPROPERTY(BlueprintAssignable)
	FMkFloatMulticastDynamic OnInputTriggered;

	UPROPERTY(BlueprintAssignable)
	FMkFloatMulticastDynamic OnInputReleased;

	UPROPERTY()
	class UInputMappingContext* MappingContext;

	UPROPERTY()
	class UInputAction* InputAction;

	int32 PressedHandle;
	int32 TriggeredHandle;
	int32 ReleasedHandle;
public:
	/**
	 * @brief Waits for direct Input from ability, either enable globally or per ability using Replicate Input Directly.
	 * @param TaskInstanceName Instance name, sometimes used to recycle tasks
	 * @return Task Object
	 */
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UTask_WaitEnhancedInputAction* WaitEnhancedInputAction(
		UGameplayAbility* OwningAbility
		, class UInputMappingContext* InMappingContext
		, class UInputAction* InInputAction
		, FName TaskInstanceName);


	virtual void Activate() override;
	virtual void OnDestroy(bool bInOwnerFinished) override;
private:
	UFUNCTION()
	void HandleOnInputPressed();

	UFUNCTION()
	void HandleOnInputTriggered();

	UFUNCTION()
	void HandleOnInputReleased();
};

