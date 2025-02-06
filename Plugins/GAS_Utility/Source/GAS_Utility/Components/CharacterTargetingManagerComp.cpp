// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/CharacterTargetingManagerComp.h"

#include "AbilitySystemInterface.h"
#include "CharacterAttributeSet.h"
#include "NativeGameplayTags.h"
#include "CharacterMovementComponentBase.h"
#include "GameplayTagsManager.h"
#include "Components/CapsuleComponent.h"
#include "Engine/RendererSettings.h"
#include "GameFramework/Character.h"

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_MovingBackward, "System.Move.MoveTo.MovingBackward");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_DisableOrientRotationToMovement, "System.Targeting.MovingBackward.DisableOrientRotationToMovement");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Target, "System.Targeting.Target");

UCharacterTargetingManagerComp::UCharacterTargetingManagerComp(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
	TargetActor = nullptr;
	OrientRotationToMovement_AttributeChanger = nullptr;
	OrientRotationToMovement_GameplayChanger = nullptr;
	bOrientDesiredDirectionToTarget = true;
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	bAutoActivate = true;
	WithTargetGC = nullptr;
}

void UCharacterTargetingManagerComp::BeginPlay()
{
	Super::BeginPlay();
	if (APawn* PawnOwner = GetPawn<APawn>())
	{
		CharacterOwner = Cast<ACharacter>(PawnOwner);
		if (CharacterOwner)
		{
			CharacterMovementComponent = CharacterOwner->GetCharacterMovement();
		}
		const IAbilitySystemInterface* CastedController = Cast<IAbilitySystemInterface>(PawnOwner);
		const UAbilitySystemComponent* TempAbilitySystemComp = CastedController->GetAbilitySystemComponent();
		AbilitySystemComp = Cast<UAbilitySystemComponentBase>(TempAbilitySystemComp);
	}
	InitBinding();
}

void UCharacterTargetingManagerComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	HandleMoveBackward();
	HandleOrientDesiredDirectionToTarget();
}

void UCharacterTargetingManagerComp::HandleMoveBackward()
{
	if (AbilitySystemComp && CharacterOwner)
	{
		const FGameplayTag Tag = TAG_MovingBackward;
		if (bool MovingBackward = AbilitySystemComp->HasGameplayTag(Tag))
		{
			CharacterOwner->AddMovementInput(GetMovingBackwardDirection(), 1.f, true);

			if (OrientRotationToMovement_AttributeChanger && OrientRotationToMovement_AttributeChanger->GetOperation() == EAttributeChanger::Disabled)
			{
				OrientRotationToMovement_AttributeChanger->SetOperation(EAttributeChanger::SetValue);
			}
		}
		else
		{
			if (OrientRotationToMovement_AttributeChanger && OrientRotationToMovement_AttributeChanger->GetOperation() != EAttributeChanger::Disabled)
			{
				OrientRotationToMovement_AttributeChanger->SetOperation(EAttributeChanger::Disabled);
			}
		}
	}
}

void UCharacterTargetingManagerComp::HandleOrientDesiredDirectionToTarget()
{
	if (bOrientDesiredDirectionToTarget && HaveCharacterTarget() && CharacterMovementComponent.IsValid())
	{
		auto CharMovCast = Cast<UCharacterMovementComponentBase>(CharacterMovementComponent.Get());
		if (CharMovCast)
		{
			CharMovCast->SetDesiredDirection(GetDirection2DToTarget());
		}
	}
}

FVector UCharacterTargetingManagerComp::GetMovingBackwardDirection_Implementation()
{
	if (CharacterOwner)
		return CharacterOwner->GetActorForwardVector() * -1.f;
	return FVector::ZeroVector;
}

FVector UCharacterTargetingManagerComp::GetTargetLocation_Implementation()
{
	if(IsValid(TargetActor))
		return TargetActor->GetActorLocation();
	return FVector::ZeroVector;
}

float UCharacterTargetingManagerComp::GetAngleToTarget_Implementation()
{
	if (HaveCharacterTarget() && CharacterOwner.IsValid())
	{
		const FVector DirectionToTarget = (GetTargetLocation() - CharacterOwner->GetActorLocation()).GetSafeNormal2D();
		return FMath::RadiansToDegrees(FMath::Acos(CharacterOwner->GetActorForwardVector().CosineAngle2D(DirectionToTarget)));
	}
	return -1.f;
}

float UCharacterTargetingManagerComp::GetSignedAngleToTarget_Implementation()
{
	if (HaveCharacterTarget() && CharacterOwner.IsValid())
	{
		const FVector DirectionToTarget = (GetTargetLocation() - CharacterOwner->GetActorLocation()).GetSafeNormal2D();
		const float Angle = FMath::RadiansToDegrees(FMath::Acos(CharacterOwner->GetActorForwardVector().CosineAngle2D(DirectionToTarget)));
		if (FMath::RadiansToDegrees(FMath::Acos(CharacterOwner->GetActorRightVector().CosineAngle2D(DirectionToTarget))) <= 90.f)
		{
			return Angle;
		}
		return Angle * -1.f;
	}
	return -1.f;
}

FVector UCharacterTargetingManagerComp::GetDirectionToTarget_Implementation()
{
	if (!CharacterOwner.IsValid())
		return FVector::ZeroVector;

	if (!HaveCharacterTarget())
		return CharacterOwner->GetActorForwardVector();

	return (GetTargetLocation() - CharacterOwner->GetActorLocation()).GetSafeNormal();
}

FVector UCharacterTargetingManagerComp::GetDirection2DToTarget_Implementation()
{
	if (!CharacterOwner.IsValid())
		return FVector::ZeroVector;

	if (!HaveCharacterTarget())
		return CharacterOwner->GetActorForwardVector();

	return (GetTargetLocation() - CharacterOwner->GetActorLocation()).GetSafeNormal2D();
}

float UCharacterTargetingManagerComp::GetCapsuleDistanceToTarget_Implementation()
{
	if (HaveCharacterTarget() && CharacterOwner && CharacterOwner->GetCapsuleComponent())
	{
		const FVector DistanceVector = (GetTargetLocation() - CharacterOwner->GetActorLocation());
		return DistanceVector.Size() - CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleRadius();
	}
	return -1.f;
}

float UCharacterTargetingManagerComp::GetCapsuleDistance2DToTarget_Implementation()
{
	if (HaveCharacterTarget() && CharacterOwner.IsValid() && CharacterOwner->GetCapsuleComponent())
	{
		const FVector DistanceVector = (GetTargetLocation() - CharacterOwner->GetActorLocation());
		return DistanceVector.Size2D() - CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleRadius();
	}
	return -1.f;
}

float UCharacterTargetingManagerComp::GetDistanceToTarget_Implementation()
{
	if (HaveCharacterTarget() && CharacterOwner.IsValid())
	{
		const FVector DistanceVector = (GetTargetLocation() - CharacterOwner->GetActorLocation());
		return DistanceVector.Size();
	}
	return -1.f;
}

float UCharacterTargetingManagerComp::GetDistance2DToTarget_Implementation()
{
	if (HaveCharacterTarget() && CharacterOwner.IsValid())
	{
		const FVector DistanceVector = (GetTargetLocation() - CharacterOwner->GetActorLocation());
		return DistanceVector.Size2D();
	}
	return -1.f;
}

AActor* UCharacterTargetingManagerComp::GetCharacterTarget_Implementation()
{
	return TargetActor;
}

bool UCharacterTargetingManagerComp::HaveCharacterTarget_Implementation()
{
	if (IsValid(TargetActor))
	{
		return true;
	}
	return false;
}

void UCharacterTargetingManagerComp::InitBinding()
{
	if (AbilitySystemComp)
	{
		const FGameplayTag Tag = TAG_Target;
		FOnStoredObjectChange* Delegate = AbilitySystemComp->GetStoredObjectChangeDelegates(Tag);
		if (Delegate)
			Delegate->AddDynamic(this, &UCharacterTargetingManagerComp::TargetObjectChange);

		if (UObject* Object = AbilitySystemComp->GetFirstObjectStoredWithGameplayTag(Tag))
		{
			TargetActor = Cast<AActor>(Object);
		}
		if (!OrientRotationToMovement_GameplayChanger)
		{
			// Construct GameplayChanger
			OrientRotationToMovement_GameplayChanger = NewObject<UGameplayChangerBase>();

			// Construct AttributeChanger
			OrientRotationToMovement_AttributeChanger = NewObject<UAttributeChanger>();
			OrientRotationToMovement_AttributeChanger->SetOperation(EAttributeChanger::Disabled);
			OrientRotationToMovement_AttributeChanger->UniqueAttributeChangerTag = TAG_DisableOrientRotationToMovement;
			OrientRotationToMovement_AttributeChanger->SetFloatValue(0.0f);
			OrientRotationToMovement_AttributeChanger->SetPriorityValue(10.0f);

			// Link Attribute changer and gameplay changer
			TMap<FGameplayAttribute, FAttributeChangerArray> Attributes;
			auto OrientRotationToMovementArray = FAttributeChangerArray();
			OrientRotationToMovementArray.ArrayList.Add(OrientRotationToMovement_AttributeChanger);
			Attributes.Add(UCharacterAttributeSet::GetOrientRotationToMovementAttribute(), OrientRotationToMovementArray);
			OrientRotationToMovement_GameplayChanger->SetAttributeChangers(Attributes);

			// Add gameplay changer to Ability sys comp
			AbilitySystemComp->AddGameplayChanger(OrientRotationToMovement_GameplayChanger);
		}
		AbilitySystemComp->GetGameplayAttributeValueChangeDelegate(UCharacterAttributeSet::GetOrientDesiredDirectionToTargetAttribute()).AddUObject(this, &UCharacterTargetingManagerComp::OrientDesiredDirectionToTarget_AttributeChanged);
		bOrientDesiredDirectionToTarget = AbilitySystemComp->GetAttribute(UCharacterAttributeSet::GetOrientDesiredDirectionToTargetAttribute()) > 0.1f;
		
	}
}

void UCharacterTargetingManagerComp::TargetObjectChange(FGameplayTag Tag)
{
	if (AbilitySystemComp)
	{
		if (UObject* Object = AbilitySystemComp->GetFirstObjectStoredWithGameplayTag(Tag))
		{
			TargetActor = Cast<AActor>(Object);
			if(IsValid(WithTargetGC_Class) && AbilitySystemComp)
			{
				if(!IsValid(WithTargetGC))
				{
					WithTargetGC = AbilitySystemComp->AddGameplayChangerByClass(WithTargetGC_Class);
				}
			}
		}
		else
		{
			TargetActor = nullptr;
			if(IsValid(WithTargetGC_Class) && AbilitySystemComp)
			{
				if(IsValid(WithTargetGC))
				{
					AbilitySystemComp->RemoveGameplayChanger(WithTargetGC);
					WithTargetGC = nullptr;
				}
			}
		}
	}
}

void UCharacterTargetingManagerComp::OrientDesiredDirectionToTarget_AttributeChanged(const FOnAttributeChangeData& Data)
{
	bOrientDesiredDirectionToTarget = Data.NewValue > 0.1f;
}
