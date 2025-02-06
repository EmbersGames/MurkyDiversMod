// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AnimNotifyState_MotionWarping.h"
#include "Components/MotionWarpingComponentBase.h"
#include "AbilityTask_WaitMotionWarping.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMotionWarpingEventDelegate,UAnimNotifyState_MotionWarping*,NotifyMotionWarping,URootMotionModifier*, Modifier);
/**
 * 
 */
UCLASS()
class GAS_UTILITY_API UAbilityTask_WaitMotionWarping : public UAbilityTask
{
	GENERATED_BODY()
	
public:
	// Constructor and overrides
	UAbilityTask_WaitMotionWarping(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintAssignable)
	FMotionWarpingEventDelegate OnWarpStart;
	
	UPROPERTY(BlueprintAssignable)
	FMotionWarpingEventDelegate OnWarpUpdate;
	
	UPROPERTY(BlueprintAssignable)
	FMotionWarpingEventDelegate OnWarpEnd;

	UFUNCTION()
	void OnWarpEnd_Callback(UAnimNotifyState_MotionWarping* NotifyMotionWarping,URootMotionModifier* Modifier);
	UFUNCTION()
	void OnWarpStart_Callback(UAnimNotifyState_MotionWarping* NotifyMotionWarping,URootMotionModifier* Modifier);
	UFUNCTION()
	void OnWarpUpdate_Callback(UAnimNotifyState_MotionWarping* NotifyMotionWarping,URootMotionModifier* Modifier);
	
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
		static UAbilityTask_WaitMotionWarping* WaitForMotionWarpingEvent(
				UGameplayAbility* OwningAbility,
				FName TaskInstanceName);
	
	
	virtual void Activate() override;
	virtual void ExternalCancel() override;
	virtual FString GetDebugString() const override;
	virtual void OnDestroy(bool AbilityEnded) override;
};
