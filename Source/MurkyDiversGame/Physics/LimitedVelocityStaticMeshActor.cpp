// Copyright Embers 2024


#include "Physics/LimitedVelocityStaticMeshActor.h"

#include "LimitedVelocityStaticMeshComp.h"

ALimitedVelocityStaticMeshActor::ALimitedVelocityStaticMeshActor(const FObjectInitializer& ObjectInitializer) :
Super(ObjectInitializer.SetDefaultSubobjectClass<ULimitedVelocityStaticMeshComp>(AStaticMeshActor::StaticMeshComponentName))
{
	
}
