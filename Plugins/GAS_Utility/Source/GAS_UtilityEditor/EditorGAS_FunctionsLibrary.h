// Embers 2024

#pragma once

#include "CoreMinimal.h"
#include "ClassViewerFilter.h"
#include "EditorUtilityWidgetComponents.h"
#include "GameplayTagContainer.h"
#include "UObject/NoExportTypes.h"
#include "EditorUtilityWidget.h"

#include "EditorGAS_FunctionsLibrary.generated.h"

/**
 * 
 */



UCLASS()
class GAS_UTILITYEDITOR_API UEditorGAS_FunctionsLibrary : public UObject
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, Category=EditorGAS_FunctionsLibrary)
	static TArray<FString> SplitStringByCapsAndDash(const FString& InputString);

	UFUNCTION(BlueprintCallable, Category=EditorGAS_FunctionsLibrary)
	static FString StringRemoveNumbers(const FString& InputString);

	UFUNCTION(BlueprintCallable, Category=EditorGAS_FunctionsLibrary)
	static FGameplayTagContainer RequestGameplayTagChildren(const FGameplayTag& GameplayTag);


};
