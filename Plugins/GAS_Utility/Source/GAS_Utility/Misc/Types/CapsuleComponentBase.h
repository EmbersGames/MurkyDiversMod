// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "CapsuleComponentBase.generated.h"

/**
 * 
 */
UCLASS(ClassGroup="Collision",Blueprintable,BlueprintType, editinlinenew, hidecategories=(Object,LOD,Lighting,TextureStreaming), meta=(DisplayName="Capsule Collision", BlueprintSpawnableComponent))
class GAS_UTILITY_API UCapsuleComponentBase : public UCapsuleComponent
{
	GENERATED_BODY()
	
};
