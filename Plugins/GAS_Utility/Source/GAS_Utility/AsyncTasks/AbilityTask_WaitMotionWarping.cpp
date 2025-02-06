// Fill out your copyright notice in the Description page of Project Settings.


#include "AsyncTasks/AbilityTask_WaitMotionWarping.h"

#include "Components/MotionWarpingComponentBase.h"

UAbilityTask_WaitMotionWarping::UAbilityTask_WaitMotionWarping(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
}

void UAbilityTask_WaitMotionWarping::OnWarpStart_Callback(UAnimNotifyState_MotionWarping* NotifyMotionWarping,URootMotionModifier* Modifier)
{
	OnWarpStart.Broadcast(NotifyMotionWarping,Modifier);
}

void UAbilityTask_WaitMotionWarping::OnWarpUpdate_Callback(UAnimNotifyState_MotionWarping* NotifyMotionWarping,URootMotionModifier* Modifier)
{
	OnWarpUpdate.Broadcast(NotifyMotionWarping,Modifier);
}

void UAbilityTask_WaitMotionWarping::OnWarpEnd_Callback(UAnimNotifyState_MotionWarping* NotifyMotionWarping,URootMotionModifier* Modifier)
{
	OnWarpEnd.Broadcast(NotifyMotionWarping,Modifier);
}


UAbilityTask_WaitMotionWarping* UAbilityTask_WaitMotionWarping::WaitForMotionWarpingEvent(UGameplayAbility* OwningAbility, FName TaskInstanceName)
{
	UAbilityTask_WaitMotionWarping* MyObj = NewAbilityTask<UAbilityTask_WaitMotionWarping>(OwningAbility, TaskInstanceName);
	return MyObj;
}


void UAbilityTask_WaitMotionWarping::Activate()
{
	Super::Activate();

	if (Ability == nullptr)
	{
		return;
	}
	const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
	if (!ActorInfo || !ActorInfo->AvatarActor.IsValid()) return;
	auto ActorMotionWarpingComp = ActorInfo->AvatarActor->GetComponentByClass(UMotionWarpingComponentBase::StaticClass());
	if(ActorMotionWarpingComp)
	{
		auto MotionWarpingComp = Cast<UMotionWarpingComponentBase>(ActorMotionWarpingComp);
		MotionWarpingComp->OnWarpStart.AddDynamic(this,&UAbilityTask_WaitMotionWarping::OnWarpStart_Callback);
		MotionWarpingComp->OnWarUpdate.AddDynamic(this,&UAbilityTask_WaitMotionWarping::OnWarpUpdate_Callback);
		MotionWarpingComp->OnWarpEnd.AddDynamic(this,&UAbilityTask_WaitMotionWarping::OnWarpEnd_Callback);
	}
}

void UAbilityTask_WaitMotionWarping::ExternalCancel()
{
	Super::ExternalCancel();
}

FString UAbilityTask_WaitMotionWarping::GetDebugString() const
{
	return Super::GetDebugString();
}

void UAbilityTask_WaitMotionWarping::OnDestroy(bool AbilityEnded)
{
	const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
	if (!ActorInfo || !ActorInfo->AvatarActor.IsValid()) return;
	auto ActorMotionWarpingComp = ActorInfo->AvatarActor->GetComponentByClass(UMotionWarpingComponentBase::StaticClass());
	if(ActorMotionWarpingComp)
	{
		auto MotionWarpingComp = Cast<UMotionWarpingComponentBase>(ActorMotionWarpingComp);
		MotionWarpingComp->OnWarpStart.RemoveDynamic(this,&UAbilityTask_WaitMotionWarping::OnWarpStart_Callback);
		MotionWarpingComp->OnWarUpdate.RemoveDynamic(this,&UAbilityTask_WaitMotionWarping::OnWarpUpdate_Callback);
		MotionWarpingComp->OnWarpEnd.RemoveDynamic(this,&UAbilityTask_WaitMotionWarping::OnWarpEnd_Callback);
	}

	Super::OnDestroy(AbilityEnded);
}
