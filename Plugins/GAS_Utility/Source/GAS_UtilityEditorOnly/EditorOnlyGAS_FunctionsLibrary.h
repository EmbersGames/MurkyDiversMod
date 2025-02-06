// Embers 2024

#pragma once

#include "CoreMinimal.h"
#include "ClassViewerFilter.h"
#include "EditorUtilityWidgetComponents.h"
#include "GameplayTagContainer.h"
#include "UObject/NoExportTypes.h"
#include "EditorUtilityWidget.h"

#include "EditorOnlyGAS_FunctionsLibrary.generated.h"

/**
 * 
 */

UINTERFACE()
class UTabEditorUtilityWidget : public UInterface
{
	GENERATED_BODY()
};

class GAS_UTILITYEDITORONLY_API ITabEditorUtilityWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "User Widget")
	void SetTabID(FName TabId);
};



UCLASS()
class GAS_UTILITYEDITORONLY_API UEditorGASOnly_FunctionsLibrary : public UObject
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, Category=EditorGAS_FunctionsLibrary)
	static bool UpdateGameplayTagInINI(const FGameplayTag Tag, FString DevComment);

	UFUNCTION(BlueprintCallable, Category=EditorGAS_FunctionsLibrary)
	static FString GetGameplayTagComment(const FGameplayTag Tag);
	
	UFUNCTION(BlueprintCallable, Category=EditorGAS_FunctionsLibrary)
	static bool AddGameplayTagInINI(FName Tag, FString DevComment, const FGameplayTag SourceFileFromTag);

	UFUNCTION(BlueprintCallable, Category=EditorGAS_FunctionsLibrary)
	static bool IsValidGameplayTagString(FString Tag, FString& FixedTag, FText& ErrorMsg);


	UFUNCTION(BlueprintCallable, Category=EditorGAS_FunctionsLibrary)
	static void ForceReloadEnglishWordsList();

	UFUNCTION(BlueprintCallable, Category=EditorGAS_FunctionsLibrary)
	static bool IsValidEnglishWord(FString Word);

	UFUNCTION(BlueprintCallable, Category=EditorGAS_FunctionsLibrary)
	static FString StringRemoveNumbers(const FString& InputString);

	UFUNCTION(BlueprintCallable, Category=EditorGAS_FunctionsLibrary)
	static TArray<FString> GetDefaultsGameplayTags();

	UFUNCTION(BlueprintCallable, Category=EditorGAS_FunctionsLibrary)
	static TArray<FString> GetLinesFromConfigFile(FString ConfigFileName);

	UFUNCTION(BlueprintCallable, Category=EditorGAS_FunctionsLibrary)
	static TArray<FString> GetAllWordInGameplayTagString(const FString InputString);

	UFUNCTION(BlueprintCallable, Category=EditorGAS_FunctionsLibrary)
	static TArray<FString> SplitStringByCapsAndDash(const FString& InputString);

	UFUNCTION(BlueprintCallable, Category=EditorGAS_FunctionsLibrary)
	static void CopyTextToClipboard(const FString& Text);

	UFUNCTION(BlueprintCallable, Category=EditorGAS_FunctionsLibrary)
	static bool ContainLineBreak(const FString& Text);

	UFUNCTION(BlueprintCallable, Category=EditorGAS_FunctionsLibrary)
	static void AddNewEnglishWord(FString NewWord);
	
	UFUNCTION(BlueprintCallable, Category = "Editor Scripting | Asset Tools")
	static UObject* CreateBlueprintAsset(const FString& AssetName, const FString& PackagePath, UClass* AssetClass);

	UFUNCTION(BlueprintCallable, Category = "Editor Scripting | Asset Tools")
	static UObject* CreateAbilityBlueprintAsset(const FString& AssetName, const FString& PackagePath, UClass* AssetClass);
	
	UFUNCTION(BlueprintCallable, Category = "Editor Scripting | Asset Tools")
	static void AddAbilityDefaultNodeForClass(UClass* Class);

	UFUNCTION(BlueprintCallable, Category=EditorGAS_FunctionsLibrary)
	static FGameplayTagContainer RequestGameplayTagChildren(const FGameplayTag& GameplayTag);


};

UCLASS()
class GAS_UTILITYEDITORONLY_API UGASEditorUtilityMultiLineEditableTextBox : public UEditorUtilityMultiLineEditableTextBox
{
	GENERATED_BODY()

	UGASEditorUtilityMultiLineEditableTextBox(const FObjectInitializer& ObjectInitializer);
	UFUNCTION(BlueprintCallable, Category = "UGASEditorUtilityMultiLineEditableTextBox")
	void SetWrapTextWidth(float InWrapTextAt);
};
