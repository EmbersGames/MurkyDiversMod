// Copyright Embers 2024

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "Containers/Ticker.h"
#include "LimitedVelocityStaticMeshComp.generated.h"

/**
 * 
 */
UCLASS()
class MURKYDIVERSGAME_API ULimitedVelocityStaticMeshComp : public UStaticMeshComponent
{
	GENERATED_BODY()
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	UFUNCTION()
	bool CustomTick(float Deltatime);
	void LimitVelocity();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bLimitVelocity = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	double MaxVelocity = 0.f;

	/** Delegate for callbacks to Tick */
	FTickerDelegate TickDelegate;

	/** Handle to various registered delegates */
	FTSTicker::FDelegateHandle TickDelegateHandle;
};
