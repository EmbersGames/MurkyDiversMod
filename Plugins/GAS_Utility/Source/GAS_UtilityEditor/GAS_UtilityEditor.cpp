// Copyright Epic Games, Inc. All Rights Reserved.

#include "GAS_UtilityEditor.h"


#define LOCTEXT_NAMESPACE "FGAS_UtilityEditorModule"

void FGAS_UtilityEditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module


	FCoreDelegates::OnPostEngineInit.AddRaw(this, &FGAS_UtilityEditorModule::OnPostEngineInit);
}

void FGAS_UtilityEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

void FGAS_UtilityEditorModule::OnPostEngineInit()
{
}


#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FGAS_UtilityEditorModule, GAS_UtilityEditor)


DEFINE_LOG_CATEGORY(LogGAS_UtilityEditor)
