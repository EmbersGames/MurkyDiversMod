// Embers 2024


#include "GameplayAbilityWizard.h"

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


GameplayAbilityWizard::GameplayAbilityWizard() : TCommands<GameplayAbilityWizard>(
	TEXT("GameplayAbilityWizardCommands"),
	NSLOCTEXT("Contexts", "GameplayAbilityWizardCommands", "GameplayTag Wizard Commands"),
	NAME_None,
	FAppStyle::GetAppStyleSetName()
)
{
}


void GameplayAbilityWizard::OpenGameplayAbilityWizardFunc()
{
	
	UObject* Blueprint = UEditorAssetLibrary::LoadAsset(FString(TEXT("EditorUtilityWidgetBlueprint'/GAS_Utility/Tool/GameplayAbility/EUWB_GameplayAbility_Wizard'")));
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


void GameplayAbilityWizard::RegisterCommands()
{
	
	UI_COMMAND(OpenGameplayAbilityWizard, "OpenGameplayAbilityWizard", "Open Ability wizard", EUserInterfaceActionType::Button, FInputChord(FKey(TEXT("<")), false, true, false, false));
	FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));
	TSharedPtr<FUICommandList> CommandList = LevelEditorModule.GetGlobalLevelEditorActions();
	CommandList->MapAction(OpenGameplayAbilityWizard, FExecuteAction::CreateStatic(&OpenGameplayAbilityWizardFunc), FCanExecuteAction());
	FBlueprintEditorModule& BlueprintEditorModule = FModuleManager::LoadModuleChecked<FBlueprintEditorModule>("Kismet");
	TSharedPtr<FUICommandList> CommandListBP = BlueprintEditorModule.GetsSharedBlueprintEditorCommands();
	CommandListBP->MapAction(OpenGameplayAbilityWizard, FExecuteAction::CreateStatic(&OpenGameplayAbilityWizardFunc), FCanExecuteAction());

	TSharedPtr<FUICommandList> CommandListGAS = FGAS_UtilityInputProcessor::Get().GetCommandList();
	CommandListGAS->MapAction(OpenGameplayAbilityWizard, FExecuteAction::CreateStatic(&OpenGameplayAbilityWizardFunc), FCanExecuteAction());
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

void GameplayAbilityWizard::OnAssetOpened(UObject* Asset, IAssetEditorInstance* AssetEditor)
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


TSharedPtr<SDockTab> GameplayAbilityWizard::GetTabForAssetEditor(IAssetEditorInstance* AssetEditor) const
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


void GameplayAbilityWizard::OnAssetOpenedInEditor(UObject* Asset, IAssetEditorInstance* AssetEditor)
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
		
		ToolkitCommands->MapAction(OpenGameplayAbilityWizard, FExecuteAction::CreateStatic(&OpenGameplayAbilityWizardFunc), FCanExecuteAction());
		
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
					FToolBarExtensionDelegate::CreateRaw(this, &GameplayAbilityWizard::ExtendToolbarAndProcessTab, TWeakPtr<SDockTab>(TabManager->GetOwnerTab())));
			}
		}
		else
		{
			TSharedRef<const FExtensionBase> Extension = ToolbarExtender->AddToolBarExtension(
				"Asset",
				EExtensionHook::After,
				ToolkitCommands,
				FToolBarExtensionDelegate::CreateRaw(this, &GameplayAbilityWizard::ExtendToolbar));
		}

		ToolbarExtenderMap.Add(WeakToolkit, ToolbarExtender);
		AssetEditorToolkit->AddToolbarExtender(ToolbarExtender);
	}
}


void GameplayAbilityWizard::ExtendToolbarAndProcessTab(FToolBarBuilder& ToolbarBuilder, TWeakPtr<SDockTab> Tab)
{
	if (!Tab.IsValid())
	{
		return;
	}

	ExtendToolbar(ToolbarBuilder);
}

void GameplayAbilityWizard::ExtendToolbar(FToolBarBuilder& ToolbarBuilder)
{
	ToolbarBuilder.AddToolBarButton(OpenGameplayAbilityWizard,NAME_None,
		MakeAttributeSP(this, &GameplayAbilityWizard::GetAbilityTagText),
		TAttribute<FText>(),
					FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Blueprints"));
}



#undef LOCTEXT_NAMESPACE
