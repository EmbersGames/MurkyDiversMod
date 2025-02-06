// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/MotionWarpingComponentBase.h"

#include "AnimNotify/RootMotionModifier/RootMotionModifier_SkewWarp_Base.h"

void UMotionWarpingComponentBase::TriggerWarpStart(UAnimNotifyState_MotionWarping* NotifyMotionWarping, URootMotionModifier* Modifier)
{
	OnWarpStart.Broadcast(NotifyMotionWarping, Modifier);
}

void UMotionWarpingComponentBase::TriggerWarpUpdate(UAnimNotifyState_MotionWarping* NotifyMotionWarping, URootMotionModifier* Modifier)
{
	OnWarUpdate.Broadcast(NotifyMotionWarping, Modifier);
}

void UMotionWarpingComponentBase::TriggerWarpEnd(UAnimNotifyState_MotionWarping* NotifyMotionWarping, URootMotionModifier* Modifier)
{
	OnWarpEnd.Broadcast(NotifyMotionWarping, Modifier);
}

void UMotionWarpingComponentBase::RemoveModifierFromTemplate(URootMotionModifier* Modifier)
{
	if (Modifier)
	{
		if (const URootMotionModifier_SkewWarp_Base* ModifierCasteD = Cast<URootMotionModifier_SkewWarp_Base>(Modifier))
		{
			for (int i = Modifiers.Num() - 1; i >= 0; i--)
			{
				if (TObjectPtr<URootMotionModifier> TempModifier = Modifiers[i])
				{
					if (const URootMotionModifier_SkewWarp_Base* Casted = Cast<URootMotionModifier_SkewWarp_Base>(TempModifier))
					{
						if (ModifierCasteD->Notify == Casted->Notify && Casted->Notify)
							Modifiers.RemoveAt(i);
					}
				}
			}
		}
	}
}
