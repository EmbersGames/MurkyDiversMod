// Embers 2024

#pragma once

#include "CoreMinimal.h"
#include "Framework/Application/IInputProcessor.h"

/**
 * 
 */
class GAS_UTILITYEDITORONLY_API FGAS_UtilityInputProcessor final
	: public TSharedFromThis<FGAS_UtilityInputProcessor>
	, public IInputProcessor
{
public:
	static void Create();
	void Cleanup();

	static FGAS_UtilityInputProcessor& Get();
	FGAS_UtilityInputProcessor();
	~FGAS_UtilityInputProcessor();
	const TSharedPtr<FUICommandList>& GetCommandList() { return CommandList; }
	TSharedPtr<FUICommandList> CommandList;
	bool CanExecuteCommand(TSharedRef<const FUICommandInfo> Command) const;
	bool TryExecuteCommand(TSharedRef<const FUICommandInfo> Command);
	virtual void Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor) override;
	bool ProcessCommandBindings(TSharedPtr<FUICommandList> LCommandList, const FKeyEvent& KeyEvent);
	virtual bool HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override;
};
