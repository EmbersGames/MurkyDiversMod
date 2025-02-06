// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "CameraArmAttributeSet.generated.h"

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
UCLASS()
class GAS_UTILITY_API UCameraArmAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
public:
	
	UPROPERTY(BlueprintReadOnly, Category = "Fov")
	FGameplayAttributeData Fov;
	ATTRIBUTE_ACCESSORS(UCameraArmAttributeSet, Fov)
	
	UPROPERTY(BlueprintReadOnly, Category = "ArmLength")
	FGameplayAttributeData ArmLength;
	ATTRIBUTE_ACCESSORS(UCameraArmAttributeSet, ArmLength)

	UPROPERTY(BlueprintReadOnly, Category = "Offset")
	FGameplayAttributeData SocketOffsetX;
	ATTRIBUTE_ACCESSORS(UCameraArmAttributeSet, SocketOffsetX)
	
	UPROPERTY(BlueprintReadOnly, Category = "Offset")
	FGameplayAttributeData SocketOffsetY;
	ATTRIBUTE_ACCESSORS(UCameraArmAttributeSet, SocketOffsetY)
	
	UPROPERTY(BlueprintReadOnly, Category = "Offset")
	FGameplayAttributeData SocketOffsetZ;
	ATTRIBUTE_ACCESSORS(UCameraArmAttributeSet, SocketOffsetZ)

	UPROPERTY(BlueprintReadOnly, Category = "Offset")
	FGameplayAttributeData TargetOffsetX;
	ATTRIBUTE_ACCESSORS(UCameraArmAttributeSet, TargetOffsetX)
	
	UPROPERTY(BlueprintReadOnly, Category = "Offset")
	FGameplayAttributeData TargetOffsetY;
	ATTRIBUTE_ACCESSORS(UCameraArmAttributeSet, TargetOffsetY)
	
	UPROPERTY(BlueprintReadOnly, Category = "Offset")
	FGameplayAttributeData TargetOffsetZ;
	ATTRIBUTE_ACCESSORS(UCameraArmAttributeSet, TargetOffsetZ)

	UPROPERTY(BlueprintReadOnly, Category = "PitchLimit")
	FGameplayAttributeData MaxPitch;
	ATTRIBUTE_ACCESSORS(UCameraArmAttributeSet, MaxPitch)
	
	UPROPERTY(BlueprintReadOnly, Category = "PitchLimit")
	FGameplayAttributeData MinPitch;
	ATTRIBUTE_ACCESSORS(UCameraArmAttributeSet, MinPitch)
	
	UPROPERTY(BlueprintReadOnly, Category = "Roll")
	FGameplayAttributeData Roll;
	ATTRIBUTE_ACCESSORS(UCameraArmAttributeSet, Roll)
	
	UPROPERTY(BlueprintReadOnly, Category = "LookUpRate")
	FGameplayAttributeData LookUpRate;
	ATTRIBUTE_ACCESSORS(UCameraArmAttributeSet, LookUpRate)
	
	UPROPERTY(BlueprintReadOnly, Category = "TurnRate")
	FGameplayAttributeData TurnRate;
	ATTRIBUTE_ACCESSORS(UCameraArmAttributeSet, TurnRate)
	
	UPROPERTY(BlueprintReadOnly, Category = "Rotation")
	FGameplayAttributeData OrientRotationToMovement;
	ATTRIBUTE_ACCESSORS(UCameraArmAttributeSet, OrientRotationToMovement)
	
	UPROPERTY(BlueprintReadOnly, Category = "OrientCharacter")
	FGameplayAttributeData OrientCharacterToCameraTarget;
	ATTRIBUTE_ACCESSORS(UCameraArmAttributeSet, OrientCharacterToCameraTarget)
	
	UPROPERTY(BlueprintReadOnly, Category = "CameraLag")
	FGameplayAttributeData CameraLag;
	ATTRIBUTE_ACCESSORS(UCameraArmAttributeSet, CameraLag)
	
	UPROPERTY(BlueprintReadOnly, Category = "CameraLagRotation")
	FGameplayAttributeData CameraLagRotation;
	ATTRIBUTE_ACCESSORS(UCameraArmAttributeSet, CameraLagRotation)
	
};
