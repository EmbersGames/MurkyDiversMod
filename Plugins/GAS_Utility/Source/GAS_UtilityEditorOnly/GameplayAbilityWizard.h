// Embers 2024

#pragma once
#include "CoreMinimal.h"


/**
 * 
 */
class GAS_UTILITYEDITORONLY_API GameplayAbilityWizard final : public TCommands<GameplayAbilityWizard>
{
public:
	GameplayAbilityWizard();

	static void OpenGameplayAbilityWizardFunc();
	TSharedPtr<FUICommandInfo> OpenGameplayAbilityWizard;
	void OnAssetOpened(UObject* Asset, IAssetEditorInstance* AssetEditor);
	
	TSharedPtr<SDockTab> GetTabForAssetEditor(IAssetEditorInstance* AssetEditor) const ;
	
	TMap<TWeakPtr<SDockTab>, TWeakObjectPtr<UObject>> AssetsByTab;
	void OnAssetOpenedInEditor(UObject* Asset, IAssetEditorInstance* AssetEditor);
	virtual void RegisterCommands() override;
	
	TMap<TWeakPtr<FAssetEditorToolkit>, TSharedPtr<FExtender>> ToolbarExtenderMap;
	void ExtendToolbarAndProcessTab(FToolBarBuilder& ToolbarBuilder, TWeakPtr<SDockTab> Tab);
	void ExtendToolbar(FToolBarBuilder& ToolbarBuilder);

	FText GetAbilityTagText() const { return FText::FromString("Add Ability"); }
	
};
