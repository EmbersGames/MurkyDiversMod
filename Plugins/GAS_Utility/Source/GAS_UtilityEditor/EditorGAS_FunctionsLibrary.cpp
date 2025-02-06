// Embers 2024


#include "EditorGAS_FunctionsLibrary.h"

#include "AssetToolsModule.h"
#include "GameplayTagsEditorModule.h"
#include "GameplayTagsManager.h"
#include "Components/TextWidgetTypes.h"
#include "GameplayTagsSettings.h"
#include "GAS_UtilityEditor.h"
#include "Factories/BlueprintFactory.h"
#include "Factories/ObjectLibraryFactory.h"
#include "Windows/WindowsPlatformApplicationMisc.h"
#include "GAS_Utility/AbilitySystem/GameplayAbilityBase.h"



TArray<FString> UEditorGAS_FunctionsLibrary::SplitStringByCapsAndDash(const FString& InputString)
{
	TArray<FString> StringArray;
	TArray<TCHAR> SingleCharList = {'A', 'R', 'L'};
	FString CurrentWord = TEXT("");
	for (int32 i = 0; i < InputString.Len(); i++)
	{
		if (TChar<TCHAR>::IsUpper(InputString[i]) || InputString[i] == '-' || InputString[i] == '_')
		{
			if ((i > 0 && CurrentWord.Len() > 0 && (FChar::IsDigit(InputString[i - 1]) || TChar<TCHAR>::IsLower(InputString[i - 1]))) || (i > 0 && CurrentWord.Len() > 0 && (InputString[i] == '-' || InputString[i] == '_')))
			{
				StringArray.Add(CurrentWord);
				CurrentWord.Empty();
			}
		}

		if (CurrentWord.Len() == 2 && TChar<TCHAR>::IsLower(InputString[i]) && TChar<TCHAR>::IsUpper(CurrentWord[1]))
		{
			for (const TCHAR SingleChar : SingleCharList)
			{
				if (CurrentWord[0] == SingleChar)
				{
					StringArray.Add(FString::Printf(TEXT("%c"), SingleChar));
					CurrentWord.RemoveAt(0);
					break;
				}
			}
		}
		if (InputString[i] != '-' && InputString[i] != '_')
			CurrentWord.AppendChar(InputString[i]);
	}
	if (CurrentWord.Len() > 0)
	{
		StringArray.Add(CurrentWord);
	}
	return StringArray;
}

FString UEditorGAS_FunctionsLibrary::StringRemoveNumbers(const FString& InputString)
{
	FString ResultString = TEXT("");

	// Parcourez la chaîne d'entrée
	for (int32 i = 0; i < InputString.Len(); i++)
	{
		// Vérifiez si le caractère actuel est un chiffre
		if (!FChar::IsDigit(InputString[i]))
		{
			// Ajoutez le caractère actuel à la chaîne de résultat
			ResultString.AppendChar(InputString[i]);
		}
	}
	return ResultString;
}


FGameplayTagContainer UEditorGAS_FunctionsLibrary::RequestGameplayTagChildren(const FGameplayTag& GameplayTag)
{
	FGameplayTagContainer Result;
	UGameplayTagsManager& TagManager = UGameplayTagsManager::Get();
	TSharedPtr<FGameplayTagNode> Node = TagManager.FindTagNode(GameplayTag);
	TArray< TSharedPtr<FGameplayTagNode> >& ChildrenNodes = Node->GetChildTagNodes();
	for (TSharedPtr<FGameplayTagNode> ChildNode : ChildrenNodes)
	{
		if (ChildNode.IsValid())
		{
			Result.AddTag(ChildNode->GetCompleteTag());
		}
	}
	return Result;
}
