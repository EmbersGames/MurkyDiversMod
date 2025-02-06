// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/NotifyState_GameplayCollision.h"
#include "Engine/World.h"
#include "Components/MeshComponent.h"

UNotifyGameplayCollisionHandle::UNotifyGameplayCollisionHandle(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
	OverrideSocketName = "None";
	NotifyTotalDuration = 0.f;
	AnimationNotify = nullptr;
	MeshCompOwner = nullptr;
	NotifyOwner = nullptr;
	PrimitiveCollision = nullptr;
}

void UNotifyGameplayCollisionHandle::NotifyTick_Implementation(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference, const UNotifyState_GameplayCollision* Notify)
{
}

void UNotifyGameplayCollisionHandle::NotifyBegin_Implementation(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference, const UNotifyState_GameplayCollision* Notify)
{
}

void UNotifyGameplayCollisionHandle::NotifyEnd_Implementation(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference, const UNotifyState_GameplayCollision* Notify)
{
}

UGameInstance* UNotifyGameplayCollisionHandle::GetGameInstance() const
{
	if (IsValid(MeshCompOwner))
	{
		if (const auto World = MeshCompOwner->GetWorld())
		{
			return World->GetGameInstance();
		}
	}
	return nullptr;
}

void UNotifyState_GameplayCollision::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	for (auto GameplayCollisionHandle : GameplayCollisions)
	{
		if (IsValid(GameplayCollisionHandle))
		{
			GameplayCollisionHandle->NotifyOwner = this;
			GameplayCollisionHandle->MeshCompOwner = Cast<UMeshComponent>(MeshComp);
			GameplayCollisionHandle->AnimationNotify = Animation;
			GameplayCollisionHandle->EventReferenceNotify = EventReference;
			GameplayCollisionHandle->NotifyTotalDuration = TotalDuration;
			GameplayCollisionHandle->TagForCollisionAddEvent = TagForCollisionAddEvent;
			GameplayCollisionHandle->NotifyBegin(MeshComp, Animation, TotalDuration, EventReference, this);
		}
	}
}

void UNotifyState_GameplayCollision::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
	for (auto GameplayCollisionHandle : GameplayCollisions)
	{
		if (IsValid(GameplayCollisionHandle))
		{
			GameplayCollisionHandle->NotifyOwner = this;
			GameplayCollisionHandle->MeshCompOwner = Cast<UMeshComponent>(MeshComp);
			GameplayCollisionHandle->NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference, this);
		}
	}
}

void UNotifyState_GameplayCollision::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	for (auto GameplayCollisionHandle : GameplayCollisions)
	{
		if (IsValid(GameplayCollisionHandle))
		{
			GameplayCollisionHandle->NotifyOwner = this;
			GameplayCollisionHandle->MeshCompOwner = Cast<UMeshComponent>(MeshComp);
			GameplayCollisionHandle->NotifyEnd(MeshComp, Animation, EventReference, this);
		}
	}
}
