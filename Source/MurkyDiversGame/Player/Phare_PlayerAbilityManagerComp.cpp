// Fill out your copyright notice in the Description page of Project Settings.


#include "Phare_PlayerAbilityManagerComp.h"

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GAS_Utility/AbilitySystem/AbilityInputBindingComponent.h"
#include "GAS_Utility/AbilitySystem/AbilitySystemComponentBase.h"
#include "Components/PawnComponent.h"
#include "GameFramework/Character.h"

UPhare_PlayerAbilityManagerComp::UPhare_PlayerAbilityManagerComp(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	bAutoActivate = true;
}

ACharacter* UPhare_PlayerAbilityManagerComp::GetPhareCharacter()
{
	if (PhareCharacterOwner)
		return PhareCharacterOwner;
	auto PawnOwner = GetPawn<APawn>();
	if (PawnOwner)
	{
		if (auto PharePlayer = Cast<ACharacter>(PawnOwner))
		{
			PhareCharacterOwner = PharePlayer;
			return PharePlayer;
		}
	}
	return nullptr;
}

APlayerController* UPhare_PlayerAbilityManagerComp::GetPlayerController()
{
	if (!PhareCharacterOwner)
	{
		PhareCharacterOwner = GetPhareCharacter();
	}
	if (PhareCharacterOwner)
	{
		if (AController* Controller = PhareCharacterOwner->GetController())
		{
			return Cast<APlayerController>(Controller);
		}
	}
	return nullptr;
}



UAbilitySystemComponentBase* UPhare_PlayerAbilityManagerComp::GetOwnerAbilitySystemComponent()
{
	const auto Owner = GetPhareCharacter();
	if (const auto OwnerInterface = Cast<IAbilitySystemInterface>(Owner))
		return Cast<UAbilitySystemComponentBase>(OwnerInterface->GetAbilitySystemComponent());
	return nullptr;
}
