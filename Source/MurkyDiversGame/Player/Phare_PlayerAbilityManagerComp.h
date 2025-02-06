// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"
#include "Phare_PlayerAbilityManagerComp.generated.h"


class UAbilityInputBindingComponent;
/**
 * 
 */
UCLASS()
class MURKYDIVERSGAME_API UPhare_PlayerAbilityManagerComp : public UPawnComponent
{
	GENERATED_BODY()

public:
	UPhare_PlayerAbilityManagerComp(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="CharacterOwner")
	ACharacter* GetPhareCharacter();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="CharacterOwner")
	APlayerController* GetPlayerController();

	UPROPERTY()
	ACharacter* PhareCharacterOwner;

	

	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ability",DisplayName="GetGAS_Comp")
	UAbilitySystemComponentBase* GetOwnerAbilitySystemComponent();
};
