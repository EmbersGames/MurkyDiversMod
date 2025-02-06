// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagWizard.h"
#include "Modules/ModuleManager.h"


DECLARE_LOG_CATEGORY_EXTERN(LogGAS_UtilityEditorOnly, Log, All)
class FGAS_UtilityEditorOnlyModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	void OnPostEngineInit();
	
	static inline FGAS_UtilityEditorOnlyModule& Get()
	{
		return FModuleManager::LoadModuleChecked<FGAS_UtilityEditorOnlyModule>("GAS_UtilityEditorOnly");
	}
	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("GAS_UtilityEditorOnly");
	}
	
	TArray<FString> EnglishWords;
	bool EnglishWordsListLoaded;
	
	void LoadEnglishWordsList();
	
	void ForceReloadEnglishWordsList();
	
	bool IsValidEnglishWord(FString Word);

	TArray<FName> DefaultAbilityNodes;
	TMap<UClass*,TArray<FName> > DefaultNodeToSpawnByClass;

	void AddDefaultNodeForClass(UClass* Class,TArray<FName> Nodes);
};
