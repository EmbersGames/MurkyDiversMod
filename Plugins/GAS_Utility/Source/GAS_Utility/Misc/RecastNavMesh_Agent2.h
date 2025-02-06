// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NavMesh/RecastNavMesh.h"
#include "RecastNavMesh_Agent2.generated.h"

/**
 * 
 */
UCLASS()
class GAS_UTILITY_API ARecastNavMesh_Agent2 : public ARecastNavMesh
{
	GENERATED_BODY()
public:
	ARecastNavMesh_Agent2(const FObjectInitializer& ObjectInitializer);
	
};
