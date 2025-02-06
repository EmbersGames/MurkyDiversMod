// Embers 2024


#include "EditorOnlyGAS_FunctionsLibrary.h"

#include "AssetToolsModule.h"
#include "GameplayTagsEditorModule.h"
#include "GameplayTagsManager.h"
#include "Components/TextWidgetTypes.h"
#include "GameplayTagsSettings.h"
#include "GAS_UtilityEditorOnly.h"
#include "Factories/BlueprintFactory.h"
#include "Factories/ObjectLibraryFactory.h"
#include "Windows/WindowsPlatformApplicationMisc.h"
#include "GAS_Utility/AbilitySystem/GameplayAbilityBase.h"



bool UEditorGASOnly_FunctionsLibrary::UpdateGameplayTagInINI(const FGameplayTag Tag, FString DevComment)
{
	IGameplayTagsEditorModule& TagsEditor = IGameplayTagsEditorModule::Get();
	UGameplayTagsManager& Manager = UGameplayTagsManager::Get();

	FString OldComment;
	TArray<FName> TagSourceNames;
	bool bTagIsExplicit;
	bool bTagWasRestricted;
	bool bTagDidAllowNonRestrictedChildren;
	bool TagFound = false;
	if (Manager.GetTagEditorData(Tag.GetTagName(), OldComment, TagSourceNames, bTagIsExplicit, bTagWasRestricted, bTagDidAllowNonRestrictedChildren))
	{
		for (auto TagSourceName : TagSourceNames)
		{
			const FGameplayTagSource* TagSource = Manager.FindTagSource(TagSourceName);
			if (TagSource && TagSource->SourceTagList)
			{
				if (TagSource->SourceType != EGameplayTagSourceType::Native && TagSource->SourceType != EGameplayTagSourceType::DataTable && TagSource->SourceType != EGameplayTagSourceType::Invalid)
				{
					const FString& ConfigFileName = TagSource->SourceTagList->ConfigFileName;

					TArray<FGameplayTagTableRow>& List = TagSource->SourceTagList->GameplayTagList;
					for (int i = 0; i < List.Num(); i++)
					{
						if (List[i].Tag == Tag.GetTagName())
						{
							List[i].DevComment = DevComment;
							TagSource->SourceTagList->TryUpdateDefaultConfigFile(ConfigFileName);
							TagFound = true;
							Manager.EditorRefreshGameplayTagTree();
							break;
						}
					}
				}
			}
		}
	}
	if (!TagFound)
		TagFound = TagsEditor.UpdateTagInINI(Tag.ToString(), DevComment, true, true);
	return TagFound;
}

FString UEditorGASOnly_FunctionsLibrary::GetGameplayTagComment(const FGameplayTag Tag)
{
	IGameplayTagsEditorModule& TagsEditor = IGameplayTagsEditorModule::Get();
	UGameplayTagsManager& Manager = UGameplayTagsManager::Get();

	FString OldComment;
	TArray<FName> TagSourceNames;
	bool bTagIsExplicit;
	bool bTagWasRestricted;
	bool bTagDidAllowNonRestrictedChildren;
	bool TagFound = false;
	if (Manager.GetTagEditorData(Tag.GetTagName(), OldComment, TagSourceNames, bTagIsExplicit, bTagWasRestricted, bTagDidAllowNonRestrictedChildren))
	{
		for (const FName TagSourceName : TagSourceNames)
		{
			const FGameplayTagSource* TagSource = Manager.FindTagSource(TagSourceName);
			if (TagSource && TagSource->SourceTagList)
			{
				if (TagSource->SourceType != EGameplayTagSourceType::Native && TagSource->SourceType != EGameplayTagSourceType::DataTable && TagSource->SourceType != EGameplayTagSourceType::Invalid)
				{
					TArray<FGameplayTagTableRow>& List = TagSource->SourceTagList->GameplayTagList;
					for (int i = 0; i < List.Num(); i++)
					{
						if (List[i].Tag == Tag.GetTagName())
						{
							return List[i].DevComment;
						}
					}
				}
			}
		}
		for (const FName TagSourceName : TagSourceNames)
		{
			const FGameplayTagSource* TagSource = Manager.FindTagSource(TagSourceName);
			if (TagSource && TagSource->SourceTagList)
			{
				TArray<FGameplayTagTableRow>& List = TagSource->SourceTagList->GameplayTagList;
				for (int i = 0; i < List.Num(); i++)
				{
					if (List[i].Tag == Tag.GetTagName())
					{
						return List[i].DevComment;
					}
				}
			}
		}
	}
	return "";
}

bool UEditorGASOnly_FunctionsLibrary::AddGameplayTagInINI(FName Tag, FString DevComment, const FGameplayTag SourceFileFromTag)
{
	if (Tag.ToString().IsEmpty())
	{
		return false;
	}
	UGameplayTagsManager& Manager = UGameplayTagsManager::Get();

	FText ErrorText;
	FString FixedString;
	if (!Manager.IsValidGameplayTagString(Tag.ToString(), &ErrorText, &FixedString))
	{
		UE_LOG(LogTemp, Warning, TEXT("AddTagFailure_BadString Failed to add gameplay tag %s: %s, try %s instead!"), *Tag.ToString(), *ErrorText.ToString(), *FixedString);
		return false;
	}
	FString OldComment;
	TArray<FName> FoundTagSourceNames;
	TArray<FName> TagSourceNames;
	bool bTagIsExplicit;
	bool bTagWasRestricted;
	bool bTagDidAllowNonRestrictedChildren;
	bool TagFound = false;
	if (bool AlreadyExist = Manager.GetTagEditorData(Tag, OldComment, FoundTagSourceNames, bTagIsExplicit, bTagWasRestricted, bTagDidAllowNonRestrictedChildren))
	{
		for (const FName FoundTagSourceName : FoundTagSourceNames)
		{
			const FGameplayTagSource* TagSource = Manager.FindTagSource(FoundTagSourceName);
			if (TagSource->SourceType != EGameplayTagSourceType::Native && TagSource->SourceType != EGameplayTagSourceType::DataTable && TagSource->SourceType != EGameplayTagSourceType::Invalid)
			{
				UE_LOG(LogTemp, Warning, TEXT("Tag %s Already exist in %s "), *Tag.ToString(), *FoundTagSourceName.ToString())
				return false;
			}
		}
	}
	if (SourceFileFromTag.IsValid())
		Manager.GetTagEditorData(SourceFileFromTag.GetTagName(), OldComment, TagSourceNames, bTagIsExplicit, bTagWasRestricted, bTagDidAllowNonRestrictedChildren);
	else
		TagSourceNames.Add(FGameplayTagSource::GetDefaultName());
	if (!TagSourceNames.IsEmpty())
	{
		for (auto TagSourceName : TagSourceNames)
		{
			const FGameplayTagSource* TagSource = Manager.FindTagSource(TagSourceName);
			if (TagSource->SourceType != EGameplayTagSourceType::Native && TagSource->SourceType != EGameplayTagSourceType::DataTable && TagSource->SourceType != EGameplayTagSourceType::Invalid)
			{
				if (TagSource && TagSource->SourceTagList)
				{
					const FString& ConfigFileName = TagSource->SourceTagList->ConfigFileName;
					TArray<FGameplayTagTableRow>& List = TagSource->SourceTagList->GameplayTagList;
					List.Add(FGameplayTagTableRow(Tag, DevComment));
					TagSource->SourceTagList->SortTags();
					TagSource->SourceTagList->TryUpdateDefaultConfigFile(ConfigFileName);
					TagFound = true;
					Manager.EditorRefreshGameplayTagTree();
					return true;
				}
			}
		}
	}

	return TagFound;
}

bool UEditorGASOnly_FunctionsLibrary::IsValidGameplayTagString(FString Tag, FString& FixedTag, FText& ErrorMsg)
{
	UGameplayTagsManager& Manager = UGameplayTagsManager::Get();
	return Manager.IsValidGameplayTagString(Tag, &ErrorMsg, &FixedTag);
}

void UEditorGASOnly_FunctionsLibrary::ForceReloadEnglishWordsList()
{
	FGAS_UtilityEditorOnlyModule& GASUtilityEditorModule = FGAS_UtilityEditorOnlyModule::Get();
	GASUtilityEditorModule.ForceReloadEnglishWordsList();
}

bool UEditorGASOnly_FunctionsLibrary::IsValidEnglishWord(FString Word)
{
	if (FGAS_UtilityEditorOnlyModule::IsAvailable())
	{
		FGAS_UtilityEditorOnlyModule& GASUtilityEditorModule = FGAS_UtilityEditorOnlyModule::Get();
		return GASUtilityEditorModule.IsValidEnglishWord(Word);
	}
	return false;
}

FString UEditorGASOnly_FunctionsLibrary::StringRemoveNumbers(const FString& InputString)
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

TArray<FString> UEditorGASOnly_FunctionsLibrary::GetDefaultsGameplayTags()
{
	TArray<FString> Result;
	FName IniFile = FGameplayTagSource::GetDefaultName();
	UGameplayTagsManager& Manager = UGameplayTagsManager::Get();

	const FGameplayTagSource* TagSource = Manager.FindTagSource(IniFile);

	if (TagSource && TagSource->SourceTagList)
	{
		TArray<FGameplayTagTableRow>& List = TagSource->SourceTagList->GameplayTagList;
		for (auto Element : List)
		{
			Result.Add(Element.Tag.ToString());
		}
	}
	return Result;
}

TArray<FString> UEditorGASOnly_FunctionsLibrary::GetLinesFromConfigFile(FString ConfigFileName)
{
	TArray<FString> Result;
	FString ProjectDir = FPaths::ProjectDir();
	FString ConfigDir = FPaths::Combine(*ProjectDir, TEXT("Config"));
	FString FilePath = FPaths::Combine(*ConfigDir, ConfigFileName);

	if (FPaths::FileExists(FilePath))
	{
		FString OutText;
		if (FFileHelper::LoadFileToString(OutText, *FilePath))
		{
			OutText.ParseIntoArrayLines(Result);
		}
	}
	return Result;
}

TArray<FString> UEditorGASOnly_FunctionsLibrary::GetAllWordInGameplayTagString(const FString InputString)
{
	TArray<FString> Result;
	TArray<FString> SubTags;

	InputString.ParseIntoArray(SubTags, TEXT("."));
	for (auto SubTag : SubTags)
	{
		auto SubTagArray = SplitStringByCapsAndDash(SubTag);
		for (auto SubTagWord : SubTagArray)
		{
			Result.Add(StringRemoveNumbers(SubTagWord));
		}
	}
	return Result;
}

TArray<FString> UEditorGASOnly_FunctionsLibrary::SplitStringByCapsAndDash(const FString& InputString)
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
void UEditorGASOnly_FunctionsLibrary::CopyTextToClipboard(const FString& Text)
{
	FPlatformApplicationMisc::ClipboardCopy(*Text);
}

bool UEditorGASOnly_FunctionsLibrary::ContainLineBreak(const FString& Text)
{
	const TCHAR* LineBreakCharacters[] = {TEXT("\r\n"), TEXT("\n")};
	for (const TCHAR* LineBreakCharacter : LineBreakCharacters)
	{
		if (Text.Contains(LineBreakCharacter))
		{
			return true;
		}
	}
	return false;
}


UGASEditorUtilityMultiLineEditableTextBox::UGASEditorUtilityMultiLineEditableTextBox(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
}

void UGASEditorUtilityMultiLineEditableTextBox::SetWrapTextWidth(float InWrapTextAt)
{
	WrapTextAt = InWrapTextAt;
	SynchronizeProperties();
}

void UEditorGASOnly_FunctionsLibrary::AddNewEnglishWord(FString NewWord)
{
	if (NewWord != "")

		if (FGAS_UtilityEditorOnlyModule::IsAvailable())
		{
			FGAS_UtilityEditorOnlyModule& GASUtilityEditorModule = FGAS_UtilityEditorOnlyModule::Get();
			if (!GASUtilityEditorModule.IsValidEnglishWord(NewWord))
			{
				GASUtilityEditorModule.EnglishWords.Add(NewWord);

				FString ProjectDir = FPaths::ProjectDir();
				FString ConfigDir = FPaths::Combine(*ProjectDir, TEXT("Config"));
				FString FilePath = FPaths::Combine(*ConfigDir, TEXT("Dictionary.txt"));

				FString NewLine = TEXT("\n") + NewWord;
				FFileHelper::SaveStringToFile(NewLine, *FilePath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), FILEWRITE_Append);
			}
		}
}

UObject* UEditorGASOnly_FunctionsLibrary::CreateBlueprintAsset(const FString& AssetName, const FString& PackagePath, UClass* AssetClass)
{
		IAssetTools& AssetTools = FAssetToolsModule::GetModule().Get();
	UBlueprintFactory* ActorFactory = NewObject<UBlueprintFactory>();
	ActorFactory->AddToRoot();
	ActorFactory->SupportedClass = UBlueprint::StaticClass();
	ActorFactory->ParentClass = AssetClass;
	ActorFactory->bSkipClassPicker = true;
	
	
	if (UBlueprint* NewBlueprint = Cast<UBlueprint>(AssetTools.CreateAsset(AssetName, PackagePath, UBlueprint::StaticClass(), ActorFactory, FName("GasUtilityEditor"))))
	{
		/*UClass* GeneratedClass = NewBlueprint->GeneratedClass;
		UGameplayAbilityBase* AbilityBase =  Cast<UGameplayAbilityBase>(GeneratedClass->ClassDefaultObject);
		if (AbilityBase)
		{
			FGameplayTagContainer NewDefaultValue = FGameplayTagContainer(TAG_System);
			AbilityBase->AbilityTags = NewDefaultValue;
		}*/
		return NewBlueprint;
	}
	
	return nullptr;
}

UObject* UEditorGASOnly_FunctionsLibrary::CreateAbilityBlueprintAsset(const FString& AssetName, const FString& PackagePath, UClass* AssetClass)
{
	AddAbilityDefaultNodeForClass(AssetClass);
	return CreateBlueprintAsset(AssetName,PackagePath,AssetClass);
}

void UEditorGASOnly_FunctionsLibrary::AddAbilityDefaultNodeForClass(UClass* Class)
{
	if (FGAS_UtilityEditorOnlyModule::IsAvailable())
	{
		FGAS_UtilityEditorOnlyModule& GASUtilityEditorModule = FGAS_UtilityEditorOnlyModule::Get();
		GASUtilityEditorModule.AddDefaultNodeForClass(Class,GASUtilityEditorModule.DefaultAbilityNodes);
	}
}


FGameplayTagContainer UEditorGASOnly_FunctionsLibrary::RequestGameplayTagChildren(const FGameplayTag& GameplayTag)
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
