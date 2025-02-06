// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlueprintBase.h"
#include "BT_Service_Blueprint_Base.generated.h"

/**
 * 
 */
UCLASS()
class GAS_UTILITY_API UBT_Service_Blueprint_Base : public UBTService_BlueprintBase
{
	GENERATED_BODY()
	
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
public:

	UFUNCTION(BlueprintNativeEvent,Category=Editor)
	void InitializeInBlueprint(AActor* Controller, UObject* InParentNode);
    
	UFUNCTION(BlueprintCallable,Category=Interval)
	void SetInterval(const float NewInterval, const float NewRandomDeviation)
	{
		Interval = NewInterval;
		RandomDeviation = NewRandomDeviation;
	}

	UFUNCTION(BlueprintCallable,Category=BehaviourTree)
	UObject* GetFirstParentNodeDecoratorOfClass(UClass* ParentClass);
    
	UFUNCTION(BlueprintCallable,BlueprintPure,Category=Interval)
	float GetInterval() const { return Interval; }

	UFUNCTION(BlueprintCallable,BlueprintPure,Category=Interval)
	float GetRandomDeviation() const { return RandomDeviation; }

	// Display Editor Functions
	UFUNCTION(BlueprintCallable,BlueprintPure,Category=Editor)
	FString GetCurrentNodeName() const { return NodeName; }

	UFUNCTION(BlueprintCallable,Category=Editor)
	void SetNodeName(FString NewName);

	virtual FString GetStaticDescription() const override;

	UFUNCTION(BlueprintNativeEvent,Category=Editor)
	FString GetCustomDescription();
    
	UFUNCTION(BlueprintCallable,BlueprintPure,Category=BehaviourTree)
	UObject* GetInstanceParentNode();
    
	// ==========

    
	UObject* RecursiveSearchParentNodeDecoratorOfClass(UBTCompositeNode* Node, UBTCompositeNode* CurParentNode, UClass* ParentClass);
};
