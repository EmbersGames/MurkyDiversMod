// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GameplayChangerBase.h"
#include "AttributeSet.h"
#include "GAS_Utility/VariableContainer/VariableContainer.h"
#include "AbilitySystemComponentBase.generated.h"


USTRUCT(BlueprintType)
struct GAS_UTILITY_API FObjectArray
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, Category="GAS_Utility")
	TArray<TObjectPtr<UObject>> ArrayList;
};

USTRUCT(BlueprintType)
struct GAS_UTILITY_API FTagPriorityArray
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="GAS_Utility")
	TArray<FGameplayTag> TagList;
};

USTRUCT(BlueprintType)
struct GAS_UTILITY_API FTagPriorityArrayContainer
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="GAS_Utility")
	TMap<float, FTagPriorityArray> OverridePriorityFListMap;
};

USTRUCT(BlueprintType)
struct GAS_UTILITY_API FAbilitySystemComponentTagAccessor
{
	GENERATED_BODY()
	UPROPERTY()
	TSoftObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
};

USTRUCT(BlueprintType)
struct FDatatableDamageRow : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
	FDatatableDamageRow();
	/* Row Name Should Be the Tag not FilterTags */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=DatatableDamageRow)
	FGameplayTagContainer FilterTags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=DatatableDamageRow)
	float DamageValue = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=DatatableDamageRow)
	FGameplayTagContainer PayloadTags;
};

USTRUCT(BlueprintType)
struct FDebug_GAS_Utility_GroupBase
{
	GENERATED_BODY()

public:
	FDebug_GAS_Utility_GroupBase()
	{
		bIsCollapsedByDefault = true;
		GroupName = "Group Name";
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GAS|Debug")
	FName GroupName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GAS|Debug")
	bool bIsCollapsedByDefault;
};

USTRUCT(BlueprintType)
struct FDebug_GAS_Utility_TagGroup : public FDebug_GAS_Utility_GroupBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GAS|Debug")
	FGameplayTagContainer MatchTags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GAS|Debug")
	FGameplayTagContainer MatchExactlyTags;
};

USTRUCT(BlueprintType)
struct FDebug_GAS_Utility_AttributeGroup : public FDebug_GAS_Utility_GroupBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GAS|Debug")
	TArray<FGameplayAttribute> Attributes;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStoredObjectChange, const FGameplayTag, Tag);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNewAbilitySystemComponentDelegateCpp, const UAbilitySystemComponentBase*, AbilitySystemComp);

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnStoredObjectChangeBP, const FGameplayTag, Tag);

DECLARE_DYNAMIC_DELEGATE_OneParam(FNewAbilitySystemComponentDelegate, const UAbilitySystemComponentBase*, AbilitySystemComp);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilitySystemComponentDestoyDelegate, const UAbilitySystemComponentBase*, AbilitySystemComp);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FNewTagUpdateDelegate, const FGameplayTag, GameplayTag, bool, TagExist);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityAddedOrRemoveDelegate, const UGameplayAbility*, GameplayAbility);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTagChange, const FGameplayTag, Tag);

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, meta=(BlueprintSpawnableComponent))
class GAS_UTILITY_API UAbilitySystemComponentBase : public UAbilitySystemComponent
{
	GENERATED_BODY()

public :
	UAbilitySystemComponentBase(const FObjectInitializer& ObjectInitializer);
	virtual ~UAbilitySystemComponentBase() override;
	virtual void BeginPlay() override;
	virtual void DestroyComponent(bool bPromoteChildren) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual bool GetShouldTick() const override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Deactivate() override;
	virtual void BeginDestroy() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void ClearTimerHandlers();

	FCriticalSection StoreObjectsMutex;
	/* GAMEPLAY TAGS ------------------------------------------------------------- */

	void OnGameplayTagUpdate_ServerReplicate(const FGameplayTag GameplayTag, bool TagExist);

	UFUNCTION(BlueprintCallable, Category = "Gameplay Tags", BlueprintAuthorityOnly)
	void SetReplicatedGameplayTag(FGameplayTag GameplayTag, int32 Count = 1);

	UFUNCTION(BlueprintCallable, Category = "Gameplay Tags", BlueprintAuthorityOnly)
	void AddReplicatedGameplayTag(FGameplayTag GameplayTag);

	UFUNCTION(BlueprintCallable, Category = "Gameplay Tags", BlueprintAuthorityOnly)
	void AddReplicatedGameplayTags(FGameplayTagContainer GameplayTags);

	UFUNCTION(BlueprintCallable, Category = "Gameplay Tags", BlueprintAuthorityOnly)
	void RemoveReplicatedGameplayTags(FGameplayTagContainer GameplayTags);

	UFUNCTION(BlueprintCallable, Category = "Gameplay Tags", BlueprintAuthorityOnly)
	void RemoveReplicatedGameplayTag(FGameplayTag GameplayTag);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Gameplay Tags")
	bool HasGameplayTag(FGameplayTag GameplayTag);

	UFUNCTION(BlueprintCallable, Category = "Gameplay Tags")
	void AddGameplayTag(FGameplayTag GameplayTag, int32 Count = 1);

	UFUNCTION(BlueprintCallable, Category = "Gameplay Tags")
	void AddGameplayTags(const FGameplayTagContainer& GameplayTags, int32 Count = 1);

	UFUNCTION(BlueprintCallable, Category = "Gameplay Tags")
	void RemoveGameplayTag(FGameplayTag GameplayTag, int32 Count = 1);

	UFUNCTION(BlueprintCallable, Category = "Gameplay Tags")
	void RemoveGameplayTags(const FGameplayTagContainer& GameplayTags, int32 Count = 1);

	UFUNCTION(BlueprintCallable, Category = "Gameplay Tags")
	void SetGameplayTagCount(FGameplayTag GameplayTag, int32 Count = 1);


	/* GAMEPLAY DAMAGE  ------------------------------------------------------------- */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Gameplay Damage")
	float GetDamageForAbilityAndTag(FGameplayTag AbilityTag, FGameplayTag DamageTag, FGameplayTag DamageQuantityTag, float DefaultDamage, FGameplayTagContainer& PayloadTags);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Gameplay Damage")
	bool GetDamageDataForAbilityAndTag(FGameplayTag AbilityTag, FGameplayTag DamageTag, FGameplayTag DamageQuantityTag, FDatatableDamageRow& ResultData, float DefaultDamage);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category= "Gameplay Damage")
	UDataTable* DamageDataTable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category= "Gameplay Damage")
	FGameplayAttribute DamageSendFactorAttribute;

	/* GAMEPLAY CHANGERS  ------------------------------------------------------------- */

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="GAS_Utility")
	TArray<TObjectPtr<UGameplayChangerBase>> GameplayChangers;

	UPROPERTY(BlueprintReadOnly, Category="GAS_Utility")
	TArray<FGameplayAttribute> DirtyAttributes;

	UPROPERTY(BlueprintReadOnly, Category="GAS_Utility")
	TArray<FGameplayAttribute> AttributesAlreadyInitialized;

	UFUNCTION(BlueprintCallable, Category="GameplayChanger", meta = (DeterminesOutputType = GameplayChangerClass))
	UGameplayChangerBase* AddGameplayChangerByClass(TSubclassOf<UGameplayChangerBase> GameplayChangerClass);

	UFUNCTION(BlueprintCallable, Category="GameplayChanger")
	UGameplayChangerBase* GetGameplayChangerByClass(TSubclassOf<UGameplayChangerBase> GameplayChangerClass);


	UFUNCTION(BlueprintCallable, Category="GameplayChanger")
	UGameplayChangerBase* GetGameplayChangerByTag(FGameplayTag GameplayChangerTag);

	UFUNCTION(BlueprintCallable, Category="GameplayChanger")
	bool AddGameplayChanger(UGameplayChangerBase* GameplayChanger);

	UFUNCTION(BlueprintCallable, Category="GameplayChanger")
	bool RemoveGameplayChanger(UGameplayChangerBase* GameplayChanger);

	UFUNCTION(BlueprintCallable, Category="GameplayChanger")
	bool RemoveGameplayChangerArray(TArray<UGameplayChangerBase*> GameplayChangerArray);

	UFUNCTION(BlueprintCallable, Category="GameplayChanger")
	bool RemoveGameplayChangerByTag(FGameplayTag GameplayChangerTag);

	UFUNCTION(BlueprintCallable, Category="GameplayChanger")
	bool RemoveGameplayChangerByMatchingTag(FGameplayTag GameplayChangerTag);

	UFUNCTION(BlueprintCallable, Category="Attributes")
	void RefreshAllDirtyAttributes();

	UFUNCTION(BlueprintCallable, Category="Attributes")
	void RefreshAttribute(FGameplayAttribute GameplayAttribute);

	UFUNCTION(BlueprintCallable, Category="Attribute")
	void AddAttributeSet(UAttributeSet* Attributes);

	void SetReplicatedAttributeValue(FGameplayAttribute Attribute, float NewValue, float OldValue);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="GAS_Utility")
	TMap<FGameplayAttribute, FTagPriorityArrayContainer> GameplayChangerOverridePriority;

	UFUNCTION(BlueprintCallable, Category=Attribute)
	UAttributeChanger* CreateLooseAttributeChanger(FGameplayAttribute Attribute, FGameplayTag Tag);

	UFUNCTION(BlueprintCallable, Category=Attribute)
	UAttributeChanger* CreateLooseAttributeChangerWithData(FGameplayAttribute Attribute, FGameplayTag Tag, TEnumAsByte<EAttributeChanger::Type> Operation = EAttributeChanger::SetValue, float Value = 0.0f, float Priority = 0.0f);

	UFUNCTION(BlueprintCallable, Category=Attribute)
	bool RemoveLooseAttributeChanger(UAttributeChanger* AttributeChanger, bool bRemoveGameplayChangerParent = true);

	/* OBJECTS STORED -------------------------------------------------------------*/

	UFUNCTION(BlueprintPure, BlueprintCallable, Category="Stored Objects")
	UObject* GetFirstObjectStoredWithGameplayTag(const FGameplayTag Tag);

	UFUNCTION(BlueprintPure, BlueprintCallable, Category="Stored Objects")
	TArray<UObject*> GetObjectsStoredWithGameplayTag(const FGameplayTag Tag);

	UFUNCTION(BlueprintCallable, Category="Stored Objects")
	void AddObjectsStoredWithGameplayTag(const FGameplayTag Tag, TArray<UObject*> Array);

	UFUNCTION(BlueprintCallable, Category="Stored Objects")
	void AddObjectStoredWithGameplayTag(const FGameplayTag Tag, UObject* Object);

	UFUNCTION(BlueprintCallable, Category="Stored Objects")
	void ClearObjectsStoredWithGameplayTag(const FGameplayTag Tag);

	UFUNCTION(BlueprintCallable, Category="Stored Objects")
	void RemoveObjectsStoredWithGameplayTag(const FGameplayTag Tag, TArray<UObject*> Array);

	UFUNCTION(BlueprintCallable, Category="Stored Objects")
	void RemoveObjectStoredWithGameplayTag(const FGameplayTag Tag, UObject* Object);

	UFUNCTION(BlueprintCallable, Category="Stored Objects")
	void SetObjectStoredWithGameplayTag(const FGameplayTag Tag, UObject* Object);

	UPROPERTY(BlueprintReadWrite, Category="Stored Objects")
	TMap<FGameplayTag, FObjectArray> ObjectsStoredByGameplayTag;

	void TriggerObjectStoredChangeDelegates(const FGameplayTag Tag);

	FOnStoredObjectChange* GetStoredObjectChangeDelegates(FGameplayTag Tag);

	TMap<FGameplayTag, FOnStoredObjectChange> StoredObjectChangeDelegates;

	UFUNCTION(BlueprintCallable, Category="Stored Objects|Delegate")
	void BindObjectStoredChangeDelegate(FOnStoredObjectChangeBP Delegate, FGameplayTag Tag);

	UFUNCTION(BlueprintCallable, Category="Stored Objects|Delegate")
	void UnbindObjectStoredChangeDelegate(FOnStoredObjectChangeBP Delegate, FGameplayTag Tag);

	UFUNCTION(BlueprintCallable, Category="Stored Objects|Sort")
	TArray<AActor*> GetActorStoredWithGameplayTagSortByAngle(FVector Direction, FVector Origin, FGameplayTag Tag, FString SceneComponentTag = "");

	UFUNCTION(BlueprintCallable, Category="Stored Objects|Sort")
	TArray<AActor*> GetActorStoredWithGameplayTagSortByDistance(FVector Location, FGameplayTag Tag, FString SceneComponentTag = "");


	/* UTILITIES -------------------------------------------------------------*/

	UFUNCTION(BlueprintPure, Category = "Ability|GameplayAbility")
	UGameplayAbility* GetGameplayAbilityFromClass(TSubclassOf<UGameplayAbility> InAbilityToActivate);

	UFUNCTION(BlueprintPure, Category = "Ability|GameplayAbility")
	UGameplayAbility* GetGameplayAbilityFromTag(const FGameplayTag AbilityTag);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (BlueprintThreadSafe), Category = "Ability|GameplayAttribute")
	float GetAttribute(FGameplayAttribute Attribute) const;


	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Ability|Accessor")
	FAbilitySystemComponentTagAccessor GetTagAccessor();


	UFUNCTION(BlueprintPure, BlueprintCallable, meta = (BlueprintThreadSafe), Category = "Ability|Accessor|GameplayTag")
	static bool AccessorHasMatchingTag(FAbilitySystemComponentTagAccessor Accessor, FGameplayTag Tag);

	UFUNCTION(BlueprintPure, BlueprintCallable, meta = (BlueprintThreadSafe), Category = "Ability|Accessor|GameplayTag")
	static float GetGameplayAttributeValueByAccessor(FAbilitySystemComponentTagAccessor Accessor, FGameplayAttribute Attribute);

	UFUNCTION(BlueprintPure, BlueprintCallable, meta = (BlueprintThreadSafe), Category = "Ability|Accessor|GameplayTag")
	static float GetGameplayCooldownByAccessor(FAbilitySystemComponentTagAccessor Accessor, FGameplayTag Tag);

	UFUNCTION(BlueprintPure, BlueprintCallable, meta = (BlueprintThreadSafe), Category = "Ability|Accessor|GameplayTag")
	static UObject* GetObjectStoreWithGameplayTagByAccessor(FAbilitySystemComponentTagAccessor Accessor, FGameplayTag Tag, bool& bIsValid);

	UFUNCTION(BlueprintPure, BlueprintCallable, meta = (BlueprintThreadSafe), Category = "Ability|Accessor|GameplayTag")
	static TArray<UObject*> GetObjectsStoreWithGameplayTagByAccessor(FAbilitySystemComponentTagAccessor Accessor, FGameplayTag Tag);

	UFUNCTION(BlueprintPure, BlueprintCallable, Category="GAS_Utiliy")
	AController* GetAvatarController();

	UFUNCTION(BlueprintPure, BlueprintCallable, Category="GAS_Utiliy")
	AActor* GetAvatar();

	/* Input -------------------------------------------------------------*/

	UFUNCTION(BlueprintPure, BlueprintCallable, Category="InputAction")
	FInputActionInstance GetInputActionInstance(UInputAction* InputAction);

	UFUNCTION(BlueprintPure, BlueprintCallable, Category="InputActionInstance")
	bool GetInputActionInstanceValueAsBool(UInputAction* InputAction);

	UFUNCTION(BlueprintPure, BlueprintCallable, Category="InputActionInstance")
	float GetInputActionInstanceValueAsFloat(UInputAction* InputAction);

	UFUNCTION(BlueprintPure, BlueprintCallable, Category="InputActionInstance")
	FVector2D GetInputActionInstanceValueAsVector2D(UInputAction* InputAction);

	UFUNCTION(BlueprintPure, BlueprintCallable, Category="InputActionInstance")
	FVector GetInputActionInstanceValueAsVector(UInputAction* InputAction);


	/* OVERRIDE -------------------------------------------------------------*/

	virtual AActor* GetGameplayTaskAvatar(const UGameplayTask* Task) const override;

	UFUNCTION(BlueprintPure, Category = Ability)
	static UAbilitySystemComponentBase* GetAbilitySystemComp(const AActor* Actor);

	virtual UGameplayAbility* CreateNewInstanceOfAbility(FGameplayAbilitySpec& Spec, const UGameplayAbility* Ability) override;

	virtual void InitializeComponent() override;

	virtual void OnTagUpdated(const FGameplayTag& Tag, bool TagExists) override;

	virtual void OnGiveAbility(FGameplayAbilitySpec& AbilitySpec) override;

	virtual void OnRemoveAbility(FGameplayAbilitySpec& AbilitySpec) override;


	/* Cooldown Tag -------------------------------------------------------------*/

	UFUNCTION(BlueprintCallable, Category="CooldownTag")
	void AddCooldownTag(FGameplayTag Tag, float Duration);

	bool IsServer();

	UFUNCTION(BlueprintCallable, Category="CooldownTag")
	void RemoveCooldownTag(FGameplayTag Tag);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="CooldownTag")
	float GetCooldownFromTag(FGameplayTag Tag);

	void RemoveCooldownTagAtEnd(const FGameplayEffectRemovalInfo& Info, FGameplayTag Tag);


	UPROPERTY()
	TMap<FGameplayTag, FActiveGameplayEffectHandle> CooldownHandlesByTag;

	/* BUFFER INPUT -------------------------------------------------------------*/

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BufferInput")
	TArray<FGameplayTag> ActionTagPriorityForBufferInput;

	UFUNCTION(BlueprintCallable, Category="BufferInput")
	bool IsActionTagHigherPriorityForBufferInput(FGameplayTag Tag);

	/* Delegates -------------------------------------------------------------*/
	UPROPERTY(BlueprintAssignable, Category="AbilitySystemComponent")
	FAbilitySystemComponentDestoyDelegate OnDestroyed;

	UPROPERTY(BlueprintAssignable, Category="AbilitySystemComponent")
	FNewTagUpdateDelegate OnTagUpdate;

	UFUNCTION()
	void OnTagUpdateEvent(FGameplayTag Tag, bool TagExist);

	UFUNCTION(BlueprintCallable, Category="Gameplay Tag|Delegate")
	void BindTagChangeDelegate(FOnStoredObjectChangeBP Delegate, FGameplayTag Tag);

	UFUNCTION(BlueprintCallable, Category="Gameplay Tag|Delegate")
	void UnbindTagChangeDelegate(FOnStoredObjectChangeBP Delegate, FGameplayTag Tag);

	FOnStoredObjectChange* GetTagChangeDelegates(FGameplayTag Tag);

	TMap<FGameplayTag, FOnStoredObjectChange> TagChangeDelegates;

	UPROPERTY(BlueprintAssignable, Category="AbilitySystemComponent")
	FAbilityAddedOrRemoveDelegate OnAbilityAdded;

	UPROPERTY(BlueprintAssignable, Category="AbilitySystemComponent")
	FAbilityAddedOrRemoveDelegate OnAbilityRemoved;

	/* Static List of all Ability system Component -------------------------------------------------------------*/

	UFUNCTION(BlueprintCallable, Category="Ability|Delegate")
	static void BindNewAbilitySystemComponentDelegate(FNewAbilitySystemComponentDelegate Delegate);

	UFUNCTION(BlueprintCallable, Category="Ability|Delegate")
	static void UnbindNewAbilitySystemComponentDelegate(FNewAbilitySystemComponentDelegate Delegate);

	UFUNCTION(BlueprintCallable, Category="AbilitySystemComp")
	static TArray<UAbilitySystemComponentBase*> GetArrayAbilitySystemComponents();

	static TArray<TObjectPtr<UAbilitySystemComponentBase>> StaticArrayAbilitySystemComponents;

	static FNewAbilitySystemComponentDelegateCpp NewAbilitySystemComponentDelegate;

	/* Debug -------------------------------------------------------------*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GAS|Debug")
	TMap<FName, FDebug_GAS_Utility_TagGroup> DebugGameplayTagsGroups;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GAS|Debug")
	TMap<FName, FDebug_GAS_Utility_TagGroup> DebugAbilitiesGroups;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GAS|Debug")
	TMap<FName, FDebug_GAS_Utility_AttributeGroup> DebugAttributesGroups;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GAS|Debug")
	TArray<FString> DebugAbilityPrefixToRemove;

	// Force Ability

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="GAS|Ability|Behaviour")
	bool IsThereAbilityBehaviourTaskToForce();

	UPROPERTY(BlueprintReadWrite, BlueprintSetter="SetForceExecuteAbilityBehaviourTaskTag", Category="GAS|Ability|Behaviour")
	FGameplayTag ForceExecuteAbilityBehaviourTaskTag;

	UFUNCTION(BlueprintSetter, Category="GAS|Ability|Behaviour")
	void SetForceExecuteAbilityBehaviourTaskTag(const FGameplayTag NewTag);

	UPROPERTY(BlueprintAssignable, Category="GAS|Ability|Behaviour")
	FOnTagChange OnForceExecuteAbilityBehaviourTaskTagChange;


	/* Play Montage -------------------------------------------------------------*/

	virtual float PlayMontage(UGameplayAbility* AnimatingAbility, FGameplayAbilityActivationInfo ActivationInfo, UAnimMontage* Montage, float InPlayRate, FName StartSectionName, float StartTimeSeconds) override;

	UFUNCTION(BlueprintCallable, Category=Montage)
	void SetStopAllMontageWhenPlayNew(const bool bNewStopAllMontageWhenPlayNew);

	UPROPERTY(BlueprintReadOnly, Category=Montage)
	bool bStopAllMontageWhenPlayNew;

	/* Network  -------------------------------------------------------------*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere,Category=Network)
	bool bAutoReplicatedAllTags = true;

	UPROPERTY()
	TMap<FGameplayTag, int> IgnoreLocalGameplayEventCount;

	FORCEINLINE void SetReplicatedGameplayTagCount_Override(const FGameplayTag& GameplayTag, int32 NewCount)
	{
		if (!(NewCount == 0 && !GetReplicatedLooseTags_Mutable().TagMap.Find(GameplayTag)))
			GetReplicatedLooseTags_Mutable().SetTagCount(GameplayTag, NewCount);
		if (GetOwnerRole() == ROLE_Authority)
		{
			MulticastRefreshGameplayTagUpdateEvent(GameplayTag, NewCount);
		}
	}

	UFUNCTION(Reliable, NetMulticast, WithValidation, Category = "GameplayEvents")
	void MulticastRefreshGameplayTagUpdateEvent(FGameplayTag EventTag, int32 Count);

	UFUNCTION(BlueprintCallable, Category = "GameplayEvents")
	void SendServerOnlyGameplayEvent(FGameplayTag EventTag, FGameplayEventData Payload);

	UFUNCTION(Reliable, Server, Category = "GameplayEvents")
	void RunOnServerOnly_SendGameplayEvent(FGameplayTag EventTag, FGameplayEventData Payload);

	UFUNCTION(Reliable, Server, Category = "GameplayEvents")
	void RunOnServerOnly_SendGameplayEvent_ToActor(AActor* Actor, FGameplayTag EventTag, FGameplayEventData Payload);

	UFUNCTION(BlueprintCallable, Category = "GameplayEvents", DisplayName=SendServerOnlyGameplayEvent_VariableContainer)
	void SendServerOnlyGameplayEvent_VC(FGameplayTag EventTag, UVariableContainer* Payload);

	UFUNCTION(Reliable, Server, Category = "GameplayEvents")
	void RunOnServerOnly_SendGameplayEvent_VC(FGameplayTag EventTag, FGAS_Utility_VariableContainerStruct Payload);

	UFUNCTION(Reliable, Server, Category = "GameplayEvents")
	void RunOnServerOnly_SendGameplayEvent_ToActor_VC(AActor* Actor, FGameplayTag EventTag, FGAS_Utility_VariableContainerStruct Payload);

	UFUNCTION(BlueprintCallable, Category = "GameplayEvents")
	void SendServerGameplayEventMulticast(FGameplayTag EventTag, FGameplayEventData Payload);

	UFUNCTION(Reliable, Server, Category = "GameplayEvents")
	void MulticastOnServer_SendGameplayEvent(FGameplayTag EventTag, FGameplayEventData Payload);

	UFUNCTION(Reliable, Server, Category = "GameplayEvents")
	void MulticastOnServer_SendGameplayEvent_ToActor(AActor* Actor, FGameplayTag EventTag, FGameplayEventData Payload);

	UFUNCTION(Reliable, NetMulticast, Category = "GameplayEvents")
	void MulticastSendGameplayEvent(FGameplayTag EventTag, FGameplayEventData Payload);

	virtual void MulticastSendGameplayEvent_Implementation(FGameplayTag EventTag, FGameplayEventData Payload);

	UFUNCTION(BlueprintCallable, Category = "GameplayEvents")
	void SendServerGameplayEvent_VariableContainer_Multicast(FGameplayTag EventTag, UVariableContainer* Payload);

	UFUNCTION(Reliable, Server, Category = "GameplayEvents")
	void SendServerGameplayEvent_VC_Multicast(FGameplayTag EventTag, FGAS_Utility_VariableContainerStruct Payload);

	virtual void SendServerGameplayEvent_VC_Multicast_Implementation(FGameplayTag EventTag, FGAS_Utility_VariableContainerStruct Payload);

	UFUNCTION(Reliable, Server, Category = "GameplayEvents")
	void SendServerGameplayEvent_VC_Multicast_OnActor(AActor* Actor, FGameplayTag EventTag, FGAS_Utility_VariableContainerStruct Payload);

	UFUNCTION(Reliable, NetMulticast, Category = "GameplayEvents")
	void MulticastSendGameplayEvent_VariableContainer(FGameplayTag EventTag, FGAS_Utility_VariableContainerStruct Payload);

	virtual void MulticastSendGameplayEvent_VariableContainer_Implementation(FGameplayTag EventTag, FGAS_Utility_VariableContainerStruct Payload);

	virtual void HandleGameplayEvent_VariableContainer(FGameplayTag EventTag, FGAS_Utility_VariableContainerStruct Payload);
};
