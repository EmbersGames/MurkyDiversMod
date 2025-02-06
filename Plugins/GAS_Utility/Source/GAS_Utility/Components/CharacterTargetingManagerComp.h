// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS_Utility/AbilitySystem/AbilitySystemComponentBase.h"
#include "Components/PawnComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CharacterTargetingManagerComp.generated.h"


/**
 * 
 */
UCLASS(BlueprintType,Blueprintable, ClassGroup = IA,meta=(BlueprintSpawnableComponent))
class GAS_UTILITY_API UCharacterTargetingManagerComp : public UPawnComponent
{
	GENERATED_BODY()

public:

	UCharacterTargetingManagerComp(const FObjectInitializer& ObjectInitializer);
	
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void HandleMoveBackward();
	
	virtual void HandleOrientDesiredDirectionToTarget();

	UPROPERTY(BlueprintReadOnly,VisibleAnywhere,Category="Target|Direction",meta=(ToolTip = "Use OrientDesiredDirectionToTarget Character Attribute to change this value"))
	bool bOrientDesiredDirectionToTarget;

	UFUNCTION(BlueprintPure,BlueprintNativeEvent,Category="Target|MovingBackward")
	FVector GetMovingBackwardDirection();

	UFUNCTION(BlueprintPure, BlueprintNativeEvent,Category="Target|Getter")
	FVector GetTargetLocation();
	
	UFUNCTION(BlueprintPure,BlueprintNativeEvent, Category="Target|Getter")
	float GetAngleToTarget();
	
	UFUNCTION(BlueprintPure,BlueprintNativeEvent, Category="Target|Getter")
	float GetSignedAngleToTarget();
	
	UFUNCTION(BlueprintPure,BlueprintNativeEvent, Category="Target|Getter")
	FVector GetDirectionToTarget();
	
	UFUNCTION(BlueprintPure,BlueprintNativeEvent, Category="Target|Getter")
	FVector GetDirection2DToTarget();
		
	UFUNCTION(BlueprintPure,BlueprintNativeEvent, Category="Target|Getter")
	float GetCapsuleDistanceToTarget();
	
	UFUNCTION(BlueprintPure,BlueprintNativeEvent, Category="Target|Getter")
	float GetCapsuleDistance2DToTarget();
	
	UFUNCTION(BlueprintPure,BlueprintNativeEvent, Category="Target|Getter")
	float GetDistanceToTarget();
	
	UFUNCTION(BlueprintPure,BlueprintNativeEvent, Category="Target|Getter")
	float GetDistance2DToTarget();
	
	UFUNCTION(BlueprintPure,BlueprintNativeEvent, Category="Target|Getter")
	AActor* GetCharacterTarget();
	
	UFUNCTION(BlueprintPure,BlueprintNativeEvent, Category="Target|Getter")
	bool HaveCharacterTarget();

	UPROPERTY()
	TSoftObjectPtr<ACharacter> CharacterOwner;

	UPROPERTY(BlueprintReadOnly, Category="Target|Getter")
	TSoftObjectPtr<UAbilitySystemComponentBase> AbilitySystemComp;

	UPROPERTY()
	TWeakObjectPtr<UCharacterMovementComponent> CharacterMovementComponent;

	UPROPERTY(BlueprintReadOnly, Category=Target)
	AActor* TargetActor;

	UFUNCTION(BlueprintCallable, Category="Target")
	virtual void InitBinding();

	UFUNCTION()
	void TargetObjectChange(FGameplayTag Tag);

// OrientRotationToMovement Attribute Changer
	
	UPROPERTY(BlueprintReadWrite,Category="Target|OrientRotationToMovement")
	UGameplayChangerBase* OrientRotationToMovement_GameplayChanger;

	UPROPERTY(BlueprintReadWrite,Category="Target|OrientRotationToMovement")
	UAttributeChanger* OrientRotationToMovement_AttributeChanger;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Target|Gameplay Changer")
	TSubclassOf<UGameplayChangerBase> WithTargetGC_Class;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Target|Gameplay Changer")
	UGameplayChangerBase* WithTargetGC;
	
	void OrientDesiredDirectionToTarget_AttributeChanged(const FOnAttributeChangeData& Data);
};
