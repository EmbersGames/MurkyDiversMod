// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityInputBindingComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Abilities/GameplayAbility.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayAbilityBase.generated.h"

class UGameplayAbilityBase;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameplayAbilityBaseEnded, UGameplayAbilityBase*, Ability);

/**
 * 
 */
UCLASS()
class GAS_UTILITY_API UGameplayAbilityBase : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGameplayAbilityBase();
	UGameplayAbilityBase(const FObjectInitializer& ObjectInitializer);
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

	UFUNCTION(BlueprintNativeEvent, Category = "Ability|Input")
	void InputReleased_BP(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo);

	virtual AActor* GetGameplayTaskAvatar(const UGameplayTask* Task) const override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ability")
	bool IsActive_BP();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ability|Get")
	ACharacter* GetCharacterOwner() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ability|Get")
	FVector GetCharacterLocation() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ability|Get")
	FRotator GetCharacterRotation() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ability|Get")
	AController* GetControllerOwner();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ability|Get")
	APlayerController* GetPlayerControllerOwner();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ability|Get")
	UCharacterMovementComponent* GetOwnerCharacterMovementComponent();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ability|Get")
	UAbilityInputBindingComponent* GetAbilityInputBindingComponent();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ability|Get")
	USkeletalMeshComponent* GetOwnerMesh();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ability|Get", DisplayName="GetGAS_Comp")
	UAbilitySystemComponentBase* GetOwnerAbilitySystemComponent();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ability|Gameplay Tags")
	bool HasMatchingGameplayTag(FGameplayTag GameplayTag);

	UFUNCTION(BlueprintCallable, Category = "Ability|Gameplay Tags")
	void AddGameplayTag(FGameplayTag GameplayTag, int32 Count = 1);

	UFUNCTION(BlueprintCallable, Category = "Ability|Gameplay Tags")
	void AddGameplayTags(const FGameplayTagContainer& GameplayTags, int32 Count = 1);

	UFUNCTION(BlueprintCallable, Category = "Ability|Gameplay Tags")
	void RemoveGameplayTag(FGameplayTag GameplayTag, int32 Count = 1);

	UFUNCTION(BlueprintCallable, Category = "Ability|Gameplay Tags")
	void RemoveGameplayTags(const FGameplayTagContainer& GameplayTags, int32 Count = 1);

	UFUNCTION(BlueprintCallable, Category = "Ability|Gameplay Tags")
	void SetGameplayTagCount(FGameplayTag GameplayTag, int32 Count = 1);

	UFUNCTION(BlueprintCallable, Category = "Ability|Gameplay Tags")
	int32 GetGameplayTagCount(FGameplayTag GameplayTag);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Ability", DisplayName="CanActivateAbility")
	virtual bool CanActivateAbility_BP();

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void PreActivate(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	virtual void CancelTagAddedToActor(const FGameplayTag Tag, int32 NewCount);


	UFUNCTION(BlueprintNativeEvent, Category="TagsCategory|CancelTag")
	bool ConfirmCancelAbilityWithTag(const FGameplayTag Tag) const;

	UPROPERTY(BlueprintReadWrite, Category="InputAction")
	FInputActionInstance LastInputDataInstanceSnapshotAtActivate;

	UPROPERTY(BlueprintReadWrite, Category="InputAction")
	FInputActionInstance LastInputDataInstanceTemp;

	// Lock others Abilities that reference this Tag, Usually it should be Action.Doing.LockAll
	UPROPERTY(EditDefaultsOnly, Category = Tags, meta=(Categories="TagsCategory"))
	FGameplayTagContainer TagsToLockAll;

	// Lock others Abilities that reference this Tag, Usually it should be Action.Doing.LockAll
	UPROPERTY(EditDefaultsOnly, Category = Tags, meta=(Categories="TagsCategory"))
	FGameplayTagContainer TagsToCancelThisAbility;

	TMap<FGameplayTag, FDelegateHandle> TagsToCancelThisAbilityDelegateHandles;

	bool TagsToLockAllAdded;

	UFUNCTION(BlueprintCallable, Category="TagsCategory")
	bool RemoveTagsToLockAll();

	UFUNCTION(BlueprintPure, BlueprintCallable, Category="Ability")
	FGameplayAbilitySpecHandle GetSpecHandle();

	UPROPERTY(BlueprintAssignable, Category = "Ability")
	FOnGameplayAbilityBaseEnded OnGameplayAbilityEnded;
	// Return first tag in AbilityTags
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Ability|Tag")
	FGameplayTag GetAbilityTag() const;

	UPROPERTY(BlueprintReadWrite, Category="Ability|BehaviourTask")
	bool bForceExecuteAbilityBehaviourTask;
};
