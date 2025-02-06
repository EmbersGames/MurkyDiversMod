// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS_Utility/Components/CharacterMovementComponentBase.h"

#include "LyraCharacterMovementComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MURKYDIVERSGAME_API ULyraCharacterMovementComponent : public UCharacterMovementComponentBase
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	ULyraCharacterMovementComponent();

};
