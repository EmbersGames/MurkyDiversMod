// Copyright Embers 2024


#include "Physics/LimitedVelocityStaticMeshComp.h"

bool ULimitedVelocityStaticMeshComp::CustomTick(float Deltatime)
{
	if (!IsValid(this))
		return false;
	if (bLimitVelocity)
		LimitVelocity();
	return true;
}

void ULimitedVelocityStaticMeshComp::BeginPlay()
{
	Super::BeginPlay();
	if (bLimitVelocity)
	{
		TickDelegate = FTickerDelegate::CreateUObject(this, &ULimitedVelocityStaticMeshComp::CustomTick);
		TickDelegateHandle = FTSTicker::GetCoreTicker().AddTicker(TickDelegate, 0.f);
	}
}

void ULimitedVelocityStaticMeshComp::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (TickDelegateHandle.IsValid())
		FTSTicker::GetCoreTicker().RemoveTicker(TickDelegateHandle);
}

void ULimitedVelocityStaticMeshComp::LimitVelocity()
{
	FVector Velocity = this->GetPhysicsLinearVelocity();
	if (Velocity.SizeSquared() > FMath::Square(MaxVelocity))
	{
		Velocity.Normalize();
		Velocity *= MaxVelocity;
		this->SetPhysicsLinearVelocity(Velocity);
	}
}
