// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/RootMotionModifier/RootMotionModifier_SkewWarp_Base.h"

#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "MotionWarpingComponent.h"
#include "DrawDebugHelpers.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"

URootMotionModifier_SkewWarp_Base::URootMotionModifier_SkewWarp_Base(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	InWarpPointAnimBoneOrientation = FRotator(-90.f, 0.f, 0.f);
	bOrientInWarpPointAnimToDirectionMeshToTarget = true;
	WarpPointAnimBoneName = "target_position0";
	WarpTargetName = "Target";
	RotationType = EMotionWarpRotationType::Facing;
	WarpPointAnimProvider = EWarpPointAnimProvider::Bone;
}

void URootMotionModifier_SkewWarp_Base::Update(const FMotionWarpingUpdateContext& Context)
{
	const ACharacter* CharacterOwner = GetCharacterOwner();
	if (CharacterOwner == nullptr)
	{
		return;
	}
	
	URootMotionModifier::Update(Context);

	// Cache sync point transform and trigger OnTargetTransformChanged if needed
	const UMotionWarpingComponent* OwnerComp = GetOwnerComponent();
	if (OwnerComp && GetState() == ERootMotionModifierState::Active)
	{
		const FMotionWarpingTarget* WarpTargetPtr = OwnerComp->FindWarpTarget(WarpTargetName);

		// Disable if there is no target for us
		if (WarpTargetPtr == nullptr)
		{
			UE_LOG(LogTemp, Verbose, TEXT("MotionWarping: Marking RootMotionModifier as Disabled. Reason: Invalid Warp Target (%s). Char: %s Animation: %s [%f %f] [%f %f]"),
			       *WarpTargetName.ToString(), *GetNameSafe(OwnerComp->GetOwner()), *GetNameSafe(Animation.Get()), StartTime, EndTime, PreviousPosition, CurrentPosition);

			SetState(ERootMotionModifierState::Disabled);
			return;
		}

		// Get the warp point sent by the game
		FTransform WarpPointTransformGame = WarpTargetPtr->GetTargetTrasform();

		// Initialize our target transform (where the root should end at the end of the window) with the warp point sent by the game
		FTransform TargetTransform = WarpPointTransformGame;

		// Check if a warp point is defined in the animation. If so, we need to extract it and offset the target transform 
		// the same amount the root bone is offset from the warp point in the animation
		if (WarpPointAnimProvider != EWarpPointAnimProvider::None)
		{
			if (!CachedOffsetFromWarpPoint.IsSet())
			{
				if ( CharacterOwner)
				{
					if (WarpPointAnimProvider == EWarpPointAnimProvider::Static)
					{
						CachedOffsetFromWarpPoint = UMotionWarpingUtilities::CalculateRootTransformRelativeToWarpPointAtTime(*CharacterOwner, GetAnimation(), EndTime, WarpPointAnimTransform);
					}
					else if (WarpPointAnimProvider == EWarpPointAnimProvider::Bone)
					{
						auto RootTransform = UMotionWarpingUtilities::CalculateRootTransformRelativeToWarpPointAtTime(*CharacterOwner, GetAnimation(), 0.f, WarpPointAnimTransform);

						CachedOffsetFromWarpPoint = UMotionWarpingUtilities::CalculateRootTransformRelativeToWarpPointAtTime(*CharacterOwner, GetAnimation(), EndTime, WarpPointAnimBoneName);

						CachedOffsetFromWarpPoint->SetLocation(InWarpPointAnimBoneOrientation.RotateVector(CachedOffsetFromWarpPoint->GetLocation() * CharacterOwner->GetMesh()->GetComponentScale()) + InWarpPointAnimBoneOffset);

						if (bOrientInWarpPointAnimToDirectionMeshToTarget)
						{
							const auto Direction = (TargetTransform.GetLocation() - CharacterOwner->GetActorLocation()).GetSafeNormal().Rotation();
							CachedOffsetFromWarpPoint->SetLocation(Direction.RotateVector(CachedOffsetFromWarpPoint->GetLocation()));
						}
					}
				}
			}

			// Update Target Transform based on the offset between the root and the warp point in the animation
			TargetTransform = CachedOffsetFromWarpPoint.GetValue() * WarpPointTransformGame;
		}

		if (!CachedTargetTransform.Equals(TargetTransform))
		{
			CachedTargetTransform = TargetTransform;

			OnTargetTransformChanged();
		}
	}
}

FTransform URootMotionModifier_SkewWarp_Base::ProcessRootMotion(const FTransform& InRootMotion, float DeltaSeconds)
{
	const ACharacter* CharacterOwner = GetCharacterOwner();
	if (CharacterOwner == nullptr)
	{
		return InRootMotion;
	}

	FTransform FinalRootMotion = InRootMotion;

	const FTransform RootMotionTotal = UMotionWarpingUtilities::ExtractRootMotionFromAnimation(Animation.Get(), PreviousPosition, EndTime);
	const FTransform RootMotionDelta = UMotionWarpingUtilities::ExtractRootMotionFromAnimation(Animation.Get(), PreviousPosition, FMath::Min(CurrentPosition, EndTime));

	FTransform ExtraRootMotion = FTransform::Identity;
	if (CurrentPosition > EndTime)
	{
		ExtraRootMotion = UMotionWarpingUtilities::ExtractRootMotionFromAnimation(Animation.Get(), EndTime, CurrentPosition);
	}

	if (bWarpTranslation)
	{
		const float CapsuleHalfHeight = CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		const FQuat CurrentRotation = CharacterOwner->GetActorQuat();
		const FVector CurrentLocation = (CharacterOwner->GetActorLocation() - CurrentRotation.GetUpVector() * CapsuleHalfHeight);

		const FVector DeltaTranslation = RootMotionDelta.GetLocation();
		const FVector TotalTranslation = RootMotionTotal.GetLocation();

		FVector TargetLocation = GetTargetLocation();
		if (bIgnoreZAxis)
		{
			TargetLocation.Z = CurrentLocation.Z;
		}

		// if there is translation in the animation, warp it
		if (!TotalTranslation.IsNearlyZero())
		{
			if (!DeltaTranslation.IsNearlyZero())
			{
				const FTransform MeshTransform = FTransform(CharacterOwner->GetBaseRotationOffset(), CharacterOwner->GetBaseTranslationOffset()) * CharacterOwner->GetActorTransform();
				TargetLocation = MeshTransform.InverseTransformPositionNoScale(TargetLocation);

				const FVector WarpedTranslation = WarpTranslation(FTransform::Identity, DeltaTranslation, TotalTranslation, TargetLocation) + ExtraRootMotion.GetLocation();
				FinalRootMotion.SetTranslation(WarpedTranslation);
			}
		}
		// if there is no translation in the animation, add it
		else
		{
			const FVector DeltaToTarget = TargetLocation - CurrentLocation;
			if (DeltaToTarget.IsNearlyZero())
			{
				FinalRootMotion.SetTranslation(FVector::ZeroVector);
			}
			else
			{
				float Alpha = FMath::Clamp((CurrentPosition - ActualStartTime) / (EndTime - ActualStartTime), 0.f, 1.f);
				Alpha = FAlphaBlend::AlphaToBlendOption(Alpha, AddTranslationEasingFunc, AddTranslationEasingCurve);

				const FVector NextLocation = FMath::Lerp<FVector, float>(StartTransform.GetLocation(), TargetLocation, Alpha);
				FVector FinalDeltaTranslation = (NextLocation - CurrentLocation);
				FinalDeltaTranslation = (CurrentRotation.Inverse() * DeltaToTarget.ToOrientationQuat()).GetForwardVector() * FinalDeltaTranslation.Size();
				FinalDeltaTranslation = CharacterOwner->GetBaseRotationOffset().UnrotateVector(FinalDeltaTranslation);

				FinalRootMotion.SetTranslation(FinalDeltaTranslation + ExtraRootMotion.GetLocation());
			}
		}
	}

	if (bWarpRotation)
	{
		const FQuat WarpedRotation = ExtraRootMotion.GetRotation() * WarpRotationOverride(RootMotionDelta, RootMotionTotal, DeltaSeconds);
		FinalRootMotion.SetRotation(WarpedRotation);
	}

	// Debug
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	const int32 DebugLevel = FMotionWarpingCVars::CVarMotionWarpingDebug.GetValueOnGameThread();
	if (DebugLevel == 1 || DebugLevel == 3)
	{
		PrintLog(TEXT("SkewWarp"), InRootMotion, FinalRootMotion);
	}

	if (DebugLevel == 2 || DebugLevel == 3)
	{
		const float DrawDebugDuration = FMotionWarpingCVars::CVarMotionWarpingDrawDebugDuration.GetValueOnGameThread();
#if ENABLE_DRAW_DEBUG
		DrawDebugCoordinateSystem(CharacterOwner->GetWorld(), GetTargetLocation(), GetTargetRotator(), 50.f, false, DrawDebugDuration, 0, 1.f);
#endif
		
	}
#endif

	return FinalRootMotion;
}

FQuat URootMotionModifier_SkewWarp_Base::WarpRotationOverride(const FTransform& RootMotionDelta, const FTransform& RootMotionTotal, float DeltaSeconds)
{
	const ACharacter* CharacterOwner = GetCharacterOwner();
	if (CharacterOwner == nullptr)
	{
		return FQuat::Identity;
	}

	const FQuat TotalRootMotionRotation = RootMotionTotal.GetRotation();
	const FQuat CurrentRotation = CharacterOwner->GetActorQuat() * CharacterOwner->GetBaseRotationOffset();
	const FQuat TargetRotation = CurrentRotation.Inverse() * (GetTargetRotationOverride() * CharacterOwner->GetBaseRotationOffset());
	const float TimeRemaining = (EndTime - PreviousPosition) * WarpRotationTimeMultiplier;
	const float Alpha = FMath::Clamp(DeltaSeconds / TimeRemaining, 0.f, 1.f);
	const FQuat TargetRotThisFrame = FQuat::Slerp(TotalRootMotionRotation, TargetRotation, Alpha);
	const FQuat DeltaOut = TargetRotThisFrame * TotalRootMotionRotation.Inverse();
	return (DeltaOut * RootMotionDelta.GetRotation());
}

FQuat URootMotionModifier_SkewWarp_Base::GetTargetRotationOverride() const
{
	if (RotationType == EMotionWarpRotationType::Default)
	{
		return CachedTargetTransform.GetRotation();
	}
	else if (RotationType == EMotionWarpRotationType::Facing)
	{
		if (const ACharacter* CharacterOwner = GetCharacterOwner())
		{
			const FTransform& CharacterTransform = CharacterOwner->GetActorTransform();
			FVector ToSyncPoint = (CachedTargetTransform.GetLocation() - CharacterTransform.GetLocation()).GetSafeNormal2D();
			const UMotionWarpingComponent* OwnerComp = GetOwnerComponent();
			if (OwnerComp)
			{
				const FMotionWarpingTarget* WarpTargetPtr = OwnerComp->FindWarpTarget(WarpTargetName);
				if (WarpTargetPtr != nullptr)
				{
					FTransform WarpPointTransformGame = WarpTargetPtr->GetTargetTrasform();
					ToSyncPoint = (WarpPointTransformGame.GetLocation() - CharacterTransform.GetLocation()).GetSafeNormal2D();
				}
			}
			return FRotationMatrix::MakeFromXZ(ToSyncPoint, FVector::UpVector).ToQuat();
		}
	}

	return FQuat::Identity;
}
