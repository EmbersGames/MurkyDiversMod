// Embers 2024


#include "GameplayTagWizard.h"

#include "AssetToolsModule.h"
#include "BlueprintEditorModule.h"
#include "EditorAssetLibrary.h"
#include "EditorOnlyGAS_FunctionsLibrary.h"
#include "EditorUtilitySubsystem.h"
#include "EditorUtilityWidgetBlueprint.h"
#include "GAS_UtilityInputProcessor.h"
#include "Framework/Commands/UICommandInfo.h"
#include "Internationalization/Text.h"
#include "LevelEditor.h"
#include "SBlueprintEditorToolbar.h"
#include "GAS_Utility/GAS_Utility.h"

#define LOCTEXT_NAMESPACE "FGAS_UtilityEditorModule"

class FLevelEditorModule;


GameplayTagWizard::GameplayTagWizard() : TCommands<GameplayTagWizard>(
	TEXT("GameplayTagWizardCommands"),
	NSLOCTEXT("Contexts", "GameplayTagWizardCommands", "GameplayTag Wizard Commands"),
	NAME_None,
	FAppStyle::GetAppStyleSetName()
)
{
}


void GameplayTagWizard::OpenGameplayTagWizardFunc()
{
	
	UObject* Blueprint = UEditorAssetLibrary::LoadAsset(FString(TEXT("EditorUtilityWidgetBlueprint'/GAS_Utility/Tool/GameplayTag/EUWB_GameplayTag_Wizard'")));
	if (IsValid(Blueprint))
	{
		UEditorUtilityWidgetBlueprint* EditorWidget = Cast<UEditorUtilityWidgetBlueprint>(Blueprint);
		if (IsValid(EditorWidget))
		{
			UEditorUtilitySubsystem* EditorUtilitySubsystem = GEditor->GetEditorSubsystem<UEditorUtilitySubsystem>();
			FName TabId;
			UEditorUtilityWidget* Widget = EditorUtilitySubsystem->SpawnAndRegisterTabAndGetID(EditorWidget, TabId);

			if(Widget->GetClass()->ImplementsInterface(UTabEditorUtilityWidget::StaticClass())){
				ITabEditorUtilityWidget::Execute_SetTabID(Widget,TabId);
			}
		}
	}
}


void GameplayTagWizard::RegisterCommands()
{
	
	UI_COMMAND(OpenGameplayTagWizard, "OpenGameplayTagWizard", "Open gameplay tag wizard", EUserInterfaceActionType::Button, FInputChord(EKeys::T, false, true, false, false));
	FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));
	TSharedPtr<FUICommandList> CommandList = LevelEditorModule.GetGlobalLevelEditorActions();
	CommandList->MapAction(OpenGameplayTagWizard, FExecuteAction::CreateStatic(&OpenGameplayTagWizardFunc), FCanExecuteAction());
	FBlueprintEditorModule& BlueprintEditorModule = FModuleManager::LoadModuleChecked<FBlueprintEditorModule>("Kismet");
	TSharedPtr<FUICommandList> CommandListBP = BlueprintEditorModule.GetsSharedBlueprintEditorCommands();
	CommandListBP->MapAction(OpenGameplayTagWizard, FExecuteAction::CreateStatic(&OpenGameplayTagWizardFunc), FCanExecuteAction());

	TSharedPtr<FUICommandList> CommandListGAS = FGAS_UtilityInputProcessor::Get().GetCommandList();
	CommandListGAS->MapAction(OpenGameplayTagWizard, FExecuteAction::CreateStatic(&OpenGameplayTagWizardFunc), FCanExecuteAction());
	{
		check(GEditor);

		if (UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>())
		{
			AssetEditorSubsystem->OnAssetOpenedInEditor().AddLambda([this](UObject* InAsset, IAssetEditorInstance* InInstance) {
	OnAssetOpened(InAsset, InInstance);
});
		}
	}
}

void GameplayTagWizard::OnAssetOpened(UObject* Asset, IAssetEditorInstance* AssetEditor)
{
	
	const FString AssetEditorName = AssetEditor ? AssetEditor->GetEditorName().ToString() : "nullptr";
	check(Asset);
	UE_LOG(LogTemp, Log, TEXT("Asset opened %s (%s)"), *Asset->GetName(), *AssetEditorName);

	// Update assets by tab
	if (TSharedPtr<SDockTab> Tab = GetTabForAssetEditor(AssetEditor))
	{
		AssetsByTab.Add(TWeakPtr<SDockTab>(Tab), Asset);
	}

	// apply the toolbar to the newly opened asset
	OnAssetOpenedInEditor(Asset, AssetEditor);
}


TSharedPtr<SDockTab> GameplayTagWizard::GetTabForAssetEditor(IAssetEditorInstance* AssetEditor) const
{
	if (AssetEditor)
	{
		TSharedPtr<FTabManager> TabManager = AssetEditor->GetAssociatedTabManager();
		if (TabManager.IsValid())
		{
			return TabManager->GetOwnerTab();
		}
	}

	return TSharedPtr<SDockTab>();
}


void GameplayTagWizard::OnAssetOpenedInEditor(UObject* Asset, IAssetEditorInstance* AssetEditor)
{
	if (!Asset || !AssetEditor)
	{
		return;
	}
	TArray<FName> SupportedAssetEditors = {
		"SoundCueEditor",
		"Niagara",
		"BlueprintEditor",
		"ControlRigEditor",
		"MaterialEditor",
		"MetaSoundEditor",
		"Behavior Tree",
		"AnimationBlueprintEditor",
		"Environment Query",
		"GameplayAbilitiesEditor",
		"FSMBlueprintEditor",
		"WidgetBlueprintEditor",
		"PCGEditor",
		"FlowEditor",
		"DialogueEditor"
	};
	UE_LOG(LogTemp, Log, TEXT("AssetEditor name : %s (%s)"), *Asset->GetName(), *AssetEditor->GetEditorName().ToString());

	if (!SupportedAssetEditors.Contains(AssetEditor->GetEditorName()))
	{
		return;
	}
	
	FAssetEditorToolkit* AssetEditorToolkit = static_cast<FAssetEditorToolkit*>(AssetEditor);
	if (AssetEditorToolkit)
	{
		
		TWeakPtr<FAssetEditorToolkit> WeakToolkit = AssetEditorToolkit->AsShared();
		TSharedRef<FUICommandList> ToolkitCommands = AssetEditorToolkit->GetToolkitCommands();
		
		ToolkitCommands->MapAction(OpenGameplayTagWizard, FExecuteAction::CreateStatic(&OpenGameplayTagWizardFunc), FCanExecuteAction());
		
		TSharedPtr<FExtender> Extender = ToolbarExtenderMap.FindRef(WeakToolkit);
		if (Extender.IsValid())
		{
			AssetEditorToolkit->RemoveToolbarExtender(Extender);
		}

		TSharedRef<FExtender> ToolbarExtender = MakeShareable(new FExtender);

		if (AssetEditor)
		{
			TSharedPtr<FTabManager> TabManager = AssetEditor->GetAssociatedTabManager();
			if (TabManager.IsValid())
			{
				ToolbarExtender->AddToolBarExtension(
					"Asset",
					EExtensionHook::After,
					ToolkitCommands,
					FToolBarExtensionDelegate::CreateRaw(this, &GameplayTagWizard::ExtendToolbarAndProcessTab, TWeakPtr<SDockTab>(TabManager->GetOwnerTab())));
			}
		}
		else
		{
			TSharedRef<const FExtensionBase> Extension = ToolbarExtender->AddToolBarExtension(
				"Asset",
				EExtensionHook::After,
				ToolkitCommands,
				FToolBarExtensionDelegate::CreateRaw(this, &GameplayTagWizard::ExtendToolbar));
		}

		ToolbarExtenderMap.Add(WeakToolkit, ToolbarExtender);
		AssetEditorToolkit->AddToolbarExtender(ToolbarExtender);
	}
}


void GameplayTagWizard::ExtendToolbarAndProcessTab(FToolBarBuilder& ToolbarBuilder, TWeakPtr<SDockTab> Tab)
{
	if (!Tab.IsValid())
	{
		return;
	}

	ExtendToolbar(ToolbarBuilder);
}

void GameplayTagWizard::ExtendToolbar(FToolBarBuilder& ToolbarBuilder)
{
	ToolbarBuilder.AddToolBarButton(OpenGameplayTagWizard,NAME_None, MakeAttributeSP(this, &GameplayTagWizard::GetAddTagText), TAttribute<FText>(),
					FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.pyramid"));
}



#undef LOCTEXT_NAMESPACE
