// Copyright Epic Games, Inc. All Rights Reserved.

#include "GAS_Utility.h"

#define LOCTEXT_NAMESPACE "FGAS_UtilityModule"



void FGAS_UtilityModule::StartupModule()
{


	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FGAS_UtilityModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FGAS_UtilityModule, GAS_Utility)


DEFINE_LOG_CATEGORY(LogGAS_Utility)

#define GAS_Utility_FUNC (FString(__FUNCTION__))              // Current Class Name + Function Name where this is called
#define GAS_Utility_LINE (FString::FromInt(__LINE__))         // Current Line Number in the code where this is called
#define GAS_Utility_FUNC_LINE (GAS_Utility_FUNC + "(" + GAS_Utility_LINE + ")") // Current Class and Line Number where this is called!
