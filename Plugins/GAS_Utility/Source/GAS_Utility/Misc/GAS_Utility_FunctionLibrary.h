// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"
#include "UObject/NoExportTypes.h"
#include "GAS_Utility_FunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class GAS_UTILITY_API UGAS_Utility_FunctionLibrary : public UObject
{
	GENERATED_BODY()
	
	// ============================ Navigation ============================
	
	UFUNCTION(BlueprintCallable, BlueprintPure,Category="AI|Navigation")
	static EPathFollowingStatus::Type GetNavPathFollowingStatus(UPathFollowingComponent* PathFollowComp);
	
	UFUNCTION(BlueprintCallable,Category = "AI|Navigation")
	static void AbortMove_NavPathFollowing(UPathFollowingComponent* PathFollowComp, UObject* Instigator);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "AI|Navigation", meta = (WorldContext = "WorldContextObject"))
	static ANavigationData* GetNavDataFor(UObject* WorldContextObject, ACharacter* CharRef);

	
	// ============================ Utility ============================

	UFUNCTION(BlueprintCallable,Category="Actor|Component")
	static void ForceDestroyActorComponent(UActorComponent* Component);

	
	UFUNCTION(BlueprintCallable,BlueprintPure,Category="GameplayTag")
	static FString GetLastTagFromGameplayTag(FGameplayTag Tag);

	// ============================ Animation ============================
	UFUNCTION(BlueprintCallable,BlueprintPure,Category="AnimSequence")
	static float GetAnimationRateScale(UAnimSequenceBase* Anim);

	UFUNCTION(BlueprintPure, Category = "GameplayTags")
	static bool TryRequestGameplayTag(FName TagName, FGameplayTag& Tag);

};

