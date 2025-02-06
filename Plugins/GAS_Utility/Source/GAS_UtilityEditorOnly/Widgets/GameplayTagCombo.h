// Embers 2024

#pragma once

#include "CoreMinimal.h"
#include "SGameplayTagCombo.h"
#include "Components/Widget.h"
#include "GameplayTagCombo.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameplayTagChangeDelegate, const FGameplayTag&, Tag);

/**
 * 
 */
UCLASS(Blueprintable,BlueprintType)
class GAS_UTILITYEDITORONLY_API UGameplayTagCombo : public UWidget
{
	GENERATED_BODY()
	
protected:
	virtual void BuildContentWidget();
	TSharedPtr<SBorder> GetDisplayWidget() const { return DisplayedWidget; }

	//~ UWidget interface
public:

	DECLARE_DELEGATE_OneParam(FOnTagChanged, const FGameplayTag /*Tag*/)
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	virtual const FText GetPaletteCategory() override;

	UFUNCTION(BlueprintCallable,Category=EditorGameplayTag)
	void SetGameplayTag(const FGameplayTag Tag);
	
	UPROPERTY(BlueprintReadOnly,EditAnywhere,Category=EditorGameplayTag)
	FGameplayTag GameplayTag;
	
	void OnGameplayTagChange( const FGameplayTag Tag);

	UPROPERTY(BlueprintAssignable, Category = "Gameplay Tags")
	FOnGameplayTagChangeDelegate OnGameplayTagComboChange;


protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	
	TSharedPtr<SGameplayTagCombo> GameplayTagComboWidget;
	//~ End of UWidget interface

	//~ UObject interface
public:
	virtual void PostLoad() override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	//~ End of UObject interface

private:
	TSharedPtr<SBorder> DisplayedWidget;
};
