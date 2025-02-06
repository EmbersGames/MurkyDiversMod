// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/GAS_Utility_FunctionLibrary.h"

#include "GameplayTagContainer.h"
#include "GameplayTagsManager.h"
#include "GameFramework/Character.h"

EPathFollowingStatus::Type UGAS_Utility_FunctionLibrary::GetNavPathFollowingStatus(UPathFollowingComponent* PathFollowComp)
{
	if (IsValid(PathFollowComp))
	{
		return PathFollowComp->GetStatus();
	}
	return EPathFollowingStatus::Idle;
}

void UGAS_Utility_FunctionLibrary::AbortMove_NavPathFollowing(UPathFollowingComponent* PathFollowComp, UObject* Instigator)
{
	if (IsValid(PathFollowComp))
	{
		PathFollowComp->AbortMove(*Instigator, FPathFollowingResultFlags::UserAbort);
	}
}

ANavigationData* UGAS_Utility_FunctionLibrary::GetNavDataFor(UObject* WorldContextObject, ACharacter* CharRef)
{
	if (IsValid(CharRef))
	{
		UNavigationSystemV1* const NavSys = UNavigationSystemV1::GetNavigationSystem(WorldContextObject);
		if (IsValid(NavSys) && IsValid(CharRef))
			return NavSys->GetNavDataForProps(CharRef->GetNavAgentPropertiesRef());
	}
	return nullptr;
}

void UGAS_Utility_FunctionLibrary::ForceDestroyActorComponent(UActorComponent* Component)
{
	if (IsValid(Component))
	{
		Component->DestroyComponent();
	}
}

FString UGAS_Utility_FunctionLibrary::GetLastTagFromGameplayTag(FGameplayTag Tag)
{
	FString TagName = Tag.GetTagName().ToString();
	if (TagName.IsEmpty())
	{
		return FString();
	}

	TArray<FString> TagWords;
	TagName.ParseIntoArray(TagWords, TEXT("."), true);

	if (TagWords.Num() == 0)
	{
		return TagName;
	}

	return TagWords.Last();
}

float UGAS_Utility_FunctionLibrary::GetAnimationRateScale(UAnimSequenceBase* Anim)
{
	if (Anim)
		return Anim->RateScale;
	return 1.f;
}

bool UGAS_Utility_FunctionLibrary::TryRequestGameplayTag(FName TagName, FGameplayTag& Tag)
{
	Tag = UGameplayTagsManager::Get().RequestGameplayTag(TagName, false);
	return Tag.IsValid();
}

