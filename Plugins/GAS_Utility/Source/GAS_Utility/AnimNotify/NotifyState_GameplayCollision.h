// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "Particles/ParticleSystem.h"
#include "Components/MeshComponent.h"
#include "NotifyState_GameplayCollision.generated.h"


/**
 * 
 */
class UNotifyState_GameplayCollision;
UCLASS(BlueprintType, Blueprintable, EditInlineNew)
class GAS_UTILITY_API UNotifyGameplayCollisionHandle : public UObject
{
	GENERATED_BODY()

public:
	UNotifyGameplayCollisionHandle(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GAS_Utility", meta=(AnimNotifyBoneName="true"))
	FName OverrideSocketName;

	UFUNCTION(BlueprintNativeEvent, Category="AnimNotify")
	void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference, const UNotifyState_GameplayCollision* Notify);

	UFUNCTION(BlueprintNativeEvent, Category="AnimNotify")
	void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference, const UNotifyState_GameplayCollision* Notify);

	UFUNCTION(BlueprintNativeEvent, Category="AnimNotify")
	void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference, const UNotifyState_GameplayCollision* Notify);

	UPROPERTY(BlueprintReadOnly, Category="AnimNotify")
	const UNotifyState_GameplayCollision* NotifyOwner;

	UFUNCTION(BlueprintPure,Category="AnimNotify")
	UGameInstance* GetGameInstance() const;
	
	UPROPERTY(BlueprintReadOnly, Category="AnimNotify")
	const UMeshComponent* MeshCompOwner;
	
	UPROPERTY(BlueprintReadWrite, Category="NotifyGameplayCollision")
	UFXSystemComponent* VisualDebugComp;
	
	UPROPERTY(BlueprintReadOnly, Category="NotifyGameplayCollision")
	float NotifyTotalDuration;
	
	UPROPERTY(BlueprintReadWrite, Category="NotifyGameplayCollision")
	UPrimitiveComponent* PrimitiveCollision;
	
	UPROPERTY(BlueprintReadOnly, Category="AnimNotify")
	UAnimSequenceBase* AnimationNotify;
	
	UPROPERTY(BlueprintReadOnly, Category="AnimNotify")
	FAnimNotifyEventReference EventReferenceNotify;
	
	UPROPERTY(BlueprintReadOnly, Category="AnimNotify")
	FGameplayTag TagForCollisionAddEvent;

};

/**
 * 
 */
UCLASS()
class GAS_UTILITY_API UNotifyState_GameplayCollision : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category="GameplayCollision")
	TArray<TObjectPtr<UNotifyGameplayCollisionHandle>> GameplayCollisions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GAS_Utility", meta=(AnimNotifyBoneName="true"))
	FName SocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GAS_Utility",AdvancedDisplay)
	FGameplayTag TagForCollisionAddEvent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GAS_Utility",AdvancedDisplay)
	FGameplayTag ActionGameplayTag;

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;

	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;

	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
