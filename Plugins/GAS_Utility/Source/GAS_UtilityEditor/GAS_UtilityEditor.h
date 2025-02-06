// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"


DECLARE_LOG_CATEGORY_EXTERN(LogGAS_UtilityEditor, Log, All)
class FGAS_UtilityEditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	void OnPostEngineInit();
	
	static inline FGAS_UtilityEditorModule& Get()
	{
		return FModuleManager::LoadModuleChecked<FGAS_UtilityEditorModule>("GAS_UtilityEditor");
	}
	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("GAS_UtilityEditor");
	}
	
	

};
