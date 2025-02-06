// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Subsystems/EngineSubsystem.h"
#include "VariableContainer/VariableContainer.h"
#include "VariableContainerSubsystem.generated.h"
/**
 * 
 */
UCLASS()
class GAS_UTILITY_API UVariableContainerSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()
public:
	UVariableContainerSubsystem();

	
	// Begin USubsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	// End USubsystem

	
	bool IsNameStableForNetworking() const override;
	bool IsSupportedForNetworking() const override;
	/** Create new Json object */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Construct Variable Container"), Category = "GAS_Utility|VariableContainer")
	UVariableContainer* ConstructVariableContainer();

	
	/** Create new Json object (static one for MakeJson node, hack for #293) */
	UFUNCTION()
	static UVariableContainer* StaticConstructVariableContainer();

};
