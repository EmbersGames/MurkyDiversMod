// Fill out your copyright notice in the Description page of Project Settings.

#include "VariableContainer/VariableContainerSubsystem.h"

#include "GAS_Utility.h"

#include "Subsystems/SubsystemBlueprintLibrary.h"

#define GAS_Utility_FUNC (FString(__FUNCTION__))              // Current Class Name + Function Name where this is called
#define GAS_Utility_LINE (FString::FromInt(__LINE__))         // Current Line Number in the code where this is called
#define GAS_Utility_FUNC_LINE (GAS_Utility_FUNC + "(" + GAS_Utility_LINE + ")") // Current Class and Line Number where this is called!


UVariableContainerSubsystem::UVariableContainerSubsystem()
	: UEngineSubsystem()
{
}

void UVariableContainerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UE_LOG(LogGAS_Utility, Log, TEXT("%s: VariableContainer subsystem initialized"), *GAS_Utility_FUNC_LINE);
}

void UVariableContainerSubsystem::Deinitialize()
{
	// Do nothing for now
	Super::Deinitialize();
}

bool UVariableContainerSubsystem::IsNameStableForNetworking() const
{
	return true;
}

bool UVariableContainerSubsystem::IsSupportedForNetworking() const
{
	return true;
}

UVariableContainer* UVariableContainerSubsystem::ConstructVariableContainer()
{
	return NewObject<UVariableContainer>(this);
}

UVariableContainer* UVariableContainerSubsystem::StaticConstructVariableContainer()
{
	auto SelfSystem = CastChecked<UVariableContainerSubsystem>(USubsystemBlueprintLibrary::GetEngineSubsystem(UVariableContainerSubsystem::StaticClass()), ECastCheckedType::NullChecked);
	return SelfSystem->ConstructVariableContainer();
}
