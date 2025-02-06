// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "Components/PawnComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS_Utility/AbilitySystem/GameplayChangerBase.h"
#include "CharacterAttributeManagerComp.generated.h"

/**
 * 
 */
UCLASS(BlueprintType,Blueprintable, ClassGroup = Character,meta=(BlueprintSpawnableComponent))
class GAS_UTILITY_API UCharacterAttributeManagerComp : public UPawnComponent
{
	GENERATED_BODY()

public:

	UCharacterAttributeManagerComp(const FObjectInitializer& ObjectInitializer);
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void Activate(bool bReset) override;

	UFUNCTION(BlueprintCallable, Category="AttributeBinding")
	virtual void InitAttributeBinding();
	
	void Speed_AttributeChanged(const FOnAttributeChangeData& Data);
	UFUNCTION(BlueprintImplementableEvent,Category="Speed")
	void Speed_AttributeChangeEvent(const float OldValue,const float NewValue);

	void MaxSpeed_AttributeChanged(const FOnAttributeChangeData& Data);
	UFUNCTION(BlueprintImplementableEvent, Category = "MaxSpeed")
	void MaxSpeed_AttributeChangeEvent(const float OldValue, const float NewValue);

	void GravityScale_AttributeChanged(const FOnAttributeChangeData& Data);
	UFUNCTION(BlueprintImplementableEvent, Category = "GravityScale")
	void GravityScale_AttributeChangeEvent(const float OldValue, const float NewValue);

	void MaxAcceleration_AttributeChanged(const FOnAttributeChangeData& Data);
	UFUNCTION(BlueprintImplementableEvent, Category = "MaxAcceleration")
	void MaxAcceleration_AttributeChangeEvent(const float OldValue, const float NewValue);

	void MaxStepHeight_AttributeChanged(const FOnAttributeChangeData& Data);
	UFUNCTION(BlueprintImplementableEvent, Category = "MaxStepHeight")
	void MaxStepHeight_AttributeChangeEvent(const float OldValue, const float NewValue);

	void WalkableFloorAngle_AttributeChanged(const FOnAttributeChangeData& Data);
	UFUNCTION(BlueprintImplementableEvent, Category = "WalkableFloorAngle")
	void WalkableFloorAngle_AttributeChangeEvent(const float OldValue, const float NewValue);

	void AirControl_AttributeChanged(const FOnAttributeChangeData& Data);
	UFUNCTION(BlueprintImplementableEvent, Category = "AirControl")
	void AirControl_AttributeChangeEvent(const float OldValue, const float NewValue);

	void RotationRate_AttributeChanged(const FOnAttributeChangeData& Data);
	UFUNCTION(BlueprintImplementableEvent, Category = "RotationRate")
	void RotationRate_AttributeChangeEvent(const float OldValue, const float NewValue);

	void OrientRotationToMovement_AttributeChanged(const FOnAttributeChangeData& Data);
	UFUNCTION(BlueprintImplementableEvent, Category = "OrientRotationToMovement")
	void OrientRotationToMovement_AttributeChangeEvent(const float OldValue, const float NewValue);

	void RootMotionScale_AttributeChanged(const FOnAttributeChangeData& Data);
	UFUNCTION(BlueprintImplementableEvent, Category = "RootMotionScale")
	void RootMotionScale_AttributeChangeEvent(const float OldValue, const float NewValue);

	void SlowWalkSpeedByDirection_AttributeChanged(const FOnAttributeChangeData& Data);
	UFUNCTION(BlueprintImplementableEvent, Category = "SlowWalkSpeedByDirection")
	void SlowWalkSpeedByDirection_AttributeChangeEvent(const float OldValue, const float NewValue);
	


	UPROPERTY(BlueprintReadOnly,Category="Speed")
	float SpeedAttribute;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category=Speed)
	bool bIsFlyingByDefault;

	UPROPERTY(BlueprintReadWrite, Category="AttributeBinding")
	bool bAttributesBinded;
	
	UPROPERTY()
	TSoftObjectPtr<ACharacter>  CharacterOwner;
	
	UPROPERTY()
	TSoftObjectPtr<UAbilitySystemComponent>  AbilitySystemComp;

	UPROPERTY(BlueprintReadOnly,Category = "CharacterMovement")
	TWeakObjectPtr<UCharacterMovementComponent> CharacterMovementComponent;


	// Animation Drive SlowFactors

	UFUNCTION(BlueprintCallable,BlueprintNativeEvent,Category="Slow|Animation")
	void ApplySlowFromAnimation();
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Slow|Animation")
	bool bEnableSlowWalk;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Slow|Animation")
	bool bEnableSlowRotation;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Slow|Animation")
	bool bEnableSlowWalkAndRotation;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Slow|Animation")
	FName SlowWalk_CurveName;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Slow|Animation")
	FName SlowRotation_CurveName;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Slow|Animation")
	FName SlowWalkAndRotation_CurveName;

	UPROPERTY(BlueprintReadWrite,Category="Slow|Animation")
	UGameplayChangerBase* SlowGameplayChanger;

	UPROPERTY(BlueprintReadWrite,Category="Slow|Animation")
	UAttributeChanger* SlowWalk_AttributeChanger;
	
	UPROPERTY(BlueprintReadWrite,Category="Slow|Animation")
	UAttributeChanger* SlowRotation_AttributeChanger;
	
	UPROPERTY(BlueprintReadWrite,Category="Slow|Animation")
	UAttributeChanger* SlowWalkAndRotation_AttributeChangerForWalk;
	
	UPROPERTY(BlueprintReadWrite,Category="Slow|Animation")
	UAttributeChanger* SlowWalkAndRotation_AttributeChangerForRotation;
	
	void SetupSlowGameplayChanger();

/* Character is Slow if he try to move in other direction than his forward direction */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,BlueprintSetter="SetSlowWalkSpeedByDirection",Category="Slow|Rotation")
	bool bSlowWalkSpeedByDirection;

	UFUNCTION(BlueprintCallable,BlueprintSetter,Category="Slow|Rotation")
	void SetSlowWalkSpeedByDirection(bool NewSlowWalkSpeedByDirection);
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Slow|Rotation")
	double SlowWalkSpeedByDirection_Factor;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Slow|Rotation")
	double SlowWalkSpeedByDirection_SafeAngle;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Slow|Rotation")
	double SlowWalkSpeedByDirection_BlendAngle;
	
	UPROPERTY(BlueprintReadWrite,Category="Slow|Rotation")
	UAttributeChanger* SlowWalkSpeedByDirection_AttributeChanger;
	
	void ApplySlowWalkSpeedFromDirection();
	
};



