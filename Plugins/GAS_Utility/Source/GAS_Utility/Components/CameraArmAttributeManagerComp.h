// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "Camera/CameraComponent.h"
#include "Components/PawnComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "CameraArmAttributeManagerComp.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable, ClassGroup = Player, meta=(BlueprintSpawnableComponent))
class GAS_UTILITY_API UCameraArmAttributeManagerComp : public UPawnComponent
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	UFUNCTION(BlueprintCallable, Category="AttributeBinding")
	virtual void InitAttributeBinding();

	// Fov
	void Fov_AttributeChanged(const FOnAttributeChangeData& Data);
	UFUNCTION(BlueprintImplementableEvent, Category = "Fov")
	void Fov_AttributeChangeEvent(const float OldValue, const float NewValue);

	// ArmLength
	void ArmLength_AttributeChanged(const FOnAttributeChangeData& Data);
	UFUNCTION(BlueprintImplementableEvent, Category = "ArmLength")
	void ArmLength_AttributeChangeEvent(const float OldValue, const float NewValue);

	// SocketOffsetX
	void SocketOffsetX_AttributeChanged(const FOnAttributeChangeData& Data);
	UFUNCTION(BlueprintImplementableEvent, Category = "Offset")
	void SocketOffsetX_AttributeChangeEvent(const float OldValue, const float NewValue);

	// SocketOffsetY
	void SocketOffsetY_AttributeChanged(const FOnAttributeChangeData& Data);
	UFUNCTION(BlueprintImplementableEvent, Category = "Offset")
	void SocketOffsetY_AttributeChangeEvent(const float OldValue, const float NewValue);

	// SocketOffsetZ
	void SocketOffsetZ_AttributeChanged(const FOnAttributeChangeData& Data);
	UFUNCTION(BlueprintImplementableEvent, Category = "Offset")
	void SocketOffsetZ_AttributeChangeEvent(const float OldValue, const float NewValue);

	// TargetOffsetX
	void TargetOffsetX_AttributeChanged(const FOnAttributeChangeData& Data);
	UFUNCTION(BlueprintImplementableEvent, Category = "Offset")
	void TargetOffsetX_AttributeChangeEvent(const float OldValue, const float NewValue);

	// TargetOffsetY
	void TargetOffsetY_AttributeChanged(const FOnAttributeChangeData& Data);
	UFUNCTION(BlueprintImplementableEvent, Category = "Offset")
	void TargetOffsetY_AttributeChangeEvent(const float OldValue, const float NewValue);

	// TargetOffsetZ
	void TargetOffsetZ_AttributeChanged(const FOnAttributeChangeData& Data);
	UFUNCTION(BlueprintImplementableEvent, Category = "Offset")
	void TargetOffsetZ_AttributeChangeEvent(const float OldValue, const float NewValue);

	// MaxPitch
	void MaxPitch_AttributeChanged(const FOnAttributeChangeData& Data);
	UFUNCTION(BlueprintImplementableEvent, Category = "MaxPitch")
	void MaxPitch_AttributeChangeEvent(const float OldValue, const float NewValue);

	// MinPitch
	void MinPitch_AttributeChanged(const FOnAttributeChangeData& Data);
	UFUNCTION(BlueprintImplementableEvent, Category = "MinPitch")
	void MinPitch_AttributeChangeEvent(const float OldValue, const float NewValue);


	// Roll
	void Roll_AttributeChanged(const FOnAttributeChangeData& Data);
	UFUNCTION(BlueprintImplementableEvent, Category = "Roll")
	void Roll_AttributeChangeEvent(const float OldValue, const float NewValue);

	// LookUpRate
	void LookUpRate_AttributeChanged(const FOnAttributeChangeData& Data);
	UFUNCTION(BlueprintImplementableEvent, Category = "LookUpRate")
	void LookUpRate_AttributeChangeEvent(const float OldValue, const float NewValue);

	// TurnRate
	void TurnRate_AttributeChanged(const FOnAttributeChangeData& Data);
	UFUNCTION(BlueprintImplementableEvent, Category = "TurnRate")
	void TurnRate_AttributeChangeEvent(const float OldValue, const float NewValue);

	// OrientRotationToMovement
	void OrientRotationToMovement_AttributeChanged(const FOnAttributeChangeData& Data);
	UFUNCTION(BlueprintImplementableEvent, Category = "Rotation")
	void OrientRotationToMovement_AttributeChangeEvent(const float OldValue, const float NewValue);

	// CameraLagRotation
	void CameraLagRotation_AttributeChanged(const FOnAttributeChangeData& Data);
	UFUNCTION(BlueprintImplementableEvent, Category = "CameraLagRotation")
	void CameraLagRotation_AttributeChangeEvent(const float OldValue, const float NewValue);
	
	// CameraLag
	void CameraLag_AttributeChanged(const FOnAttributeChangeData& Data);
	UFUNCTION(BlueprintImplementableEvent, Category = "CameraLag")
	void CameraLag_AttributeChangeEvent(const float OldValue, const float NewValue);


	UPROPERTY(BlueprintReadWrite, Category="AttributeBinding")
	bool bAttributesBinded;

	UPROPERTY(BlueprintReadOnly, Category = "CameraArm")
	TWeakObjectPtr<USpringArmComponent> SpringArmComponent;
	UPROPERTY(BlueprintReadOnly, Category = "CameraArm")
	TWeakObjectPtr<UCameraComponent> CameraComponent;
};
