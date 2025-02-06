// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/GameplayAbilityBase.h"

#include "GameFramework/PlayerController.h"
#include "AbilitySystemComponentBase.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerState.h"

UGameplayAbilityBase::UGameplayAbilityBase() : Super()
{
	TagsToLockAllAdded = false;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

UGameplayAbilityBase::UGameplayAbilityBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	TagsToLockAllAdded = false;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGameplayAbilityBase::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
	if (ActorInfo)
		InputReleased_BP(Handle, *ActorInfo, ActivationInfo);
}

AActor* UGameplayAbilityBase::GetGameplayTaskAvatar(const UGameplayTask* Task) const
{
	return GetCharacterOwner();
}

bool UGameplayAbilityBase::IsActive_BP()
{
	return IsActive();
}

ACharacter* UGameplayAbilityBase::GetCharacterOwner() const
{
	if (!CurrentActorInfo) return nullptr;
	const auto Owner = GetOwningActorFromActorInfo();
	if (!Owner) return nullptr;
	if (const auto Character = Cast<ACharacter>(Owner))
		return Character;
	if (const auto PlayerState = Cast<APlayerState>(Owner))
	{
		if (IsValid(PlayerState->GetPawn()))
		{
			return Cast<ACharacter>(PlayerState->GetPawn());
		}
	}
	return nullptr;
}

FVector UGameplayAbilityBase::GetCharacterLocation() const
{
	if (auto CharOwner = GetCharacterOwner())
		return CharOwner->GetActorLocation();
	return FVector::ZeroVector;
}

FRotator UGameplayAbilityBase::GetCharacterRotation() const
{
	if (auto CharOwner = GetCharacterOwner())
		return CharOwner->GetActorRotation();
	return FRotator::ZeroRotator;
}

AController* UGameplayAbilityBase::GetControllerOwner()
{
	const auto Owner = GetOwningActorFromActorInfo();
	if (const auto Controller = Cast<AController>(Owner))
		return Controller;
	if (const auto Character = Cast<ACharacter>(Owner))
		return Character->Controller;
	if (const auto PlayerState = Cast<APlayerState>(Owner))
	{
		if (IsValid(PlayerState->GetOwner()))
		{
			return Cast<AController>(PlayerState->GetOwner());
		}
	}
	return nullptr;
}

APlayerController* UGameplayAbilityBase::GetPlayerControllerOwner()
{
	return Cast<APlayerController>(GetControllerOwner());
}

UCharacterMovementComponent* UGameplayAbilityBase::GetOwnerCharacterMovementComponent()
{
	if (const auto Character = GetCharacterOwner())
	{
		return Character->GetCharacterMovement();
	}
	return nullptr;
}

UAbilityInputBindingComponent* UGameplayAbilityBase::GetAbilityInputBindingComponent()
{
	if (const auto Character = GetCharacterOwner())
	{
		if (const auto AbilityInputBindingComp = Character->GetComponentByClass(UAbilityInputBindingComponent::StaticClass()))
		{
			return Cast<UAbilityInputBindingComponent>(AbilityInputBindingComp);
		}
	}
	return nullptr;
}

USkeletalMeshComponent* UGameplayAbilityBase::GetOwnerMesh()
{
	if (const auto Character = GetCharacterOwner())
	{
		return Character->GetMesh();
	}
	return nullptr;
}


UAbilitySystemComponentBase* UGameplayAbilityBase::GetOwnerAbilitySystemComponent()
{
	const auto Owner = GetOwningActorFromActorInfo();
	if (const auto OwnerInterface = Cast<IAbilitySystemInterface>(Owner))
		return Cast<UAbilitySystemComponentBase>(OwnerInterface->GetAbilitySystemComponent());
	const auto OwnerCharacter = GetCharacterOwner();
	if (const auto OwnerInterface = Cast<IAbilitySystemInterface>(OwnerCharacter))
		return Cast<UAbilitySystemComponentBase>(OwnerInterface->GetAbilitySystemComponent());
	if (const auto Component = Owner->GetComponentByClass(UAbilitySystemComponentBase::StaticClass()))
		if (const auto AbilitySystemComponent = Cast<UAbilitySystemComponentBase>(Component))
			return AbilitySystemComponent;
	return nullptr;
}

bool UGameplayAbilityBase::HasMatchingGameplayTag(FGameplayTag GameplayTag)
{
	return GetOwnerAbilitySystemComponent()->HasMatchingGameplayTag(GameplayTag);
}

void UGameplayAbilityBase::AddGameplayTag(FGameplayTag GameplayTag, int32 Count)
{
	GetOwnerAbilitySystemComponent()->AddGameplayTag(GameplayTag, Count);
}

void UGameplayAbilityBase::AddGameplayTags(const FGameplayTagContainer& GameplayTags, int32 Count)
{
	GetOwnerAbilitySystemComponent()->AddGameplayTags(GameplayTags, Count);
}

void UGameplayAbilityBase::RemoveGameplayTag(FGameplayTag GameplayTag, int32 Count)
{
	GetOwnerAbilitySystemComponent()->RemoveGameplayTag(GameplayTag, Count);
}

void UGameplayAbilityBase::RemoveGameplayTags(const FGameplayTagContainer& GameplayTags, int32 Count)
{
	GetOwnerAbilitySystemComponent()->RemoveGameplayTags(GameplayTags, Count);
}

void UGameplayAbilityBase::SetGameplayTagCount(FGameplayTag GameplayTag, int32 Count)
{
	GetOwnerAbilitySystemComponent()->SetGameplayTagCount(GameplayTag, Count);
}

int32 UGameplayAbilityBase::GetGameplayTagCount(FGameplayTag GameplayTag)
{
	return GetOwnerAbilitySystemComponent()->GetGameplayTagCount(GameplayTag);
}

bool UGameplayAbilityBase::CanActivateAbility_BP()
{
	return CanActivateAbility(CurrentSpecHandle, CurrentActorInfo);
}

bool UGameplayAbilityBase::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (ActorInfo->AbilitySystemComponent.IsValid())
	{
		UAbilitySystemComponentBase* GasComp = Cast<UAbilitySystemComponentBase>(ActorInfo->AbilitySystemComponent);
		if (GasComp)
		{
			if (GasComp->ForceExecuteAbilityBehaviourTaskTag.IsValid() && GasComp->ForceExecuteAbilityBehaviourTaskTag == AbilityTags.First() && !IsActive())
				return true;
		}

		if (ActorInfo->AbilitySystemComponent->HasAnyMatchingGameplayTags(TagsToCancelThisAbility))
		{
			for (auto Tag : TagsToCancelThisAbility)
			{
				if (ActorInfo->AbilitySystemComponent->HasMatchingGameplayTag(Tag))
				{
					if (ConfirmCancelAbilityWithTag(Tag))
						return false;
				}
			}
		}
	}

	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UGameplayAbilityBase::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	LastInputDataInstanceSnapshotAtActivate = LastInputDataInstanceTemp;
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UGameplayAbilityBase::PreActivate(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate, const FGameplayEventData* TriggerEventData)
{
	UAbilitySystemComponent* Comp = ActorInfo->AbilitySystemComponent.Get();
	if (ActorInfo->AbilitySystemComponent.IsValid())
	{
		Comp->AddLooseGameplayTags(TagsToLockAll);
		TagsToLockAllAdded = true;
		for (auto Tag : TagsToCancelThisAbility)
		{
			TagsToCancelThisAbilityDelegateHandles.Add(Tag, Comp->RegisterGameplayTagEvent(Tag).AddUObject(this, &UGameplayAbilityBase::CancelTagAddedToActor));
		}
	}
	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);
}

void UGameplayAbilityBase::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (IsEndAbilityValid(Handle, ActorInfo))
	{
		RemoveTagsToLockAll();

		UAbilitySystemComponent* Comp = ActorInfo->AbilitySystemComponent.Get();
		if (ActorInfo->AbilitySystemComponent.IsValid())
		{
			for (auto KeyValue : TagsToCancelThisAbilityDelegateHandles)
			{
				Comp->RegisterGameplayTagEvent(KeyValue.Key).Remove(KeyValue.Value);
			}
			TagsToCancelThisAbilityDelegateHandles.Empty();
		}
	}
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	OnGameplayAbilityEnded.Broadcast(this);
}

void UGameplayAbilityBase::CancelTagAddedToActor(const FGameplayTag Tag, int32 NewCount)
{
	if (ConfirmCancelAbilityWithTag(Tag))
	{
		if (CurrentActorInfo)
			CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
	}
}

bool UGameplayAbilityBase::ConfirmCancelAbilityWithTag_Implementation(const FGameplayTag Tag) const
{
	return true;
}


bool UGameplayAbilityBase::RemoveTagsToLockAll()
{
	if (TagsToLockAllAdded)
	{
		GetCurrentActorInfo()->AbilitySystemComponent->RemoveLooseGameplayTags(TagsToLockAll);
		TagsToLockAllAdded = false;
		return true;
	}
	return false;
}

FGameplayAbilitySpecHandle UGameplayAbilityBase::GetSpecHandle()
{
	return GetCurrentAbilitySpecHandle();
}

FGameplayTag UGameplayAbilityBase::GetAbilityTag() const
{
	return AbilityTags.First();
}


void UGameplayAbilityBase::InputReleased_BP_Implementation(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
}
