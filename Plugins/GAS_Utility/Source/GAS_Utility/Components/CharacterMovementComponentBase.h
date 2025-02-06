// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CharacterMovementComponentBase.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable, ClassGroup = Movement, meta=(BlueprintSpawnableComponent))
class GAS_UTILITY_API UCharacterMovementComponentBase : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UCharacterMovementComponentBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// ============================= Direction / Rotation =============================
	virtual FRotator ComputeOrientToMovementRotation(const FRotator& CurrentRotation, float DeltaTime, FRotator& DeltaRotation) const override;

	virtual void PhysicsRotation(float DeltaTime) override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	virtual void SetDesiredRotation(FRotator DesiredRot, float DeltaTime);

	UFUNCTION(BlueprintCallable, Category="Character Movement|Direction")
	virtual void SetDesiredDirection(FVector NewDesiredDirection);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Character Movement|Direction")
	virtual FVector GetDesiredDirection() const;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite, Category="Character Movement|Direction")
	FVector DesiredDirection;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite, Category="Character Movement|Direction")
	bool bForceDesiredDirection;

	double CurrentTurning_Angle;
	bool bIsCurrentlyTurning_ToLeft;
	bool bSavedCurrentTurning_Angle;

	UFUNCTION(BlueprintCallable,BlueprintPure, Category="Character Movement|Direction")
	virtual double GetCurrentTurning_Angle(bool& TurningToLeft);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Character Movement|Direction")
	virtual double GetCurrentTurning_AngleValue();

	UFUNCTION(BlueprintCallable,BlueprintPure, Category="Character Movement|Direction")
	virtual bool GetCurrentTurning_AngleDirection();


	// ============================= Other =============================
};
