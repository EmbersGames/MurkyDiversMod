// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/AnimNotifyState_MotionWarpingGAS.h"
#include "Components/SkeletalMeshComponent.h"
#include "MotionWarpingComponent.h"
#include "Components/MotionWarpingComponentBase.h"
#include "RootMotionModifier/RootMotionModifier_SkewWarp_Base.h"
#if WITH_EDITOR
void UAnimNotifyState_MotionWarpingGAS::OnAnimNotifyCreatedInEditor(FAnimNotifyEvent& ContainingAnimNotifyEvent)
{
	if (RootMotionModifier)
	{
		if (URootMotionModifier_SkewWarp_Base* Caster = Cast<URootMotionModifier_SkewWarp_Base>(RootMotionModifier))
		{
			Caster->Notify = this;
		}
	}
}
#endif
void UAnimNotifyState_MotionWarpingGAS::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	if (RootMotionModifier)
	{
		if (URootMotionModifier_SkewWarp_Base* Caster = Cast<URootMotionModifier_SkewWarp_Base>(RootMotionModifier))
		{
			Caster->Notify = this;
		}
	}

	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
}

void UAnimNotifyState_MotionWarpingGAS::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
if(MeshComp)
{
	if(const AActor* Owner = MeshComp->GetOwner())
	{
		if(UActorComponent* Comp = Owner->GetComponentByClass(UMotionWarpingComponentBase::StaticClass()))
		{
			if(UMotionWarpingComponentBase* MotionWarpingComponentBase = Cast<UMotionWarpingComponentBase>(Comp))
			{
				MotionWarpingComponentBase->RemoveModifierFromTemplate(RootMotionModifier);
			}
		}
	}	
}
	Super::NotifyEnd(MeshComp, Animation, EventReference);
}
