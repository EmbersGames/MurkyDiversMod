// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Containers/Array.h"
#include "Containers/Map.h"
#include "GameplayAbilitySpec.h" // for FGameplayAbilitySpecHandle
#include "HAL/Platform.h"
#include "HAL/PlatformCrt.h"
#include "../Input/PlayerControlsComponent.h"
#include "UObject/UObjectGlobals.h"

#include "AbilityInputBindingComponent.generated.h"

class AController;
class UAbilitySystemComponent;
class UEnhancedInputComponent;
class UInputAction;
class UObject;
struct FFrame;

USTRUCT()
struct FAbilityInputBinding
{
	GENERATED_BODY()

	int32  InputID = 0;
	uint32 OnPressedHandle = 0;
	uint32 OnReleasedHandle = 0;
	TArray<FGameplayAbilitySpecHandle> BoundAbilitiesStack;
};

/** Component that hooks up enhanced input to the ability system input logic */
UCLASS(meta=(BlueprintSpawnableComponent))
class GAS_UTILITY_API UAbilityInputBindingComponent : public UPlayerControlsComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "AncientGame|Abilities")
	void SetInputBinding(UInputAction* InputAction, FGameplayAbilitySpecHandle AbilityHandle);

	UFUNCTION(BlueprintCallable, Category = "AncientGame|Abilities")
	void ClearInputBinding(FGameplayAbilitySpecHandle AbilityHandle);

	UFUNCTION(BlueprintCallable, Category = "AncientGame|Abilities")
	void ClearAbilityBindings(UInputAction* InputAction);

	//~ Begin UPlayerControlsComponent interface
	virtual void SetupPlayerControls_Implementation(UEnhancedInputComponent* PlayerInputComponent) override;
	virtual void ReleaseInputComponent(AController* OldController) override;
	//~ End UPlayerControlsComponent interface

private:
	void ResetBindings();
	void RunAbilitySystemSetup();
	void OnAbilityInputPressed(const FInputActionInstance& Instance);
	void OnAbilityInputReleased(const FInputActionInstance& Instance);

	void RemoveEntry(UInputAction* InputAction);

	FGameplayAbilitySpec* FindAbilitySpec(FGameplayAbilitySpecHandle Handle);
	void TryBindAbilityInput(UInputAction* InputAction, FAbilityInputBinding& AbilityInputBinding);

private:
	UPROPERTY(transient)
	UAbilitySystemComponent* AbilityComponent;

	UPROPERTY(transient)
	TMap<UInputAction*, FAbilityInputBinding> MappedAbilities;
};
