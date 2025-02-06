// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AbilitySystemComponentBase.h"

#include "GameFramework/PlayerController.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemLog.h"
#include "GameplayAbilityBase.h"
#include "Animation/AnimInstance.h"
#include "UObject/UObjectGlobals.h"
#include "Engine/World.h"
#include "GameplayTagsManager.h"
#include "UnrealEngine.h"
#include "TimerManager.h"

#include "GAS_Utility.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "UObject/UnrealTypePrivate.h"

#include "AbilitySystem/NonReplicatedGameplayEffect.h"

TArray<TObjectPtr<UAbilitySystemComponentBase>> UAbilitySystemComponentBase::StaticArrayAbilitySystemComponents = TArray<TObjectPtr<UAbilitySystemComponentBase>>();
FNewAbilitySystemComponentDelegateCpp UAbilitySystemComponentBase::NewAbilitySystemComponentDelegate = FNewAbilitySystemComponentDelegateCpp();

FDatatableDamageRow::FDatatableDamageRow(): FTableRowBase(), DamageValue(0.f)
{
}

UAbilitySystemComponentBase::UAbilitySystemComponentBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	bAutoActivate = true;
	bAutoReplicatedAllTags = true;
	bStopAllMontageWhenPlayNew = true;
	OnTagUpdate.AddUniqueDynamic(this, &UAbilitySystemComponentBase::OnTagUpdateEvent);
}

UAbilitySystemComponentBase::~UAbilitySystemComponentBase()
{
	if (StaticArrayAbilitySystemComponents.Contains(this))
		StaticArrayAbilitySystemComponents.Remove(this);
}

void UAbilitySystemComponentBase::BeginPlay()
{
	Super::BeginPlay();
	StaticArrayAbilitySystemComponents.Add(this);
	NewAbilitySystemComponentDelegate.Broadcast(this);
	//UAbilitySystemGlobals::Get().ReplicateActivationOwnedTags = false;
}

void UAbilitySystemComponentBase::DestroyComponent(bool bPromoteChildren)
{
	ClearTimerHandlers();
	StaticArrayAbilitySystemComponents.Remove(this);
	OnDestroyed.Broadcast(this);
	Super::DestroyComponent(bPromoteChildren);
}

void UAbilitySystemComponentBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ClearTimerHandlers();
	Super::EndPlay(EndPlayReason);
}

void UAbilitySystemComponentBase::Deactivate()
{
	ClearTimerHandlers();
	Super::Deactivate();
}

void UAbilitySystemComponentBase::BeginDestroy()
{
	ClearTimerHandlers();
	Super::BeginDestroy();
}

void UAbilitySystemComponentBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	/*FProperty* ReplicatedProperty = GetReplicatedProperty(StaticClass(), UAbilitySystemComponentBase::StaticClass(), GET_MEMBER_NAME_CHECKED(UAbilitySystemComponentBase, ActivatableAbilities));
	const FString ReplicatedPropertyName = ReplicatedProperty->GetName();
	NetworkingPrivate::FRepPropertyDescriptor PropDesc(*ReplicatedPropertyName, ReplicatedProperty->RepIndex, ReplicatedProperty->ArrayDim);
	for (int32 i = 0; i < PropDesc.ArrayDim; i++)
	{
		const uint16 RepIndex = PropDesc.RepIndex + i;
		FLifetimeProperty* RegisteredPropertyPtr = OutLifetimeProps.FindByPredicate([&RepIndex](const FLifetimeProperty& Var) { return Var.RepIndex == RepIndex; });
		if (RegisteredPropertyPtr)
		{
			RegisteredPropertyPtr->Condition = COND_None;
		}
	}*/
	//DOREPLIFETIME_CONDITION(UAbilitySystemComponentBase, ActivatableAbilities, COND_None);
}

void UAbilitySystemComponentBase::ClearTimerHandlers()
{
	if (!GetOuter()) return;
	auto WorldRef = GetWorld();
	if (!WorldRef)
		WorldRef = GEngine->GetWorld();
	if (!WorldRef)
		return;
	FTimerManager& TimerManager = WorldRef->GetTimerManager();

	for (auto Element : CooldownHandlesByTag)
	{
		FActiveGameplayEffect* GameplayEffect = ActiveGameplayEffects.GetActiveGameplayEffect(Element.Value);
		if (GameplayEffect)
		{
			if (GameplayEffect->PeriodHandle.IsValid())
			{
				TimerManager.ClearTimer(GameplayEffect->PeriodHandle);
				GameplayEffect->PeriodHandle.Invalidate();
			}

			if (GameplayEffect->DurationHandle.IsValid())
			{
				TimerManager.ClearTimer(GameplayEffect->DurationHandle);
				GameplayEffect->DurationHandle.Invalidate();
			}
		}
		Element.Value.RemoveFromGlobalMap();
		Element.Value.Invalidate();
		ActiveGameplayEffects.RemoveActiveGameplayEffect(Element.Value, 1);
	}
	CooldownHandlesByTag.Empty();
}

void UAbilitySystemComponentBase::OnGameplayTagUpdate_ServerReplicate(const FGameplayTag GameplayTag, bool TagExist)
{
	if (GetOwnerRole() == ROLE_Authority && bAutoReplicatedAllTags)
		SetReplicatedGameplayTagCount_Override(GameplayTag, GetGameplayTagCount(GameplayTag));
}

void UAbilitySystemComponentBase::SetReplicatedGameplayTag(FGameplayTag GameplayTag, int32 Count)
{
	if (GetNetMode() == NM_DedicatedServer || GetNetMode() == NM_ListenServer)
	{
		SetGameplayTagCount(GameplayTag, Count);
		SetReplicatedGameplayTagCount_Override(GameplayTag, Count);
	}
}

void UAbilitySystemComponentBase::AddReplicatedGameplayTag(FGameplayTag GameplayTag)
{
	if (GetNetMode() == NM_DedicatedServer || GetNetMode() == NM_ListenServer)
	{
		AddGameplayTag(GameplayTag);
		SetReplicatedGameplayTagCount_Override(GameplayTag, GetTagCount(GameplayTag));
	}
}

void UAbilitySystemComponentBase::AddReplicatedGameplayTags(FGameplayTagContainer GameplayTags)
{
	if (GetNetMode() == NM_DedicatedServer || GetNetMode() == NM_ListenServer)
	{
		AddGameplayTags(GameplayTags);
		AddReplicatedLooseGameplayTags(GameplayTags);
	}
}

void UAbilitySystemComponentBase::RemoveReplicatedGameplayTags(FGameplayTagContainer GameplayTags)
{
	if (GetNetMode() == NM_DedicatedServer || GetNetMode() == NM_ListenServer)
	{
		RemoveGameplayTags(GameplayTags);
		RemoveReplicatedLooseGameplayTags(GameplayTags);
	}
}

void UAbilitySystemComponentBase::RemoveReplicatedGameplayTag(FGameplayTag GameplayTag)
{
	if (GetNetMode() == NM_DedicatedServer || GetNetMode() == NM_ListenServer)
	{
		RemoveGameplayTag(GameplayTag);
		SetReplicatedGameplayTagCount_Override(GameplayTag, GetTagCount(GameplayTag));
	}
}

void UAbilitySystemComponentBase::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	auto GameplayChangersClone = GameplayChangers;
	for (auto GameplayChanger : GameplayChangersClone)
	{
		if (IsValid(GameplayChanger))
		{
			if (GameplayChanger->bTickEnable)
			{
				GameplayChanger->TickGameplayChanger(DeltaTime);
			}
		}
	}
}

bool UAbilitySystemComponentBase::GetShouldTick() const
{
	return true;
}

bool UAbilitySystemComponentBase::HasGameplayTag(FGameplayTag GameplayTag)
{
	return HasMatchingGameplayTag(GameplayTag);
}

void UAbilitySystemComponentBase::AddGameplayTag(FGameplayTag GameplayTag, int32 Count)
{
	AddLooseGameplayTag(GameplayTag, Count);
	OnTagUpdate.Broadcast(GameplayTag, HasGameplayTag(GameplayTag));
}

void UAbilitySystemComponentBase::AddGameplayTags(const FGameplayTagContainer& GameplayTags, int32 Count)
{
	AddLooseGameplayTags(GameplayTags, Count);
	for (const FGameplayTag GameplayTag : GameplayTags)
	{
		OnTagUpdate.Broadcast(GameplayTag, HasGameplayTag(GameplayTag));
	}
}

void UAbilitySystemComponentBase::RemoveGameplayTag(FGameplayTag GameplayTag, int32 Count)
{
	RemoveLooseGameplayTag(GameplayTag, Count);
	OnTagUpdate.Broadcast(GameplayTag, HasGameplayTag(GameplayTag));
}

void UAbilitySystemComponentBase::RemoveGameplayTags(const FGameplayTagContainer& GameplayTags, int32 Count)
{
	RemoveLooseGameplayTags(GameplayTags, Count);
	for (const FGameplayTag GameplayTag : GameplayTags)
	{
		OnTagUpdate.Broadcast(GameplayTag, HasGameplayTag(GameplayTag));
	}
}

void UAbilitySystemComponentBase::SetGameplayTagCount(FGameplayTag GameplayTag, int32 Count)
{
	SetLooseGameplayTagCount(GameplayTag, Count);
	OnTagUpdate.Broadcast(GameplayTag, Count != 0);
}

float UAbilitySystemComponentBase::GetDamageForAbilityAndTag(FGameplayTag AbilityTag, FGameplayTag DamageTag, FGameplayTag DamageQuantityTag, float DefaultDamage, FGameplayTagContainer& PayloadTags)
{
	float Damage = DefaultDamage;
	FDatatableDamageRow ResultData;
	if (GetDamageDataForAbilityAndTag(AbilityTag, DamageTag, DamageQuantityTag, ResultData, DefaultDamage))
	{
		Damage = ResultData.DamageValue;
		PayloadTags = ResultData.PayloadTags;
	}
	return Damage;
}

bool UAbilitySystemComponentBase::GetDamageDataForAbilityAndTag(FGameplayTag AbilityTag, FGameplayTag DamageTag, FGameplayTag DamageQuantityTag, FDatatableDamageRow& ResultData, float DefaultDamage)
{
	FDatatableDamageRow* Result = nullptr;
	FDatatableDamageRow* MyRowData = nullptr;
	if (IsValid(DamageDataTable))
	{
		if (DamageQuantityTag.IsValid())
		{
			if (DamageDataTable->GetRowMap().Contains(DamageQuantityTag.GetTagName()))
			{
				MyRowData = DamageDataTable->FindRow<FDatatableDamageRow>(DamageQuantityTag.GetTagName(), "");
				if (MyRowData)
				{
					Result = MyRowData;
				}
			}
		}
		if (AbilityTag.IsValid())
		{
			if (DamageDataTable->GetRowMap().Contains(AbilityTag.GetTagName()))
			{
				MyRowData = DamageDataTable->FindRow<FDatatableDamageRow>(AbilityTag.GetTagName(), "");
				if (MyRowData)
				{
					if (MyRowData->FilterTags.IsEmpty() || MyRowData->FilterTags.HasTag(DamageTag) || MyRowData->FilterTags.HasTag(DamageQuantityTag))
						Result = MyRowData;
				}
			}
		}
		if (DamageTag.IsValid())
		{
			if (DamageDataTable->GetRowMap().Contains(DamageTag.GetTagName()))
			{
				MyRowData = DamageDataTable->FindRow<FDatatableDamageRow>(DamageTag.GetTagName(), "");
				if (MyRowData)
				{
					if (MyRowData->FilterTags.IsEmpty() || MyRowData->FilterTags.HasTag(DamageTag) || MyRowData->FilterTags.HasTag(DamageQuantityTag))
						Result = MyRowData;
				}
			}
		}
	}
	if (Result)
	{
		ResultData = *Result;
		const UAttributeSet* const AttributeSetOrNull = GetAttributeSubobject(DamageSendFactorAttribute.GetAttributeSetClass());
		if (AttributeSetOrNull != nullptr)
		{
			ResultData.DamageValue *= DamageSendFactorAttribute.GetNumericValue(AttributeSetOrNull);;
		}
	}
	else
	{
		ResultData = FDatatableDamageRow();
		ResultData.DamageValue = DefaultDamage;
	}
	return Result != nullptr;
}


UGameplayChangerBase* UAbilitySystemComponentBase::AddGameplayChangerByClass(TSubclassOf<UGameplayChangerBase> GameplayChangerClass)
{
	if (IsValid(GameplayChangerClass))
	{
		UGameplayChangerBase* GameplayChanger = NewObject<UGameplayChangerBase>(GetWorld(), GameplayChangerClass);
		if (GameplayChanger)
			AddGameplayChanger(GameplayChanger);

		return GameplayChanger;
	}
	return nullptr;
}

UGameplayChangerBase* UAbilitySystemComponentBase::GetGameplayChangerByClass(TSubclassOf<UGameplayChangerBase> GameplayChangerClass)
{
	for (auto GameplayChanger : GameplayChangers)
	{
		UClass* GameplayChangerClassObj = GameplayChanger->GetClass();

		if (IsValid(GameplayChanger) && (GameplayChangerClassObj == GameplayChangerClass || GameplayChangerClassObj->IsChildOf(GameplayChangerClass)))
		{
			return GameplayChanger;
		}
	}
	return nullptr;
}

UGameplayChangerBase* UAbilitySystemComponentBase::GetGameplayChangerByTag(FGameplayTag GameplayChangerTag)
{
	for (TObjectPtr<UGameplayChangerBase> GameplayChanger : GameplayChangers)
	{
		if (IsValid(GameplayChanger) && GameplayChanger->GameplayChangerTag == GameplayChangerTag)
		{
			return GameplayChanger;
		}
	}
	return nullptr;
}

bool UAbilitySystemComponentBase::AddGameplayChanger(UGameplayChangerBase* GameplayChanger)
{
	if (!GameplayChanger || !IsValid(GameplayChanger) || GameplayChangers.Contains(GameplayChanger))
		return false;

	GameplayChangers.Add(GameplayChanger);
	GameplayChanger->AbilitySystemComponentBase = this;
	GameplayChanger->InitAttributeChangers();
	GameplayChanger->OnAddedToGAS_Comp(this);
	// Loop through all AttributeChangers to mark them as Dirty
	for (auto KeyValue : GameplayChanger->AttributeChangers)
	{
		DirtyAttributes.AddUnique(KeyValue.Key);
	}
	RefreshAllDirtyAttributes();
	return true;
}

bool UAbilitySystemComponentBase::RemoveGameplayChanger(UGameplayChangerBase* GameplayChanger)
{
	if (!GameplayChanger || !IsValid(GameplayChanger) || !GameplayChangers.Contains(GameplayChanger))
		return false;
	for (auto KeyValue : GameplayChanger->AttributeChangers)
	{
		DirtyAttributes.AddUnique(KeyValue.Key);
	}
	GameplayChangers.Remove(GameplayChanger);
	GameplayChanger->OnRemovedToGAS_Comp(this);
	GameplayChanger->AbilitySystemComponentBase = nullptr;
	RefreshAllDirtyAttributes();
	return true;
}

bool UAbilitySystemComponentBase::RemoveGameplayChangerArray(TArray<UGameplayChangerBase*> GameplayChangerArray)
{
	TArray<TObjectPtr<UGameplayChangerBase>> ToDelete;
	for (auto GameplayChanger : GameplayChangerArray)
	{
		if (!GameplayChanger || !IsValid(GameplayChanger) || !GameplayChangers.Contains(GameplayChanger))
			continue;
		for (auto KeyValue : GameplayChanger->AttributeChangers)
		{
			DirtyAttributes.AddUnique(KeyValue.Key);
		}
		ToDelete.AddUnique(GameplayChanger);
		GameplayChanger->OnRemovedToGAS_Comp(this);
		GameplayChanger->AbilitySystemComponentBase = nullptr;
	}

	for (auto GameplayChangerBase : ToDelete)
	{
		GameplayChangers.Remove(GameplayChangerBase);
	}
	RefreshAllDirtyAttributes();
	return ToDelete.Num() != 0;
}

bool UAbilitySystemComponentBase::RemoveGameplayChangerByTag(FGameplayTag GameplayChangerTag)
{
	if (!GameplayChangerTag.IsValid())
		return false;
	for (auto GameplayChanger : GameplayChangers)
	{
		if (IsValid(GameplayChanger))
		{
			if (GameplayChanger->GameplayChangerTag.IsValid() && GameplayChanger->GameplayChangerTag == GameplayChangerTag)
			{
				for (auto KeyValue : GameplayChanger->AttributeChangers)
				{
					DirtyAttributes.AddUnique(KeyValue.Key);
				}
				GameplayChangers.Remove(GameplayChanger);
				GameplayChanger->OnRemovedToGAS_Comp(this);
				GameplayChanger->AbilitySystemComponentBase = nullptr;

				RefreshAllDirtyAttributes();
				return true;
			}
		}
	}
	return false;
}

bool UAbilitySystemComponentBase::RemoveGameplayChangerByMatchingTag(FGameplayTag GameplayChangerTag)
{
	if (!GameplayChangerTag.IsValid())
		return false;
	TArray<TObjectPtr<UGameplayChangerBase>> ToDelete;
	for (auto GameplayChanger : GameplayChangers)
	{
		if (IsValid(GameplayChanger))
		{
			if (GameplayChanger->GameplayChangerTag.IsValid() && GameplayChanger->GameplayChangerTag.MatchesTag(GameplayChangerTag))
			{
				for (auto KeyValue : GameplayChanger->AttributeChangers)
				{
					DirtyAttributes.AddUnique(KeyValue.Key);
				}
				ToDelete.AddUnique(GameplayChanger);
				GameplayChanger->OnRemovedToGAS_Comp(this);
				GameplayChanger->AbilitySystemComponentBase = nullptr;
			}
		}
	}
	for (auto GameplayChangerBase : ToDelete)
	{
		GameplayChangers.Remove(GameplayChangerBase);
	}
	RefreshAllDirtyAttributes();
	return ToDelete.Num() != 0;
}

void UAbilitySystemComponentBase::RefreshAllDirtyAttributes()
{
	//UE_LOG(LogGAS_Utility, Display, TEXT("RefreshAllDirtyAttributes"));
	TArray<FGameplayAttribute> DirtyAttributesClone = DirtyAttributes;
	for (FGameplayAttribute& Attribute : DirtyAttributesClone)
	{
		if (GetOwnerRole() == ROLE_Authority)
			RefreshAttribute(Attribute);
	}
}

struct FSortAttributeOperation
{
	explicit FSortAttributeOperation(TArray<UAttributeChanger*> BaseArray, FTagPriorityArrayContainer* InOverridePriorityArray)
		: MainArray(BaseArray), OverridePriorityArray(InOverridePriorityArray)
	{
	}

	TArray<UAttributeChanger*> MainArray;
	FTagPriorityArrayContainer* OverridePriorityArray;

	bool operator()(UAttributeChanger* A, UAttributeChanger* B) const
	{
		if (!A || !B)
			return false;
		float APriority = A->Priority;
		float BPriority = B->Priority;
		if (OverridePriorityArray)
		{
			if (A->UniqueAttributeChangerTag.IsValid() || B->UniqueAttributeChangerTag.IsValid())
			{
				for (TTuple<float, FTagPriorityArray> KeyValue : OverridePriorityArray->OverridePriorityFListMap)
				{
					FTagPriorityArray List = KeyValue.Get<FTagPriorityArray>();
					const int32 ResultA = List.TagList.Find(A->UniqueAttributeChangerTag);
					if (ResultA >= 0 && A->UniqueAttributeChangerTag.IsValid())
					{
						APriority = KeyValue.Key;
					}
					const int32 ResultB = List.TagList.Find(B->UniqueAttributeChangerTag);
					if (ResultB >= 0 && B->UniqueAttributeChangerTag.IsValid())
					{
						BPriority = KeyValue.Key;
					}
					if (ResultA >= 0 && ResultB >= 0 && A->UniqueAttributeChangerTag.IsValid() && B->UniqueAttributeChangerTag.IsValid())
					{
						return ResultA < ResultB;
					}
				}
			}
		}
		if (APriority == BPriority)
		{
			return MainArray.Find(A) < MainArray.Find(B);
		}
		else
		{
			return APriority < BPriority;
		}
	}
};

void UAbilitySystemComponentBase::RefreshAttribute(FGameplayAttribute GameplayAttribute)
{
	// Set the attribute directly: update the FProperty on the attribute set.

	if (!GameplayAttribute.GetUProperty())
	{
		UE_LOG(LogAbilitySystem, Error, TEXT("Attribute %s not found/valid"), *GameplayAttribute.GetName());
		return;
	}
	FProperty* Property = GameplayAttribute.GetUProperty();
	if (!(Property->GetPropertyFlags() & CPF_RepSkip))
	{
		if (GetOwnerRole() != ROLE_Authority)
		{
			DirtyAttributes.Remove(GameplayAttribute);
			return;
		}
	}
	if (UAttributeSet* AttributeSet = const_cast<UAttributeSet*>(GetAttributeSubobject(GameplayAttribute.GetAttributeSetClass())))
	{
		TArray<UAttributeChanger*> BaseChanger;
		TArray<UAttributeChanger*> ValueChanger;
		for (auto GameplayChanger : GameplayChangers)
		{
			if (IsValid(GameplayChanger))
			{
				if (!GameplayChanger->AttributeChangers.IsEmpty())
					if (const auto SearchChange = GameplayChanger->AttributeChangers.Find(GameplayAttribute))
					{
						for (auto AttributeChanger : SearchChange->ArrayList)
						{
							if (IsValid(AttributeChanger))
							{
								if (AttributeChanger->Operation == EAttributeChanger::Base)
								{
									BaseChanger.Add(AttributeChanger);
								}
								else
								{
									ValueChanger.Add(AttributeChanger);
								}
							}
						}
					}
			}
		}
		if (!BaseChanger.IsEmpty())
		{
			FTagPriorityArrayContainer* TagPriorityArrayContainer = GameplayChangerOverridePriority.Find(GameplayAttribute);
			Algo::Sort(BaseChanger, FSortAttributeOperation(BaseChanger, TagPriorityArrayContainer));
			float OldValue = ActiveGameplayEffects.GetAttributeBaseValue(GameplayAttribute);
			float NewValue = OldValue;
			for (const auto Changer : BaseChanger)
			{
				if (IsValid(Changer))
				{
					NewValue = Changer->ExecuteChangeOperation(NewValue, this);
				}
			}
			if (!AttributesAlreadyInitialized.Contains(GameplayAttribute) || NewValue != OldValue)
			{
				ActiveGameplayEffects.SetAttributeBaseValue(GameplayAttribute, NewValue);
				AttributesAlreadyInitialized.AddUnique(GameplayAttribute);
			}
		}
		float OldValue = GetNumericAttribute(GameplayAttribute);
		float NewValue = GetNumericAttributeBase(GameplayAttribute);
		if (!ValueChanger.IsEmpty() || OldValue != NewValue)
		{
			FTagPriorityArrayContainer* TagPriorityArrayContainer = GameplayChangerOverridePriority.Find(GameplayAttribute);

			Algo::Sort(ValueChanger, FSortAttributeOperation(ValueChanger, TagPriorityArrayContainer));


			float LastCumulativeMultiplierPriority = -999999.99f;

			for (int i = 0; i < ValueChanger.Num(); ++i)
			{
				const auto Changer = ValueChanger[i];
				if (IsValid(Changer))
				{
					if (Changer->Operation == EAttributeChanger::CumulativeMultiplier)
					{
						if (Changer->Priority > LastCumulativeMultiplierPriority)
						{
							float CumulativeMultiplier = 1.0;
							LastCumulativeMultiplierPriority = Changer->Priority;

							for (int y = i; y < ValueChanger.Num(); ++y)
							{
								const auto SearchCumulativeMultiplierChanger = ValueChanger[y];
								if (IsValid(SearchCumulativeMultiplierChanger))
								{
									if (SearchCumulativeMultiplierChanger->Priority != LastCumulativeMultiplierPriority)
										break;
									if (SearchCumulativeMultiplierChanger->Operation == EAttributeChanger::CumulativeMultiplier)
									{
										CumulativeMultiplier += SearchCumulativeMultiplierChanger->GetCumulativeMultiplier();
										NewValue = Changer->ExecuteChangeOperation(NewValue, this);
									}
								}
							}
							NewValue *= CumulativeMultiplier;
						}
						continue;
					}
					NewValue = Changer->ExecuteChangeOperation(NewValue, this);
				}
			}
			if (OldValue != NewValue)
			{
				SetNumericAttribute_Internal(GameplayAttribute, NewValue);
				FOnGameplayAttributeValueChange NewDelegate = GetGameplayAttributeValueChangeDelegate(GameplayAttribute);
				FOnAttributeChangeData CallbackData;
				CallbackData.Attribute = GameplayAttribute;
				CallbackData.NewValue = NewValue;
				CallbackData.OldValue = OldValue;
				NewDelegate.Broadcast(CallbackData);
			}
		}
		DirtyAttributes.Remove(GameplayAttribute);
	}
}

void UAbilitySystemComponentBase::AddAttributeSet(UAttributeSet* Attributes)
{
	AddAttributeSetSubobject<UAttributeSet>(Attributes);
	for (TFieldIterator<FProperty> PropertyIterator(Attributes->GetClass()); PropertyIterator; ++PropertyIterator)
	{
		if (const FProperty* Property = *PropertyIterator)
		{
			const FName AttributeName = FName(Property->GetName());
			FProperty* NProperty = Attributes->GetClass()->FindPropertyByName(AttributeName);
			if (!NProperty)
			{
				continue;
			}
			FGameplayAttribute Attribute = FGameplayAttribute(NProperty);
			if (Attribute.IsValid())
				DirtyAttributes.Add(Attribute);
		}
	}
}

void UAbilitySystemComponentBase::SetReplicatedAttributeValue(FGameplayAttribute Attribute, float NewValue, float OldValue)
{
	if (OldValue != NewValue)
	{
		if (GetAttributeSubobject(Attribute.GetAttributeSetClass()))
			SetNumericAttribute_Internal(Attribute, NewValue);
		FOnGameplayAttributeValueChange NewDelegate = GetGameplayAttributeValueChangeDelegate(Attribute);
		FOnAttributeChangeData CallbackData;
		CallbackData.Attribute = Attribute;
		CallbackData.NewValue = NewValue;
		CallbackData.OldValue = OldValue;
		NewDelegate.Broadcast(CallbackData);
	}
}

UAttributeChanger* UAbilitySystemComponentBase::CreateLooseAttributeChanger(FGameplayAttribute Attribute, FGameplayTag Tag)
{
	return CreateLooseAttributeChangerWithData(Attribute, Tag, EAttributeChanger::SetValue, 0.f, 0.f);
}

UAttributeChanger* UAbilitySystemComponentBase::CreateLooseAttributeChangerWithData(FGameplayAttribute Attribute, FGameplayTag Tag, TEnumAsByte<EAttributeChanger::Type> Operation, float Value, float Priority)
{
	UGameplayChangerBase* TempGameplayChanger = NewObject<UGameplayChangerBase>();
	UAttributeChanger* TempGameplayAttribute = NewObject<UAttributeChanger>();
	TMap<FGameplayAttribute, FAttributeChangerArray> Attributes;
	FAttributeChangerArray TempArray = FAttributeChangerArray();

	TempGameplayAttribute->UniqueAttributeChangerTag = Tag;
	TempGameplayAttribute->SetOperation(Operation);
	TempGameplayAttribute->SetFloatValue(Value);
	TempGameplayAttribute->SetPriorityValue(Priority);

	TempArray.ArrayList.Add(TempGameplayAttribute);
	Attributes.Add(Attribute, TempArray);
	TempGameplayChanger->SetAttributeChangers(Attributes);
	AddGameplayChanger(TempGameplayChanger);

	return TempGameplayAttribute;
}

bool UAbilitySystemComponentBase::RemoveLooseAttributeChanger(UAttributeChanger* AttributeChanger, bool bRemoveGameplayChangerParent)
{
	if (IsValid(AttributeChanger))
	{
		if (AttributeChanger->GameplayChangerParent.IsValid())
		{
			if (bRemoveGameplayChangerParent)
			{
				if (AttributeChanger->GameplayChangerParent->AbilitySystemComponentBase.IsValid())
				{
					AttributeChanger->GameplayChangerParent->AbilitySystemComponentBase->RemoveGameplayChanger(AttributeChanger->GameplayChangerParent.Get());
					return true;
				}
			}
			else
			{
				if (FAttributeChangerArray* Value = AttributeChanger->GameplayChangerParent->AttributeChangers.Find(AttributeChanger->GameplayAttribute))
				{
					Value->ArrayList.Remove(AttributeChanger);
					AttributeChanger->GameplayChangerParent->TriggerRefreshAttribute(AttributeChanger->GameplayAttribute);
					AttributeChanger->GameplayChangerParent = nullptr;
					return true;
				}
				AttributeChanger->GameplayChangerParent = nullptr;
				return false;
			}
		}
	}
	return false;
}

UObject* UAbilitySystemComponentBase::GetFirstObjectStoredWithGameplayTag(const FGameplayTag Tag)
{
	FScopeLock MyLock(&StoreObjectsMutex);
	if (!Tag.IsValid())
		return nullptr;
	if (FObjectArray* Result = ObjectsStoredByGameplayTag.Find(Tag))
	{
		if (Result->ArrayList.Num())
		{
			const TObjectPtr<UObject> Object = Result->ArrayList[0];
			if (Object)
				return Object.Get();
		}
	}
	return nullptr;
}

TArray<UObject*> UAbilitySystemComponentBase::GetObjectsStoredWithGameplayTag(const FGameplayTag Tag)
{
	FScopeLock MyLock(&StoreObjectsMutex);
	TArray<UObject*> OutResult;
	if (!Tag.IsValid())
		return OutResult;
	if (FObjectArray* Result = ObjectsStoredByGameplayTag.Find(Tag))
	{
		for (auto Item : Result->ArrayList)
		{
			if (IsValid(Item))
			{
				OutResult.Add(Item.Get());
			}
		}
	}
	return OutResult;
}

void UAbilitySystemComponentBase::AddObjectsStoredWithGameplayTag(const FGameplayTag Tag, TArray<UObject*> Array)
{
	FScopeLock MyLock(&StoreObjectsMutex);
	if (!Tag.IsValid())
		return;
	FObjectArray* Result = ObjectsStoredByGameplayTag.Find(Tag);
	if (!Result)
	{
		ObjectsStoredByGameplayTag.Add(Tag);
		Result = ObjectsStoredByGameplayTag.Find(Tag);
	}
	if (!Result)
		return;
	for (auto Object : Array)
	{
		if (Object && IsValid(Object))
			Result->ArrayList.Add(Object);
	}
	TriggerObjectStoredChangeDelegates(Tag);
}

void UAbilitySystemComponentBase::TriggerObjectStoredChangeDelegates(const FGameplayTag Tag)
{
	if (StoredObjectChangeDelegates.Contains(Tag))
	{
		FOnStoredObjectChange* Result = GetStoredObjectChangeDelegates(Tag);
		if (Result)
			Result->Broadcast(Tag);
	}
}

FOnStoredObjectChange* UAbilitySystemComponentBase::GetStoredObjectChangeDelegates(FGameplayTag Tag)
{
	return &StoredObjectChangeDelegates.FindOrAdd(Tag);
}

void UAbilitySystemComponentBase::BindObjectStoredChangeDelegate(FOnStoredObjectChangeBP Delegate, FGameplayTag Tag)
{
	if (Tag.IsValid() && Delegate.IsBound())
	{
		auto StoredObjectDelegate = GetStoredObjectChangeDelegates(Tag);
		if (StoredObjectDelegate)
		{
			StoredObjectDelegate->AddUnique(Delegate);
		}
	}
}

void UAbilitySystemComponentBase::UnbindObjectStoredChangeDelegate(FOnStoredObjectChangeBP Delegate, FGameplayTag Tag)
{
	if (Tag.IsValid() && Delegate.IsBound())
	{
		auto StoredObjectDelegate = GetStoredObjectChangeDelegates(Tag);
		if (StoredObjectDelegate)
		{
			StoredObjectDelegate->Remove(Delegate);
		}
	}
}

TArray<AActor*> UAbilitySystemComponentBase::GetActorStoredWithGameplayTagSortByAngle(FVector Direction, FVector Origin, FGameplayTag Tag, FString SceneComponentTag)
{
	FScopeLock MyLock(&StoreObjectsMutex);
	TArray<AActor*> Res = TArray<AActor*>();
	TArray<double> Angles = TArray<double>();
	auto Objects = GetObjectsStoredWithGameplayTag(Tag);
	for (auto Object : Objects)
	{
		if (Res.Contains(Object))
			continue;
		if (Object && IsValid(Object))
		{
			AActor* Actor = Cast<AActor>(Object);
			if (Actor && IsValid(Actor))
			{
				FVector Location = Actor->GetActorLocation();
				if (SceneComponentTag != "")
				{
					TArray<UActorComponent*> Comp = Actor->GetComponentsByTag(USceneComponent::StaticClass(), FName(SceneComponentTag));
					for (const auto ActorComponent : Comp)
					{
						const USceneComponent* SceneComp = Cast<USceneComponent>(ActorComponent);
						if (SceneComp && IsValid(SceneComp))
						{
							Location = SceneComp->GetComponentLocation();
							break;
						}
					}
				}
				const FVector CurrentDirection = (Location - Origin).GetSafeNormal();
				const double Angle = FMath::RadiansToDegrees(FMath::Acos(Direction.CosineAngle2D(CurrentDirection)));

				bool Added = false;

				for (int i = 0; i < Res.Num(); i++)
				{
					if (Angles[i] > Angle)
					{
						Res.Insert(Actor, i);
						Angles.Insert(Angle, i);
						Added = true;
						break;
					}
				}
				if (!Added)
				{
					Res.Add(Actor);
					Angles.Add(Angle);
				}
			}
		}
	}
	return Res;
}

TArray<AActor*> UAbilitySystemComponentBase::GetActorStoredWithGameplayTagSortByDistance(FVector Location, FGameplayTag Tag, FString SceneComponentTag)
{
	FScopeLock MyLock(&StoreObjectsMutex);
	TArray<AActor*> Res = TArray<AActor*>();
	TArray<double> Distances = TArray<double>();
	auto Objects = GetObjectsStoredWithGameplayTag(Tag);
	for (auto Object : Objects)
	{
		if (Res.Contains(Object))
			continue;
		if (Object && IsValid(Object))
		{
			AActor* Actor = Cast<AActor>(Object);
			if (Actor && IsValid(Actor))
			{
				FVector ObjectLocation = Actor->GetActorLocation();
				if (SceneComponentTag != "")
				{
					TArray<UActorComponent*> Comp = Actor->GetComponentsByTag(USceneComponent::StaticClass(), FName(SceneComponentTag));
					for (const auto ActorComponent : Comp)
					{
						const USceneComponent* SceneComp = Cast<USceneComponent>(ActorComponent);
						if (SceneComp && IsValid(SceneComp))
						{
							ObjectLocation = SceneComp->GetComponentLocation();
							break;
						}
					}
				}

				const double Distance = FVector::Distance(Location, ObjectLocation);

				bool Added = false;

				for (int i = 0; i < Res.Num(); i++)
				{
					if (Distances[i] > Distance)
					{
						Res.Insert(Actor, i);
						Distances.Insert(Distance, i);
						Added = true;
						break;
					}
				}
				if (!Added)
				{
					Res.Add(Actor);
					Distances.Add(Distance);
				}
			}
		}
	}
	return Res;
}

void UAbilitySystemComponentBase::AddObjectStoredWithGameplayTag(const FGameplayTag Tag, UObject* Object)
{
	FScopeLock MyLock(&StoreObjectsMutex);
	if (!Tag.IsValid())
		return;
	FObjectArray* Result = ObjectsStoredByGameplayTag.Find(Tag);
	if (!Result)
	{
		ObjectsStoredByGameplayTag.Add(Tag);
		Result = ObjectsStoredByGameplayTag.Find(Tag);
	}
	if (!Result)
		return;
	if (Object && IsValid(Object))
		Result->ArrayList.Add(Object);
	TriggerObjectStoredChangeDelegates(Tag);
}

void UAbilitySystemComponentBase::ClearObjectsStoredWithGameplayTag(const FGameplayTag Tag)
{
	FScopeLock MyLock(&StoreObjectsMutex);
	ObjectsStoredByGameplayTag.Remove(Tag);
	TriggerObjectStoredChangeDelegates(Tag);
}

void UAbilitySystemComponentBase::RemoveObjectsStoredWithGameplayTag(const FGameplayTag Tag, TArray<UObject*> Array)
{
	FScopeLock MyLock(&StoreObjectsMutex);
	if (!Tag.IsValid())
		return;
	FObjectArray* Result = ObjectsStoredByGameplayTag.Find(Tag);
	if (!Result)
		return;

	for (auto Object : Array)
	{
		if (Object && IsValid(Object))
			Result->ArrayList.Remove(Object);
	}
	TriggerObjectStoredChangeDelegates(Tag);
}

void UAbilitySystemComponentBase::RemoveObjectStoredWithGameplayTag(const FGameplayTag Tag, UObject* Object)
{
	FScopeLock MyLock(&StoreObjectsMutex);
	if (!Tag.IsValid())
		return;
	FObjectArray* Result = ObjectsStoredByGameplayTag.Find(Tag);
	if (!Result)
		return;

	if (Object && IsValid(Object))
		Result->ArrayList.Remove(Object);
	TriggerObjectStoredChangeDelegates(Tag);
}

void UAbilitySystemComponentBase::SetObjectStoredWithGameplayTag(const FGameplayTag Tag, UObject* Object)
{
	FScopeLock MyLock(&StoreObjectsMutex);
	if (!Tag.IsValid())
		return;
	FObjectArray* Result = ObjectsStoredByGameplayTag.Find(Tag);
	if (!Result)
	{
		ObjectsStoredByGameplayTag.Add(Tag);
		Result = ObjectsStoredByGameplayTag.Find(Tag);
	}
	if (!Result)
		return;
	if (!Result->ArrayList.IsEmpty())
		Result->ArrayList.RemoveAt(0);
	Result->ArrayList.Insert(Object, 0);
	TriggerObjectStoredChangeDelegates(Tag);
}

UGameplayAbility* UAbilitySystemComponentBase::GetGameplayAbilityFromClass(TSubclassOf<UGameplayAbility> InAbilityToActivate)
{
	if (const auto Spec = FindAbilitySpecFromClass(InAbilityToActivate))
		return Spec->GetPrimaryInstance();
	return nullptr;
}

UGameplayAbility* UAbilitySystemComponentBase::GetGameplayAbilityFromTag(const FGameplayTag AbilityTag)
{
	for (FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{
		if (Spec.Ability && Spec.Ability->AbilityTags.HasTagExact(AbilityTag))
		{
			return Spec.GetPrimaryInstance();
		}
	}
	return nullptr;
}

float UAbilitySystemComponentBase::GetAttribute(FGameplayAttribute Attribute) const
{
	bool bSuccess = false;
	const float Res = GetGameplayAttributeValue(Attribute, bSuccess);
	if (bSuccess)
		return Res;
	return -1.0f;
}

FAbilitySystemComponentTagAccessor UAbilitySystemComponentBase::GetTagAccessor()
{
	FAbilitySystemComponentTagAccessor Accessor = FAbilitySystemComponentTagAccessor();
	Accessor.AbilitySystemComponent = this;
	return Accessor;
}

bool UAbilitySystemComponentBase::AccessorHasMatchingTag(FAbilitySystemComponentTagAccessor Accessor, FGameplayTag Tag)
{
	if (Accessor.AbilitySystemComponent.IsValid())
	{
		return Accessor.AbilitySystemComponent->HasMatchingGameplayTag(Tag);
	}
	return false;
}

float UAbilitySystemComponentBase::GetGameplayAttributeValueByAccessor(FAbilitySystemComponentTagAccessor Accessor, FGameplayAttribute Attribute)
{
	if (Accessor.AbilitySystemComponent.IsValid())
	{
		bool bSuccess = false;
		const float Res = Accessor.AbilitySystemComponent->GetGameplayAttributeValue(Attribute, bSuccess);
		if (bSuccess)
			return Res;
	}
	return -1.0f;
}

float UAbilitySystemComponentBase::GetGameplayCooldownByAccessor(FAbilitySystemComponentTagAccessor Accessor, FGameplayTag Tag)
{
	if (Accessor.AbilitySystemComponent.IsValid())
	{
		UAbilitySystemComponentBase* CastedComp = Cast<UAbilitySystemComponentBase>(Accessor.AbilitySystemComponent.Get());
		if (CastedComp && IsValid(CastedComp))
			return CastedComp->GetCooldownFromTag(Tag);
	}
	return -1.0f;
}

UObject* UAbilitySystemComponentBase::GetObjectStoreWithGameplayTagByAccessor(FAbilitySystemComponentTagAccessor Accessor, FGameplayTag Tag, bool& bIsValid)
{
	bIsValid = false;
	if (Accessor.AbilitySystemComponent.IsValid())
	{
		UAbilitySystemComponentBase* CastedComp = Cast<UAbilitySystemComponentBase>(Accessor.AbilitySystemComponent.Get());
		if (CastedComp && IsValid(CastedComp))
		{
			UObject* Result = CastedComp->GetFirstObjectStoredWithGameplayTag(Tag);
			bIsValid = IsValid(Result);
			return Result;
		}
	}
	return nullptr;
}

TArray<UObject*> UAbilitySystemComponentBase::GetObjectsStoreWithGameplayTagByAccessor(FAbilitySystemComponentTagAccessor Accessor, FGameplayTag Tag)
{
	if (Accessor.AbilitySystemComponent.IsValid())
	{
		UAbilitySystemComponentBase* CastedComp = Cast<UAbilitySystemComponentBase>(Accessor.AbilitySystemComponent.Get());
		if (CastedComp && IsValid(CastedComp))
			return CastedComp->GetObjectsStoredWithGameplayTag(Tag);
	}
	TArray<UObject*> Res;
	return Res;
}

AController* UAbilitySystemComponentBase::GetAvatarController()
{
	if (AActor* Avatar = GetAvatarActor())
	{
		if (const ACharacter* Character = Cast<ACharacter>(Avatar))
		{
			return Character->GetController();
		}
		if (GetOwnerActor()->GetInstigatorController())
			return GetOwnerActor()->GetInstigatorController();
	}
	if (GetOwnerActor())
	{
		return GetOwnerActor()->GetInstigatorController();
	}
	return nullptr;
}

AActor* UAbilitySystemComponentBase::GetAvatar()
{
	return GetAvatarActor();
}

FInputActionInstance UAbilitySystemComponentBase::GetInputActionInstance(UInputAction* InputAction)
{
	if (AController* Controller = GetAvatarController())
	{
		if (const APlayerController* PlayerController = Cast<APlayerController>(Controller))
		{
			if (const TObjectPtr<UPlayerInput> PlayerInput = PlayerController->PlayerInput)
			{
				if (const UEnhancedPlayerInput* EnhancedPlayerInput = Cast<UEnhancedPlayerInput>(PlayerInput))
				{
					if (const FInputActionInstance* InputActionInstance = EnhancedPlayerInput->FindActionInstanceData(InputAction))
						return *(InputActionInstance);
				}
			}
		}
	}
	return FInputActionInstance();
}

bool UAbilitySystemComponentBase::GetInputActionInstanceValueAsBool(UInputAction* InputAction)
{
	const auto InInputActionInstance = GetInputActionInstance(InputAction);
	return InInputActionInstance.GetValue().Get<bool>();
}

float UAbilitySystemComponentBase::GetInputActionInstanceValueAsFloat(UInputAction* InputAction)
{
	const auto InInputActionInstance = GetInputActionInstance(InputAction);
	return InInputActionInstance.GetValue().Get<float>();
}

FVector2D UAbilitySystemComponentBase::GetInputActionInstanceValueAsVector2D(UInputAction* InputAction)
{
	const auto InInputActionInstance = GetInputActionInstance(InputAction);
	return InInputActionInstance.GetValue().Get<FVector2d>();
}

FVector UAbilitySystemComponentBase::GetInputActionInstanceValueAsVector(UInputAction* InputAction)
{
	const auto InInputActionInstance = GetInputActionInstance(InputAction);
	return InInputActionInstance.GetValue().Get<FVector>();
}

AActor* UAbilitySystemComponentBase::GetGameplayTaskAvatar(const UGameplayTask* Task) const
{
	const auto Owner = GetOwnerActor();
	if (const auto Character = Cast<ACharacter>(Owner))
		return Character;
	if (const auto PlayerState = Cast<APlayerState>(Owner))
	{
		if (IsValid(PlayerState->GetPawn()))
		{
			return Cast<ACharacter>(PlayerState->GetPawn());
		}
	}
	return Super::GetGameplayTaskAvatar(Task);
}

UAbilitySystemComponentBase* UAbilitySystemComponentBase::GetAbilitySystemComp(const AActor* Actor)
{
	if (!IsValid(Actor))
		return nullptr;
	if (const auto AbilitySystemComp = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor))
	{
		return Cast<UAbilitySystemComponentBase>(AbilitySystemComp);
	}
	return nullptr;
}

UGameplayAbility* UAbilitySystemComponentBase::CreateNewInstanceOfAbility(FGameplayAbilitySpec& Spec, const UGameplayAbility* Ability)
{
	if (!IsValid(Ability))
		return nullptr;
	auto AbilityInstance = Super::CreateNewInstanceOfAbility(Spec, Ability);
	if (auto MyAbilityInstance = Cast<UGameplayAbilityBase>(AbilityInstance))
	{
	}
	return AbilityInstance;
}

void UAbilitySystemComponentBase::InitializeComponent()
{
	Super::InitializeComponent();
}

void UAbilitySystemComponentBase::OnTagUpdated(const FGameplayTag& Tag, bool TagExists)
{
	Super::OnTagUpdated(Tag, TagExists);
	OnTagUpdate.Broadcast(Tag, TagExists);
}

void UAbilitySystemComponentBase::OnGiveAbility(FGameplayAbilitySpec& AbilitySpec)
{
	Super::OnGiveAbility(AbilitySpec);

	if (AbilitySpec.GetPrimaryInstance())
	{
		OnAbilityAdded.Broadcast(AbilitySpec.GetPrimaryInstance());
	}
}

void UAbilitySystemComponentBase::OnRemoveAbility(FGameplayAbilitySpec& AbilitySpec)
{
	if (AbilitySpec.GetPrimaryInstance())
	{
		OnAbilityRemoved.Broadcast(AbilitySpec.GetPrimaryInstance());
	}
	Super::OnRemoveAbility(AbilitySpec);
}

void UAbilitySystemComponentBase::AddCooldownTag(FGameplayTag Tag, float Duration)
{
	if (IsServer())
	{
		if (Duration <= 0)
			return;
		UGameplayEffect* GameplayEffect = NewObject<UNonReplicatedGameplayEffect>(GetTransientPackage(), FName(TEXT("CooldownGameplayEffect")));
		GameplayEffect->DurationPolicy = EGameplayEffectDurationType::HasDuration;
		GameplayEffect->DurationMagnitude = FGameplayEffectModifierMagnitude(Duration);


		FGameplayEffectContextHandle ContextHandle = MakeEffectContext();
		FGameplayEffectSpecHandle SpecHandle = FGameplayEffectSpecHandle(new FGameplayEffectSpec(GameplayEffect, ContextHandle));

		SpecHandle.Data->DynamicGrantedTags.AddTag(Tag);
		const FActiveGameplayEffectHandle ActiveGE = ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

		CooldownHandlesByTag.Add(Tag, ActiveGE);
		auto Delegate = OnGameplayEffectRemoved_InfoDelegate(ActiveGE);
		if (this)
			Delegate->AddUObject(this, &UAbilitySystemComponentBase::RemoveCooldownTagAtEnd, Tag);
	}
}

bool UAbilitySystemComponentBase::IsServer()
{
	UWorld* World = GetWorld();
	return World ? (World->GetNetMode() != NM_Client) : false;
}

void UAbilitySystemComponentBase::RemoveCooldownTagAtEnd(const FGameplayEffectRemovalInfo& Info, FGameplayTag Tag)
{
	CooldownHandlesByTag.Remove(Tag);
}

void UAbilitySystemComponentBase::RemoveCooldownTag(FGameplayTag Tag)
{
	auto ActiveHandle = CooldownHandlesByTag.Find(Tag);
	if (ActiveHandle)
	{
		RemoveActiveGameplayEffect(*ActiveHandle, 1);
		CooldownHandlesByTag.Remove(Tag);
	}
}

float UAbilitySystemComponentBase::GetCooldownFromTag(FGameplayTag Tag)
{
	auto ActiveHandle = CooldownHandlesByTag.Find(Tag);
	if (ActiveHandle)
	{
		auto ActiveGameplayEffect = GetActiveGameplayEffect(*ActiveHandle);
		if (ActiveGameplayEffect)
			return ActiveGameplayEffect->GetTimeRemaining(GetWorld()->GetTimeSeconds());
		CooldownHandlesByTag.Remove(Tag);
	}
	return -1.f;
}


bool UAbilitySystemComponentBase::IsActionTagHigherPriorityForBufferInput(const FGameplayTag Tag)
{
	for (const auto ActionTag : ActionTagPriorityForBufferInput)
	{
		if (HasMatchingGameplayTag(ActionTag))
		{
			return (Tag == ActionTag);
		}
	}
	return false;
}

void UAbilitySystemComponentBase::OnTagUpdateEvent(FGameplayTag Tag, bool TagExist)
{
	OnGameplayTagUpdate_ServerReplicate(Tag, TagExist);
	if (TagChangeDelegates.Contains(Tag))
	{
		FOnStoredObjectChange* Result = GetTagChangeDelegates(Tag);
		if (Result)
			Result->Broadcast(Tag);
	}
}

void UAbilitySystemComponentBase::BindTagChangeDelegate(FOnStoredObjectChangeBP Delegate, FGameplayTag Tag)
{
	if (Tag.IsValid() && Delegate.IsBound())
	{
		auto TagDelegate = GetTagChangeDelegates(Tag);
		if (TagDelegate)
		{
			TagDelegate->AddUnique(Delegate);
		}
	}
}

void UAbilitySystemComponentBase::UnbindTagChangeDelegate(FOnStoredObjectChangeBP Delegate, FGameplayTag Tag)
{
	if (Tag.IsValid() && Delegate.IsBound())
	{
		auto TagDelegate = GetTagChangeDelegates(Tag);
		if (TagDelegate)
		{
			TagDelegate->Remove(Delegate);
		}
	}
}

FOnStoredObjectChange* UAbilitySystemComponentBase::GetTagChangeDelegates(FGameplayTag Tag)
{
	return &TagChangeDelegates.FindOrAdd(Tag);
}


void UAbilitySystemComponentBase::BindNewAbilitySystemComponentDelegate(FNewAbilitySystemComponentDelegate Delegate)
{
	if (Delegate.IsBound())
	{
		NewAbilitySystemComponentDelegate.AddUnique(Delegate);
	}
}

void UAbilitySystemComponentBase::UnbindNewAbilitySystemComponentDelegate(FNewAbilitySystemComponentDelegate Delegate)
{
	if (Delegate.IsBound())
	{
		NewAbilitySystemComponentDelegate.Remove(Delegate);
	}
}

TArray<UAbilitySystemComponentBase*> UAbilitySystemComponentBase::GetArrayAbilitySystemComponents()
{
	TArray<UAbilitySystemComponentBase*> Result;
	for (TObjectPtr<UAbilitySystemComponentBase> Comp : StaticArrayAbilitySystemComponents)
	{
		if (Comp)
		{
			Result.Add(Comp.Get());
		}
	}
	return Result;
}

bool UAbilitySystemComponentBase::IsThereAbilityBehaviourTaskToForce()
{
	return ForceExecuteAbilityBehaviourTaskTag.IsValid();
}

void UAbilitySystemComponentBase::SetForceExecuteAbilityBehaviourTaskTag(const FGameplayTag NewTag)
{
	ForceExecuteAbilityBehaviourTaskTag = NewTag;
	OnForceExecuteAbilityBehaviourTaskTagChange.Broadcast(NewTag);
}

float UAbilitySystemComponentBase::PlayMontage(UGameplayAbility* InAnimatingAbility, FGameplayAbilityActivationInfo ActivationInfo, UAnimMontage* NewAnimMontage, float InPlayRate, FName StartSectionName, float StartTimeSeconds)
{
	float Duration = -1.f;

	UAnimInstance* AnimInstance = AbilityActorInfo.IsValid() ? AbilityActorInfo->GetAnimInstance() : nullptr;
	if (AnimInstance && NewAnimMontage)
	{
		Duration = AnimInstance->Montage_Play(NewAnimMontage, InPlayRate, EMontagePlayReturnType::MontageLength, StartTimeSeconds, bStopAllMontageWhenPlayNew);
		if (Duration > 0.f)
		{
			if (const UGameplayAbility* RawAnimatingAbility = LocalAnimMontageInfo.AnimatingAbility.Get())
			{
				if (RawAnimatingAbility != InAnimatingAbility)
				{
					// The ability that was previously animating will have already gotten the 'interrupted' callback.
					// It may be a good idea to make this a global policy and 'cancel' the ability.
					// 
					// For now, we expect it to end itself when this happens.
				}
			}

			if (NewAnimMontage->HasRootMotion() && AnimInstance->GetOwningActor())
			{
				UE_LOG(LogRootMotion, Log, TEXT("UAbilitySystemComponent::PlayMontage %s, Role: %s")
				       , *GetNameSafe(NewAnimMontage)
				       , *UEnum::GetValueAsString(TEXT("Engine.ENetRole"), AnimInstance->GetOwningActor()->GetLocalRole())
				);
			}

			LocalAnimMontageInfo.AnimMontage = NewAnimMontage;
			LocalAnimMontageInfo.AnimatingAbility = InAnimatingAbility;
			LocalAnimMontageInfo.PlayInstanceId = (LocalAnimMontageInfo.PlayInstanceId < UINT8_MAX ? LocalAnimMontageInfo.PlayInstanceId + 1 : 0);

			if (InAnimatingAbility)
			{
				InAnimatingAbility->SetCurrentMontage(NewAnimMontage);
			}

			// Start at a given Section.
			if (StartSectionName != NAME_None)
			{
				AnimInstance->Montage_JumpToSection(StartSectionName, NewAnimMontage);
			}

			// Replicate for non-owners and for replay recordings
			// The data we set from GetRepAnimMontageInfo_Mutable() is used both by the server to replicate to clients and by clients to record replays.
			// We need to set this data for recording clients because there exists network configurations where an abilities montage data will not replicate to some clients (for example: if the client is an autonomous proxy.)
			if (ShouldRecordMontageReplication())
			{
				FGameplayAbilityRepAnimMontage& MutableRepAnimMontageInfo = GetRepAnimMontageInfo_Mutable();

				// Those are static parameters, they are only set when the montage is played. They are not changed after that.
				MutableRepAnimMontageInfo.AnimMontage = NewAnimMontage;
				MutableRepAnimMontageInfo.PlayInstanceId = (MutableRepAnimMontageInfo.PlayInstanceId < UINT8_MAX ? MutableRepAnimMontageInfo.PlayInstanceId + 1 : 0);

				MutableRepAnimMontageInfo.SectionIdToPlay = 0;
				if (MutableRepAnimMontageInfo.AnimMontage && StartSectionName != NAME_None)
				{
					// we add one so INDEX_NONE can be used in the on rep
					MutableRepAnimMontageInfo.SectionIdToPlay = MutableRepAnimMontageInfo.AnimMontage->GetSectionIndex(StartSectionName) + 1;
				}

				// Update parameters that change during Montage life time.
				AnimMontage_UpdateReplicatedData();
			}
		}
	}

	return Duration;
}

void UAbilitySystemComponentBase::SetStopAllMontageWhenPlayNew(const bool bNewStopAllMontageWhenPlayNew)
{
	bStopAllMontageWhenPlayNew = bNewStopAllMontageWhenPlayNew;
}

void UAbilitySystemComponentBase::MulticastRefreshGameplayTagUpdateEvent_Implementation(FGameplayTag EventTag, int32 Count)
{
	if (GetOwnerRole() != ROLE_Authority)
	{
		if (HasGameplayTag(EventTag) != (Count != 0))
		{
			SetGameplayTagCount(EventTag, Count);
			//next tick
		}
		else
		{
			OnTagUpdate.Broadcast(EventTag, Count != 0);
		}
	}
}

bool UAbilitySystemComponentBase::MulticastRefreshGameplayTagUpdateEvent_Validate(FGameplayTag EventTag, int32 Count)
{
	return true;
}


void UAbilitySystemComponentBase::SendServerOnlyGameplayEvent(FGameplayTag EventTag, FGameplayEventData Payload)
{
	if (GetOwnerRole() == ROLE_Authority)
		HandleGameplayEvent(EventTag, &Payload);
	else if (GetOwnerRole() == ROLE_AutonomousProxy)
		RunOnServerOnly_SendGameplayEvent(EventTag, Payload);
	else
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
		auto Char = PC ? Cast<ACharacter>(PC->GetPawn()) : nullptr;
		if (Char)
		{
			IAbilitySystemInterface* AsAbilitySysInterface = Cast<IAbilitySystemInterface>(Char);
			if (AsAbilitySysInterface)
			{
				if (UAbilitySystemComponent* GasComp = AsAbilitySysInterface->GetAbilitySystemComponent())
				{
					if (UAbilitySystemComponentBase* GasCompCasted = Cast<UAbilitySystemComponentBase>(GasComp))
					{
						GasCompCasted->RunOnServerOnly_SendGameplayEvent_ToActor(GetAvatar(), EventTag, Payload);
					}
				}
			}
		}
	}
}

void UAbilitySystemComponentBase::SendServerOnlyGameplayEvent_VC(FGameplayTag EventTag, UVariableContainer* Payload)
{
	if (GetOwnerRole() == ROLE_Authority)
		HandleGameplayEvent_VariableContainer(EventTag, FGAS_Utility_VariableContainerStruct(Payload));
	else if (GetOwnerRole() == ROLE_AutonomousProxy)
		RunOnServerOnly_SendGameplayEvent_VC(EventTag, FGAS_Utility_VariableContainerStruct(Payload));
	else
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
		auto Char = PC ? Cast<ACharacter>(PC->GetPawn()) : nullptr;
		if (Char)
		{
			IAbilitySystemInterface* AsAbilitySysInterface = Cast<IAbilitySystemInterface>(Char);
			if (AsAbilitySysInterface)
			{
				if (UAbilitySystemComponent* GasComp = AsAbilitySysInterface->GetAbilitySystemComponent())
				{
					if (UAbilitySystemComponentBase* GasCompCasted = Cast<UAbilitySystemComponentBase>(GasComp))
					{
						GasCompCasted->RunOnServerOnly_SendGameplayEvent_ToActor_VC(GetAvatar(), EventTag, FGAS_Utility_VariableContainerStruct(Payload));
					}
				}
			}
		}
	}
}

void UAbilitySystemComponentBase::RunOnServerOnly_SendGameplayEvent_VC_Implementation(FGameplayTag EventTag, FGAS_Utility_VariableContainerStruct Payload)
{
	if (GetOwnerRole() == ROLE_Authority)
		HandleGameplayEvent_VariableContainer(EventTag, Payload);
}

void UAbilitySystemComponentBase::RunOnServerOnly_SendGameplayEvent_Implementation(FGameplayTag EventTag, FGameplayEventData Payload)
{
	if (GetOwnerRole() == ROLE_Authority)
		HandleGameplayEvent(EventTag, &Payload);
}

void UAbilitySystemComponentBase::RunOnServerOnly_SendGameplayEvent_ToActor_Implementation(AActor* Actor, FGameplayTag EventTag, FGameplayEventData Payload)
{
	if (IsValid(Actor))
	{
		IAbilitySystemInterface* AsAbilitySysInterface = Cast<IAbilitySystemInterface>(Actor);
		if (AsAbilitySysInterface)
		{
			if (UAbilitySystemComponent* GasComp = AsAbilitySysInterface->GetAbilitySystemComponent())
			{
				if (UAbilitySystemComponentBase* GasCompCasted = Cast<UAbilitySystemComponentBase>(GasComp))
				{
					GasCompCasted->HandleGameplayEvent(EventTag, &Payload);
				}
			}
		}
	}
}

void UAbilitySystemComponentBase::RunOnServerOnly_SendGameplayEvent_ToActor_VC_Implementation(AActor* Actor, FGameplayTag EventTag, FGAS_Utility_VariableContainerStruct Payload)
{
	if (IsValid(Actor))
	{
		IAbilitySystemInterface* AsAbilitySysInterface = Cast<IAbilitySystemInterface>(Actor);
		if (AsAbilitySysInterface)
		{
			if (UAbilitySystemComponent* GasComp = AsAbilitySysInterface->GetAbilitySystemComponent())
			{
				if (UAbilitySystemComponentBase* GasCompCasted = Cast<UAbilitySystemComponentBase>(GasComp))
				{
					GasCompCasted->HandleGameplayEvent_VariableContainer(EventTag, Payload);
				}
			}
		}
	}
}

void UAbilitySystemComponentBase::SendServerGameplayEventMulticast(FGameplayTag EventTag, FGameplayEventData Payload)
{
	if (GetOwnerRole() == ROLE_Authority)
		MulticastSendGameplayEvent(EventTag, Payload);
	else
	{
		if (IgnoreLocalGameplayEventCount.Contains(EventTag))
			IgnoreLocalGameplayEventCount.Add(EventTag, IgnoreLocalGameplayEventCount[EventTag] + 1);
		else
			IgnoreLocalGameplayEventCount.Add(EventTag, 1);
		HandleGameplayEvent(EventTag, &Payload);
		if (GetOwnerRole() == ROLE_AutonomousProxy)
		{
			MulticastOnServer_SendGameplayEvent(EventTag, Payload);
		}
		else
		{
			APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
			auto Char = PC ? Cast<ACharacter>(PC->GetPawn()) : nullptr;
			if (Char)
			{
				IAbilitySystemInterface* AsAbilitySysInterface = Cast<IAbilitySystemInterface>(Char);
				if (AsAbilitySysInterface)
				{
					if (UAbilitySystemComponent* GasComp = AsAbilitySysInterface->GetAbilitySystemComponent())
					{
						if (UAbilitySystemComponentBase* GasCompCasted = Cast<UAbilitySystemComponentBase>(GasComp))
						{
							GasCompCasted->MulticastOnServer_SendGameplayEvent_ToActor(GetAvatar(), EventTag, Payload);
						}
					}
				}
			}
		}
	}
}

void UAbilitySystemComponentBase::MulticastOnServer_SendGameplayEvent_ToActor_Implementation(AActor* Actor, FGameplayTag EventTag, FGameplayEventData Payload)
{
	if (IsValid(Actor))
	{
		IAbilitySystemInterface* AsAbilitySysInterface = Cast<IAbilitySystemInterface>(Actor);
		if (AsAbilitySysInterface)
		{
			if (UAbilitySystemComponent* GasComp = AsAbilitySysInterface->GetAbilitySystemComponent())
			{
				if (UAbilitySystemComponentBase* GasCompCasted = Cast<UAbilitySystemComponentBase>(GasComp))
				{
					GasCompCasted->MulticastSendGameplayEvent(EventTag, Payload);
				}
			}
		}
	}
}

void UAbilitySystemComponentBase::MulticastOnServer_SendGameplayEvent_Implementation(FGameplayTag EventTag, FGameplayEventData Payload)
{
	MulticastSendGameplayEvent(EventTag, Payload);
}


void UAbilitySystemComponentBase::MulticastSendGameplayEvent_Implementation(FGameplayTag EventTag, FGameplayEventData Payload)
{
	if (IgnoreLocalGameplayEventCount.Contains(EventTag))
	{
		if (IgnoreLocalGameplayEventCount[EventTag] <= 1)
			IgnoreLocalGameplayEventCount.Remove(EventTag);
		else
			IgnoreLocalGameplayEventCount.Add(EventTag, IgnoreLocalGameplayEventCount[EventTag] - 1);
		return;
	}
	HandleGameplayEvent(EventTag, &Payload);
}

void UAbilitySystemComponentBase::SendServerGameplayEvent_VariableContainer_Multicast(FGameplayTag EventTag, UVariableContainer* Payload)
{
	if (GetOwnerRole() == ROLE_Authority)
		MulticastSendGameplayEvent_VariableContainer(EventTag, FGAS_Utility_VariableContainerStruct(Payload));
	else
	{
		if (IgnoreLocalGameplayEventCount.Contains(EventTag))
			IgnoreLocalGameplayEventCount.Add(EventTag, IgnoreLocalGameplayEventCount[EventTag] + 1);
		else
			IgnoreLocalGameplayEventCount.Add(EventTag, 1);
		HandleGameplayEvent_VariableContainer(EventTag, FGAS_Utility_VariableContainerStruct(Payload));

		if (GetOwnerRole() == ROLE_AutonomousProxy)
			SendServerGameplayEvent_VC_Multicast(EventTag, FGAS_Utility_VariableContainerStruct(Payload));
		else
		{
			APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
			auto Char = PC ? Cast<ACharacter>(PC->GetPawn()) : nullptr;
			if (Char)
			{
				IAbilitySystemInterface* AsAbilitySysInterface = Cast<IAbilitySystemInterface>(Char);
				if (AsAbilitySysInterface)
				{
					if (UAbilitySystemComponent* GasComp = AsAbilitySysInterface->GetAbilitySystemComponent())
					{
						if (UAbilitySystemComponentBase* GasCompCasted = Cast<UAbilitySystemComponentBase>(GasComp))
						{
							GasCompCasted->SendServerGameplayEvent_VC_Multicast_OnActor(GetAvatar(), EventTag, Payload);
						}
					}
				}
			}
		}
	}
}


void UAbilitySystemComponentBase::SendServerGameplayEvent_VC_Multicast_Implementation(FGameplayTag EventTag, FGAS_Utility_VariableContainerStruct Payload)
{
	MulticastSendGameplayEvent_VariableContainer(EventTag, Payload);
}

void UAbilitySystemComponentBase::SendServerGameplayEvent_VC_Multicast_OnActor_Implementation(AActor* Actor, FGameplayTag EventTag, FGAS_Utility_VariableContainerStruct Payload)
{
	if (IsValid(Actor))
	{
		IAbilitySystemInterface* AsAbilitySysInterface = Cast<IAbilitySystemInterface>(Actor);
		if (AsAbilitySysInterface)
		{
			if (UAbilitySystemComponent* GasComp = AsAbilitySysInterface->GetAbilitySystemComponent())
			{
				if (UAbilitySystemComponentBase* GasCompCasted = Cast<UAbilitySystemComponentBase>(GasComp))
				{
					GasCompCasted->MulticastSendGameplayEvent_VariableContainer(EventTag, Payload);
				}
			}
		}
	}
}


void UAbilitySystemComponentBase::MulticastSendGameplayEvent_VariableContainer_Implementation(FGameplayTag EventTag, FGAS_Utility_VariableContainerStruct Payload)
{
	if (IgnoreLocalGameplayEventCount.Contains(EventTag))
	{
		if (IgnoreLocalGameplayEventCount[EventTag] <= 1)
			IgnoreLocalGameplayEventCount.Remove(EventTag);
		else
			IgnoreLocalGameplayEventCount.Add(EventTag, IgnoreLocalGameplayEventCount[EventTag] - 1);
		return;
	}
	HandleGameplayEvent_VariableContainer(EventTag, Payload);
}

void UAbilitySystemComponentBase::HandleGameplayEvent_VariableContainer(FGameplayTag EventTag, FGAS_Utility_VariableContainerStruct Payload)
{
	UVariableContainer* VariableContainer = NewObject<UVariableContainer>(this);
	VariableContainer->SetupFromArrays(Payload.VariablesNames, Payload.VariablesValues);
	FGameplayEventData PayloadStruct;
	PayloadStruct.OptionalObject = VariableContainer;
	HandleGameplayEvent(EventTag, &PayloadStruct);
}
