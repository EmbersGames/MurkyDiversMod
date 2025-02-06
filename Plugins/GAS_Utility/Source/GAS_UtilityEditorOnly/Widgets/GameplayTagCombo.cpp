// Embers 2024


#include "Widgets/GameplayTagCombo.h"

#include "SGameplayTagCombo.h"

#define LOCTEXT_NAMESPACE "UMG"

void UGameplayTagCombo::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	DisplayedWidget.Reset();
}


void UGameplayTagCombo::OnGameplayTagChange(const FGameplayTag Tag)
{
	if(GameplayTagComboWidget.IsValid())
	{
		GameplayTag = Tag;
		BuildContentWidget();
		OnGameplayTagComboChange.Broadcast(Tag);
	}
}

TSharedRef<SWidget> UGameplayTagCombo::RebuildWidget()
{
	DisplayedWidget = SNew(SBorder)
		.Padding(0.0f)
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		.BorderImage(FAppStyle::GetBrush("NoBorder"));

	BuildContentWidget();

	return DisplayedWidget.ToSharedRef();
}


void UGameplayTagCombo::PostLoad()
{
	Super::PostLoad();

	if (!HasAnyFlags(RF_BeginDestroyed))
		BuildContentWidget();
}


void UGameplayTagCombo::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
}


const FText UGameplayTagCombo::GetPaletteCategory()
{
	return LOCTEXT("Editor", "Editor");
}

void UGameplayTagCombo::SetGameplayTag(const FGameplayTag Tag)
{
	GameplayTag = Tag;
	RebuildWidget();
}

void UGameplayTagCombo::BuildContentWidget()
{
	GameplayTagComboWidget.Reset();

	if (!GetDisplayWidget().IsValid())
	{
		return;
	}
	if (GIsEditor)
	{
		FOnTagChanged PropertyChanged = FOnTagChanged::CreateUObject(this, &UGameplayTagCombo::OnGameplayTagChange);
		GameplayTagComboWidget = SNew(SGameplayTagCombo)
			.Tag(GameplayTag)
		.OnTagChanged(PropertyChanged)
		.EnableNavigation(true);

		if (GameplayTagComboWidget.IsValid())
		{
			GetDisplayWidget()->SetContent(GameplayTagComboWidget.ToSharedRef());
		}
	}
}


#undef LOCTEXT_NAMESPACE
