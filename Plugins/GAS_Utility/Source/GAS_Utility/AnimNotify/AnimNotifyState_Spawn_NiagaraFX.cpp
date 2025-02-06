// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/AnimNotifyState_Spawn_NiagaraFX.h"

#include "Components/SkeletalMeshComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "NativeGameplayTags.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/SceneCaptureComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Particles/ParticleSystemComponent.h"

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_IsStunned, "System.Stun.IsStunned");
UAnimNotifyState_Spawn_NiagaraFX::UAnimNotifyState_Spawn_NiagaraFX(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	FloorCollisionProfile.Add(ECC_WorldStatic);
	RelativeScale = FVector::OneVector;
	bReceivesDecals = true;
	OnlyForPlayerSceneCaptureComponent = false;
	SavedDuration = -1.f;
}

UFXSystemComponent* UAnimNotifyState_Spawn_NiagaraFX::SpawnEffect(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) const
{
	UFXSystemComponent* FxComp = nullptr;
	if (bSpawnOnFloor)
		FxComp = SpawnEffectOnFloor(MeshComp, Animation);
	else
	{
		FxComp = Super::SpawnEffect(MeshComp, Animation);
		if(FxComp)
			FxComp->SetRelativeScale3D(RelativeScale);
	}
	if (FxComp)
	{
		if (bOverrideTranslucentSortPriority)
			FxComp->SetTranslucentSortPriority(TranslucentSortPriorityValue);
		FxComp->SetReceivesDecals(bReceivesDecals);
		FxComp->SetFloatParameter(FName("TotalDuration"), SavedDuration);
		for (const auto KeyPair : NiagaraParameterByFloat)
		{
			FxComp->SetFloatParameter(KeyPair.Key, KeyPair.Value);
		}
		for (const auto KeyPair : NiagaraParameterByVector)
		{
			FxComp->SetVectorParameter(KeyPair.Key, KeyPair.Value);
		}
		for (const auto KeyPair : NiagaraParameterByBool)
		{
			FxComp->SetBoolParameter(KeyPair.Key, KeyPair.Value);
		}
		if (UNiagaraComponent* CompFxNiag = Cast<UNiagaraComponent>(FxComp))
			for (const auto KeyPair : NiagaraParameterByVector2D)
			{
				CompFxNiag->SetVariableVec2(KeyPair.Key, KeyPair.Value);
			}
		if (MeshComp)
			for (const auto KeyPair : NiagaraParameterByBoneLocation)
			{
				FxComp->SetVectorParameter(KeyPair.Key, MeshComp->GetSocketLocation(KeyPair.Value));
			}

		if(OnlyForPlayerSceneCaptureComponent)
		{
			FxComp->SetVisibleInSceneCaptureOnly(true);
			const ACharacter* Player = UGameplayStatics::GetPlayerCharacter(MeshComp, 0);
			if( Player && IsValid(Player))
			{
				UActorComponent* SceneCaptureComp = Player->GetComponentByClass(USceneCaptureComponent::StaticClass());
				if(SceneCaptureComp && IsValid(SceneCaptureComp))
				{
					USceneCaptureComponent* SceneCaptureCompCasted = Cast<USceneCaptureComponent>(SceneCaptureComp);
					if(SceneCaptureCompCasted && IsValid(SceneCaptureCompCasted))
					{
						SceneCaptureCompCasted->ShowOnlyComponent(FxComp);
					}
				}
			}
		}
	}
	return FxComp;
}

void UAnimNotifyState_Spawn_NiagaraFX::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	SavedDuration = TotalDuration;
	Super::NotifyBegin(MeshComp, Animation, TotalDuration,EventReference);
}

void UAnimNotifyState_Spawn_NiagaraFX::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
	if (UFXSystemComponent* FXComponent = GetSpawnedEffect(MeshComp))
	{
		if (MeshComp)
			for (const auto KeyPair : NiagaraParameterByBoneLocation)
			{
				FXComponent->SetVectorParameter(KeyPair.Key, MeshComp->GetSocketLocation(KeyPair.Value));
			}
		if(bDestroyIfStunned)
		{
			AActor* Owner = MeshComp->GetOwner();
			if(Owner)
			{
				auto OwnerCasted = Cast<IAbilitySystemInterface>(Owner);
				if(OwnerCasted)
				{
					auto GasComp = OwnerCasted->GetAbilitySystemComponent();
					if(GasComp)
					{
						if(GasComp->HasMatchingGameplayTag(TAG_IsStunned)){
							FXComponent->DeactivateImmediate();
							FXComponent->DestroyComponent();
						}
					}
				}
			}
		}
	}
}

void UAnimNotifyState_Spawn_NiagaraFX::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
}

UFXSystemComponent* UAnimNotifyState_Spawn_NiagaraFX::SpawnEffectOnFloor(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) const
{
	// Only spawn if we've got valid params
	if (ValidateParameters(MeshComp))
	{
		FVector InitialPosition = MeshComp->GetSocketLocation(SocketName);
		FRotator Rotation = FRotator(FQuat(MeshComp->GetComponentRotation()) * FQuat(RotationOffset));
		InitialPosition += LocationOffset;
		FHitResult HitResult;
		FCollisionQueryParams CollisionParams;
		FCollisionObjectQueryParams ObjectParams;
		for (auto Collision : FloorCollisionProfile)
		{
			ObjectParams.AddObjectTypesToQuery(Collision);
		}

		CollisionParams.AddIgnoredActor(MeshComp->GetOwner());

		bool bHit = GetWorld()->LineTraceSingleByObjectType(
			HitResult,
			InitialPosition,
			InitialPosition + FVector(0, 0, -50000),
			ObjectParams,
			CollisionParams
		);

		if (bHit)
		{
			InitialPosition = HitResult.ImpactPoint + FloorLocationOffset;
		}
		UNiagaraComponent* CompFx = UNiagaraFunctionLibrary::SpawnSystemAtLocation(MeshComp, Template, InitialPosition, Rotation, RelativeScale, !bDestroyAtEnd, true);
		CompFx->ComponentTags.Add(FName("SpawnEffectOnFloor"));
		if (bHit)
			CompFx->SetVectorParameter(FName("Floor_Normal"), HitResult.ImpactNormal);
		return CompFx;
	}
	return nullptr;
}
