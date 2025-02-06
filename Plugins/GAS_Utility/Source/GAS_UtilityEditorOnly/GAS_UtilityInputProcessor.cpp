// Embers 2024


#include "GAS_UtilityInputProcessor.h"

#include "GameplayTagWizard.h"

static TSharedPtr<FGAS_UtilityInputProcessor> GAS_UtilityInputProcessor;

void FGAS_UtilityInputProcessor::Create()
{
	GAS_UtilityInputProcessor = MakeShareable(new FGAS_UtilityInputProcessor());
	if (FSlateApplication::IsInitialized())
		FSlateApplication::Get().RegisterInputPreProcessor(GAS_UtilityInputProcessor);
}

void FGAS_UtilityInputProcessor::Cleanup()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().UnregisterInputPreProcessor(GAS_UtilityInputProcessor);
	}
	GAS_UtilityInputProcessor.Reset();
}

FGAS_UtilityInputProcessor& FGAS_UtilityInputProcessor::Get()
{
	return *GAS_UtilityInputProcessor;
}

FGAS_UtilityInputProcessor::FGAS_UtilityInputProcessor()
{
	CommandList = MakeShareable(new FUICommandList());
}

FGAS_UtilityInputProcessor::~FGAS_UtilityInputProcessor()
{
}

void FGAS_UtilityInputProcessor::Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor)
{
}

bool FGAS_UtilityInputProcessor::HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent)
{
	if (ProcessCommandBindings(CommandList, InKeyEvent))
	{
		return true;
	}
	return false;
}

bool FGAS_UtilityInputProcessor::CanExecuteCommand(TSharedRef<const FUICommandInfo> Command) const
{
	if (const FUIAction* Action = CommandList->GetActionForCommand(Command))
	{
		return Action->CanExecute();
	}

	return false;
}

bool FGAS_UtilityInputProcessor::TryExecuteCommand(TSharedRef<const FUICommandInfo> Command)
{
	if (const FUIAction* Action = CommandList->GetActionForCommand(Command))
	{
		if (Action->CanExecute())
		{
			return Action->Execute();
		}
	}

	return false;
}

bool FGAS_UtilityInputProcessor::ProcessCommandBindings(TSharedPtr<FUICommandList> LCommandList, const FKeyEvent& KeyEvent)
{
	if (FSlateApplication::Get().IsDragDropping())
	{
		return false;
	}

	FModifierKeysState ModifierKeysState = FSlateApplication::Get().GetModifierKeys();
	const FInputChord CheckChord(KeyEvent.GetKey(), EModifierKey::FromBools(
		                             ModifierKeysState.IsControlDown(),
		                             ModifierKeysState.IsAltDown(),
		                             ModifierKeysState.IsShiftDown(),
		                             ModifierKeysState.IsCommandDown()));


	const FInputBindingManager& InputBindingManager = FInputBindingManager::Get();

	TArray<TSharedPtr<FUICommandInfo>> LocalCommandInfos;
	InputBindingManager.GetCommandInfosFromContext(GameplayTagWizard::Get().GetContextName(), LocalCommandInfos);

	// Only active chords process commands
	constexpr bool bCheckDefault = false;

	static const TArray<FName> ContextNames = {GameplayTagWizard::Get().GetContextName()};

	// Check to see if there is any command in the context activated by the chord
	for (const FName& ContextName : ContextNames)
	{
		TSharedPtr<FUICommandInfo> Command = FInputBindingManager::Get().FindCommandInContext(ContextName, CheckChord, bCheckDefault);

		if (Command.IsValid() && Command->HasActiveChord(CheckChord))
		{
			// Find the bound action for this command
			const FUIAction* Action = LCommandList->GetActionForCommand(Command);

			// If there is no Action mapped to this command list, continue to the next context
			if (Action)
			{
				if (Action->CanExecute() && (!KeyEvent.IsRepeat() || Action->CanRepeat()))
				{
					// Block the command if we have disabled it in the settings

					// If the action was found and can be executed, do so now
					return Action->Execute();
				}
			}
		}
	}

	return false;
}
