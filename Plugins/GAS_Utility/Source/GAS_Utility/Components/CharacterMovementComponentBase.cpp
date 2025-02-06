// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterMovementComponentBase.h"

#include "GameFramework/Character.h"

UCharacterMovementComponentBase::UCharacterMovementComponentBase(const FObjectInitializer& ObjectInitializer)
{
	DesiredDirection = FVector::ZeroVector;
	bForceDesiredDirection = false;
	bUseAccelerationForPaths = true;
	bIsCurrentlyTurning_ToLeft = true;
	CurrentTurning_Angle = 0.f;
	bSavedCurrentTurning_Angle = false;
}

FRotator UCharacterMovementComponentBase::ComputeOrientToMovementRotation(const FRotator& CurrentRotation, float DeltaTime, FRotator& DeltaRotation) const
{
	if (bForceDesiredDirection && DesiredDirection.SizeSquared() > KINDA_SMALL_NUMBER)
	{
		return DesiredDirection.GetSafeNormal().Rotation();
	}
	if (Acceleration.SizeSquared() < KINDA_SMALL_NUMBER)
	{
		// AI path following request can orient us in that direction (it's effectively an acceleration)
		if (bHasRequestedVelocity && RequestedVelocity.SizeSquared() > KINDA_SMALL_NUMBER)
		{
			return (RequestedVelocity).GetSafeNormal().Rotation();
		}

		if (DesiredDirection.SizeSquared() > KINDA_SMALL_NUMBER)
		{
			return DesiredDirection.GetSafeNormal().Rotation();
		}

		// Don't change rotation if there is no acceleration.
		return CurrentRotation;
	}
	return Super::ComputeOrientToMovementRotation(CurrentRotation, DeltaTime, DeltaRotation);
}

void UCharacterMovementComponentBase::PhysicsRotation(float DeltaTime)
{
	if (!bOrientRotationToMovement && DesiredDirection.SizeSquared() > KINDA_SMALL_NUMBER)
	{
		SetDesiredRotation(DesiredDirection.Rotation(), DeltaTime);
	}

	if (!(bOrientRotationToMovement || bUseControllerDesiredRotation))
	{
		return;
	}

	if (!HasValidData() || (!CharacterOwner->Controller && !bRunPhysicsWithNoController))
	{
		return;
	}

	Super::PhysicsRotation(DeltaTime);

	if (UpdatedComponent)
	{
		// Disable NewDesiredDirection if the actual direction == NewDesiredDirection
		FRotator CurrentRotation = UpdatedComponent->GetComponentRotation();
		if (DesiredDirection.SizeSquared() > KINDA_SMALL_NUMBER)
		{
			if (CurrentRotation.Equals(DesiredDirection.GetSafeNormal().Rotation(), 0.1f))
			{
				DesiredDirection = FVector(0);
			}
		}
		ComputeOrientToMovementRotation(CurrentRotation, 0.1, CurrentRotation).Vector();
	}
}

void UCharacterMovementComponentBase::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	bSavedCurrentTurning_Angle = false;
}


void UCharacterMovementComponentBase::SetDesiredRotation(FRotator DesiredRot, float DeltaTime)
{
	const float AngleTolerance = 1e-3f;

	const FRotator CurrentRotation = UpdatedComponent->GetComponentRotation(); // Normalized
	CurrentRotation.DiagnosticCheckNaN(TEXT("CharacterMovementComponent::PhysicsRotation(): CurrentRotation"));

	const FRotator DeltaRot = GetDeltaRotation(DeltaTime);

	if (!CurrentRotation.Equals(DesiredRot, AngleTolerance))
	{
		// PITCH
		if (!FMath::IsNearlyEqual(CurrentRotation.Pitch, DesiredRot.Pitch, AngleTolerance))
		{
			DesiredRot.Pitch = FMath::FixedTurn(CurrentRotation.Pitch, DesiredRot.Pitch, DeltaRot.Pitch);
		}

		// YAW
		if (!FMath::IsNearlyEqual(CurrentRotation.Yaw, DesiredRot.Yaw, AngleTolerance))
		{
			DesiredRot.Yaw = FMath::FixedTurn(CurrentRotation.Yaw, DesiredRot.Yaw, DeltaRot.Yaw);
		}

		// ROLL
		if (!FMath::IsNearlyEqual(CurrentRotation.Roll, DesiredRot.Roll, AngleTolerance))
		{
			DesiredRot.Roll = FMath::FixedTurn(CurrentRotation.Roll, DesiredRot.Roll, DeltaRot.Roll);
		}

		// Set the new rotation.
		DesiredRot.DiagnosticCheckNaN(TEXT("CharacterMovementComponent::PhysicsRotation(): DesiredRotation"));

		bool res = MoveUpdatedComponent(FVector::ZeroVector, DesiredRot, /*bSweep*/ false);
	}
}

void UCharacterMovementComponentBase::SetDesiredDirection(FVector NewDesiredDirection)
{
	if (NewDesiredDirection != FVector::ZeroVector)
	{
		DesiredDirection = NewDesiredDirection.GetSafeNormal2D();
	}
	else
	{
		DesiredDirection = FVector::ZeroVector;
	}
}

FVector UCharacterMovementComponentBase::GetDesiredDirection() const
{
	return DesiredDirection;
}

double UCharacterMovementComponentBase::GetCurrentTurning_Angle(bool& TurningToLeft)
{
	if (!CharacterOwner || !IsValid(CharacterOwner))
		return 0.f;
	if (bSavedCurrentTurning_Angle)
	{
		TurningToLeft = bIsCurrentlyTurning_ToLeft;
		return CurrentTurning_Angle;
	}
	if (DesiredDirection.SquaredLength() > 0.00001f)
	{
		const FVector ForwardVector = CharacterOwner->GetActorForwardVector();
		const FVector RightDirection = CharacterOwner->GetActorRightVector();
		const FVector InputVectorNormalize = DesiredDirection.GetSafeNormal2D();

		const double Angle = FMath::RadiansToDegrees(FMath::Acos(ForwardVector.CosineAngle2D(InputVectorNormalize)));
		CurrentTurning_Angle = Angle;
		bIsCurrentlyTurning_ToLeft = FMath::RadiansToDegrees(FMath::Acos(RightDirection.CosineAngle2D(InputVectorNormalize))) > 90.f;
		bSavedCurrentTurning_Angle = true;
		TurningToLeft = bIsCurrentlyTurning_ToLeft;
		return CurrentTurning_Angle;
	}
	if (bOrientRotationToMovement)
	{
		const FVector InputVector = CharacterOwner->Internal_GetLastMovementInputVector();
		if (InputVector.SquaredLength() > 0.0001f)
		{
			const FVector ForwardVector = CharacterOwner->GetActorForwardVector();
			const FVector RightDirection = CharacterOwner->GetActorRightVector();
			const FVector InputVectorNormalize = InputVector.GetSafeNormal2D();

			const double Angle = FMath::RadiansToDegrees(FMath::Acos(ForwardVector.CosineAngle2D(InputVectorNormalize)));
			CurrentTurning_Angle = Angle;
			bIsCurrentlyTurning_ToLeft = FMath::RadiansToDegrees(FMath::Acos(RightDirection.CosineAngle2D(InputVectorNormalize))) > 90.f;
			bSavedCurrentTurning_Angle = true;
			TurningToLeft = bIsCurrentlyTurning_ToLeft;
			return CurrentTurning_Angle;
		}
	}
	bSavedCurrentTurning_Angle = true;
	CurrentTurning_Angle = 0.f;
	bIsCurrentlyTurning_ToLeft = true;
	TurningToLeft = bIsCurrentlyTurning_ToLeft;
	return 0.f;
}

double UCharacterMovementComponentBase::GetCurrentTurning_AngleValue()
{
	bool bDirection;
	return GetCurrentTurning_Angle(bDirection);
}

bool UCharacterMovementComponentBase::GetCurrentTurning_AngleDirection()
{
	bool bDirection;
	GetCurrentTurning_Angle(bDirection);
	return bDirection;
}

