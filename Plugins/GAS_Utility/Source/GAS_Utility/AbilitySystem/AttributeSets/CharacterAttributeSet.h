// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "GAS_Utility/AbilitySystem/AbilitySystemComponentBase.h"
#include "CharacterAttributeSet.generated.h"

/**
* This macro defines a set of helper functions for accessing and initializing attributes.
*
* The following example of the macro:
*    ATTRIBUTE_ACCESSORS(ULyraHealthSet, Health)
* will create the following functions:
*    static FGameplayAttribute GetHealthAttribute();
*    float GetHealth() const;
*    void SetHealth(float NewVal);
*    void InitHealth(float NewVal);
*/
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)
/**
*
*/
UCLASS(Blueprintable,BlueprintType)
class GAS_UTILITY_API UCharacterAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public :

	
	UFUNCTION()
	void OnRep_MaxSpeed(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_MaxAcceleration(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_AirControl(const FGameplayAttributeData& OldValue);

	UPROPERTY(BlueprintReadOnly, Category = "Speed")
	FGameplayAttributeData Speed;
	ATTRIBUTE_ACCESSORS(UCharacterAttributeSet, Speed)
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxSpeed, Category = "Speed", Meta = (AllowPrivateAccess = true),replicated)
	FGameplayAttributeData MaxSpeed;
	ATTRIBUTE_ACCESSORS(UCharacterAttributeSet, MaxSpeed)
	
	UPROPERTY(BlueprintReadOnly, Category = "Gravity")
	FGameplayAttributeData GravityScale;
	ATTRIBUTE_ACCESSORS(UCharacterAttributeSet, GravityScale)
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxAcceleration, Category = "MaxAcceleration",Meta = (AllowPrivateAccess = true),replicated)
	FGameplayAttributeData MaxAcceleration;
	ATTRIBUTE_ACCESSORS(UCharacterAttributeSet, MaxAcceleration)
	
	UPROPERTY(BlueprintReadOnly, Category = "MaxStepHeight")
	FGameplayAttributeData MaxStepHeight;
	ATTRIBUTE_ACCESSORS(UCharacterAttributeSet, MaxStepHeight)
	
	UPROPERTY(BlueprintReadOnly, Category = "WalkableFloorAngle")
	FGameplayAttributeData WalkableFloorAngle;
	ATTRIBUTE_ACCESSORS(UCharacterAttributeSet, WalkableFloorAngle)
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AirControl, Category = "AirControl",Meta = (AllowPrivateAccess = true),replicated)
	FGameplayAttributeData AirControl;
	ATTRIBUTE_ACCESSORS(UCharacterAttributeSet, AirControl)
	
	UPROPERTY(BlueprintReadOnly, Category = "Rotation")
	FGameplayAttributeData RotationRate;
	ATTRIBUTE_ACCESSORS(UCharacterAttributeSet, RotationRate)
	
	UPROPERTY(BlueprintReadOnly, Category = "Rotation")
	FGameplayAttributeData OrientRotationToMovement;
	ATTRIBUTE_ACCESSORS(UCharacterAttributeSet, OrientRotationToMovement)
	
	UPROPERTY(BlueprintReadOnly, Category = "RootMotion")
	FGameplayAttributeData RootMotionScale;
	ATTRIBUTE_ACCESSORS(UCharacterAttributeSet, RootMotionScale)
	
	UPROPERTY(BlueprintReadOnly, Category = "SlowWalkSpeedByDirection")
	FGameplayAttributeData SlowWalkSpeedByDirection;
	ATTRIBUTE_ACCESSORS(UCharacterAttributeSet, SlowWalkSpeedByDirection)
	
	UPROPERTY(BlueprintReadOnly, Category = "OrientDesiredDirectionToTarget")
	FGameplayAttributeData OrientDesiredDirectionToTarget;
	ATTRIBUTE_ACCESSORS(UCharacterAttributeSet, OrientDesiredDirectionToTarget)

	
	UAbilitySystemComponentBase* GetAbilitySystemComponentBase() const;
};
