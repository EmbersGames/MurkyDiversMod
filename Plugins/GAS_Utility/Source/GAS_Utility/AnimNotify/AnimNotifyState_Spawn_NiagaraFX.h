// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimNotifyState_TimedNiagaraEffect.h"
#include "AnimNotifyState_Spawn_NiagaraFX.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "Complete Advanced Timed Niagara Effect (GAS Utility)"))
class GAS_UTILITY_API UAnimNotifyState_Spawn_NiagaraFX : public UAnimNotifyState_TimedNiagaraEffectAdvanced
{
	UAnimNotifyState_Spawn_NiagaraFX(const FObjectInitializer& ObjectInitializer);
	GENERATED_BODY()
	virtual UFXSystemComponent* SpawnEffect(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) const override;
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	virtual UFXSystemComponent* SpawnEffectOnFloor(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) const;

public:
	UPROPERTY(EditAnywhere, Category = NiagaraSystem,meta=(AllowPreserveRatio,DisplayAfter="RotationOffset"))
	FVector RelativeScale;

	float SavedDuration;

	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = TranslucentSortPriority, meta = (DisplayName = "Override Translucent Sort Priority",InlineEditConditionToggle))
	bool bOverrideTranslucentSortPriority;
	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = TranslucentSortPriority, meta = (DisplayName = "Override Translucent Sort Priority", EditCondition="bOverrideTranslucentSortPriority"))
	int32 TranslucentSortPriorityValue;
	
	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = Decal, meta = (DisplayName = "Receives Decals"))
	bool bReceivesDecals;
	
	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = Stunned)
	bool bDestroyIfStunned;
	
	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = SpawnOnFloor, meta = (DisplayName = "Spawn particle on floor",ToolTip="Fx will not be attached (Spawn at location), Floor_Normal user parameter will be set to FX"))
	bool bSpawnOnFloor;

	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = SpawnOnFloor, meta = (EditCondition="bSpawnOnFloor",EditConditionHides))
	FVector FloorLocationOffset;

	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = SpawnOnFloor, meta = (DisplayName = "Floor Collision profiles", EditCondition="bSpawnOnFloor",EditConditionHides))
	TArray<TEnumAsByte<ECollisionChannel>> FloorCollisionProfile;

	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = SetNiagaraParameter)
	TMap<FName,double> NiagaraParameterByFloat;
	
	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = SetNiagaraParameter)
	TMap<FName,FVector> NiagaraParameterByVector;
	
	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = SetNiagaraParameter)
	TMap<FName,bool> NiagaraParameterByBool;
	
	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = SetNiagaraParameter)
	TMap<FName,FVector2D> NiagaraParameterByVector2D;
	
	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = SetNiagaraParameter)
	TMap<FName,FName> NiagaraParameterByBoneLocation;
	
	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = SceneCapture, meta = (DisplayName = "Add To Player Scene Capture",ToolTip="Visible in scene capture Only will be set true and Scene capture comp of player will receive the FX in Show only component list."))
	bool OnlyForPlayerSceneCaptureComponent;
};
