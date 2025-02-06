// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterAttributeManagerComp.h"

#include "NativeGameplayTags.h"
#include "AbilitySystemInterface.h"
#include "CharacterAttributeSet.h"
#include "GameplayTagsManager.h"
#include "GameFramework/Character.h"
#include "GAS_Utility/AbilitySystem/AbilitySystemComponentBase.h"
#include "Kismet/KismetMathLibrary.h"


UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_WalkSpeed_AnimFactor, "System.Move.Walk.MaxSpeed.AnimFactor");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Rotation_AnimFactor, "System.Move.Rotation.Rate.AnimFactor");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_WalkSpeedAndRotation_AnimFactor, "System.Move.Walk.MaxSpeed.AnimFactorWalkAndRotate");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_RotationAndWalkSpeed_AnimFactor, "System.Move.Rotation.Rate.AnimFactorWalkAndRotate");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_SlowWalkSpeedByDirection, "System.Move.Rotation.Rate.SlowWalkSpeedByDirection");

UCharacterAttributeManagerComp::UCharacterAttributeManagerComp(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	bAutoActivate = true;
	SlowGameplayChanger = nullptr;
	SlowWalk_AttributeChanger = nullptr;
	SlowRotation_AttributeChanger = nullptr;
	SlowWalkAndRotation_AttributeChangerForWalk = nullptr;
	SlowWalkAndRotation_AttributeChangerForRotation = nullptr;
	bEnableSlowWalk = true;
	bEnableSlowRotation = true;
	bEnableSlowWalkAndRotation = true;
	SlowWalk_CurveName = "SlowWalk";
	SlowRotation_CurveName = "SlowRotation";
	SlowWalkAndRotation_CurveName = "SlowWalkAndRotation";
	SlowWalkSpeedByDirection_SafeAngle = 5.0;
	SlowWalkSpeedByDirection_BlendAngle = 25.0;
	SlowWalkSpeedByDirection_Factor = 0.1;
	bSlowWalkSpeedByDirection = false;
}

void UCharacterAttributeManagerComp::BeginPlay()
{
	Super::BeginPlay();
	bAttributesBinded = false;

	if (APawn* PawnOwner = GetPawn<APawn>())
	{
		CharacterOwner = Cast<ACharacter>(PawnOwner);
		if (CharacterOwner)
		{
			CharacterMovementComponent = CharacterOwner->GetCharacterMovement();
		}
		const auto CastedController = Cast<IAbilitySystemInterface>(PawnOwner);
		if (CastedController)
			AbilitySystemComp = CastedController->GetAbilitySystemComponent();
		else
		{
			AbilitySystemComp = GetOwner()->GetComponentByClass(UAbilitySystemComponentBase::StaticClass());
		}
	}

	InitAttributeBinding();

	if (CharacterOwner.IsValid() && AbilitySystemComp.IsValid())
	{
		if (bEnableSlowWalk || bEnableSlowRotation || bEnableSlowWalkAndRotation)
		{
			SetupSlowGameplayChanger();
		}
	}
}


void UCharacterAttributeManagerComp::SetupSlowGameplayChanger()
{
	if (SlowGameplayChanger == nullptr && AbilitySystemComp && CharacterOwner)
	{
		SlowGameplayChanger = NewObject<UGameplayChangerBase>();

		SlowWalk_AttributeChanger = NewObject<UAttributeChanger>();
		SlowRotation_AttributeChanger = NewObject<UAttributeChanger>();
		SlowWalkAndRotation_AttributeChangerForWalk = NewObject<UAttributeChanger>();
		SlowWalkAndRotation_AttributeChangerForRotation = NewObject<UAttributeChanger>();

		SlowWalk_AttributeChanger->SetOperation(EAttributeChanger::Multiplicative);
		SlowRotation_AttributeChanger->SetOperation(EAttributeChanger::Multiplicative);
		SlowWalkAndRotation_AttributeChangerForWalk->SetOperation(EAttributeChanger::Multiplicative);
		SlowWalkAndRotation_AttributeChangerForRotation->SetOperation(EAttributeChanger::Multiplicative);

		SlowWalk_AttributeChanger->UniqueAttributeChangerTag = TAG_WalkSpeed_AnimFactor;
		SlowRotation_AttributeChanger->UniqueAttributeChangerTag = TAG_Rotation_AnimFactor;
		SlowWalkAndRotation_AttributeChangerForWalk->UniqueAttributeChangerTag = TAG_WalkSpeedAndRotation_AnimFactor;
		SlowWalkAndRotation_AttributeChangerForRotation->UniqueAttributeChangerTag = TAG_RotationAndWalkSpeed_AnimFactor;

		SlowWalk_AttributeChanger->SetFloatValue(1.0f);
		SlowRotation_AttributeChanger->SetFloatValue(1.0f);
		SlowWalkAndRotation_AttributeChangerForWalk->SetFloatValue(1.0f);
		SlowWalkAndRotation_AttributeChangerForRotation->SetFloatValue(1.0f);

		TMap<FGameplayAttribute, FAttributeChangerArray> Attributes;

		auto SlowWalkArray = FAttributeChangerArray();
		SlowWalkArray.ArrayList.Add(SlowWalk_AttributeChanger);
		SlowWalkArray.ArrayList.Add(SlowWalkAndRotation_AttributeChangerForWalk);

		auto SlowRotationArray = FAttributeChangerArray();
		SlowRotationArray.ArrayList.Add(SlowRotation_AttributeChanger);
		SlowRotationArray.ArrayList.Add(SlowWalkAndRotation_AttributeChangerForRotation);

		Attributes.Add(UCharacterAttributeSet::GetMaxSpeedAttribute(), SlowWalkArray);
		Attributes.Add(UCharacterAttributeSet::GetRotationRateAttribute(), SlowRotationArray);

		SlowGameplayChanger->SetAttributeChangers(Attributes);
		const auto PhareAbilitySystemComp = Cast<UAbilitySystemComponentBase>(AbilitySystemComp.Get());
		PhareAbilitySystemComp->AddGameplayChanger(SlowGameplayChanger);


		SlowWalkSpeedByDirection_AttributeChanger = PhareAbilitySystemComp->CreateLooseAttributeChangerWithData(UCharacterAttributeSet::GetMaxSpeedAttribute(), TAG_SlowWalkSpeedByDirection, EAttributeChanger::Disabled, 1.f, 100.f);
	}
}

void UCharacterAttributeManagerComp::SetSlowWalkSpeedByDirection(bool NewSlowWalkSpeedByDirection)
{
	if (NewSlowWalkSpeedByDirection)
	{
		bSlowWalkSpeedByDirection = NewSlowWalkSpeedByDirection;
		ApplySlowWalkSpeedFromDirection();
	}
	else
	{
		if (bSlowWalkSpeedByDirection && SlowWalkSpeedByDirection_AttributeChanger)
		{
			SlowWalkSpeedByDirection_AttributeChanger->SetOperation(EAttributeChanger::Disabled);
		}
		bSlowWalkSpeedByDirection = NewSlowWalkSpeedByDirection;
	}
}


void UCharacterAttributeManagerComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!AbilitySystemComp)
	{
		APawn* PawnOwner = GetPawn<APawn>();
		const auto CastedController = Cast<IAbilitySystemInterface>(PawnOwner);
		if (CastedController)
			AbilitySystemComp = CastedController->GetAbilitySystemComponent();
		else
		{
			AbilitySystemComp = GetOwner()->GetComponentByClass(UAbilitySystemComponentBase::StaticClass());
		}
	}
	if (!bAttributesBinded)
		InitAttributeBinding();

	if (CharacterOwner.IsValid() && AbilitySystemComp.IsValid())
	{
		// Slow 
		ApplySlowFromAnimation();

		SpeedAttribute = CharacterOwner->GetVelocity().Length();
		//AbilitySystemComp->SetNumericAttributeBase(UCharacterAttributeSet::GetSpeedAttribute(), SpeedAttribute);
		if (bSlowWalkSpeedByDirection)
			ApplySlowWalkSpeedFromDirection();
	}
}

void UCharacterAttributeManagerComp::ApplySlowWalkSpeedFromDirection()
{
	if (CharacterOwner.IsValid() && AbilitySystemComp.IsValid())
	{
		const FVector InputVector = CharacterOwner->Internal_GetLastMovementInputVector();
		if (InputVector.SquaredLength() > 0.0001f)
		{
			const FVector ForwardVector = CharacterOwner->GetActorForwardVector();
			const FVector InputVectorNormalize = InputVector.GetSafeNormal2D();

			const double Angle = FMath::RadiansToDegrees(FMath::Acos(ForwardVector.CosineAngle2D(InputVectorNormalize)));
			if (Angle <= SlowWalkSpeedByDirection_SafeAngle)
			{
				SlowWalkSpeedByDirection_AttributeChanger->SetOperation(EAttributeChanger::Disabled);
			}
			else
			{
				float Alpha = 1.0;
				if (SlowWalkSpeedByDirection_BlendAngle != SlowWalkSpeedByDirection_SafeAngle)
					Alpha = FMath::Clamp(UKismetMathLibrary::NormalizeToRange(Angle, SlowWalkSpeedByDirection_SafeAngle, SlowWalkSpeedByDirection_BlendAngle), 0.0, 1.0);
				SlowWalkSpeedByDirection_AttributeChanger->SetFloatValue(FMath::Lerp(1.0, SlowWalkSpeedByDirection_Factor, Alpha));
				SlowWalkSpeedByDirection_AttributeChanger->SetOperation(EAttributeChanger::Multiplicative);
			}
		}
		else
		{
			SlowWalkSpeedByDirection_AttributeChanger->SetOperation(EAttributeChanger::Disabled);
		}
	}
}

void UCharacterAttributeManagerComp::ApplySlowFromAnimation_Implementation()
{
	if (SlowGameplayChanger == nullptr)
		SetupSlowGameplayChanger();
	if (SlowGameplayChanger == nullptr)
		return;
	if (bEnableSlowWalk || bEnableSlowRotation || bEnableSlowWalkAndRotation)
	{
		if (CharacterOwner->GetMesh() && CharacterOwner->GetMesh()->GetAnimInstance())
		{
			const auto AnimInstance = CharacterOwner->GetMesh()->GetAnimInstance();
			if (bEnableSlowWalk)
			{
				float NewValue = 0.f;
				AnimInstance->GetCurveValue(SlowWalk_CurveName, NewValue);
				NewValue = FMath::Max(1.f - NewValue,0.f);
				if (NewValue != SlowWalk_AttributeChanger->GetFloatValue())
				{
					SlowWalk_AttributeChanger->SetFloatValue(NewValue);
				}
			}
			if (bEnableSlowRotation)
			{
				float NewValue = 0.f;
				AnimInstance->GetCurveValue(SlowRotation_CurveName, NewValue);
				NewValue = 1.f - NewValue;
				if (NewValue != SlowRotation_AttributeChanger->GetFloatValue())
				{
					SlowRotation_AttributeChanger->SetFloatValue(NewValue);
				}
			}
			if (bEnableSlowWalkAndRotation)
			{
				float NewValue = 0.f;
				AnimInstance->GetCurveValue(SlowWalkAndRotation_CurveName, NewValue);
				NewValue = 1.f - NewValue;
				if (NewValue != SlowWalkAndRotation_AttributeChangerForWalk->GetFloatValue())
				{
					SlowWalkAndRotation_AttributeChangerForWalk->SetFloatValue(NewValue);
				}
				if (NewValue != SlowWalkAndRotation_AttributeChangerForRotation->GetFloatValue())
				{
					SlowWalkAndRotation_AttributeChangerForRotation->SetFloatValue(NewValue);
				}
			}
		}
	}
}

void UCharacterAttributeManagerComp::Activate(bool bReset)
{
	Super::Activate(bReset);
}


void UCharacterAttributeManagerComp::InitAttributeBinding()
{
	if (CharacterOwner && AbilitySystemComp)
	{
		// Speed
		AbilitySystemComp->GetGameplayAttributeValueChangeDelegate(UCharacterAttributeSet::GetSpeedAttribute()).AddUObject(this, &UCharacterAttributeManagerComp::Speed_AttributeChanged);
		// MaxSpeed
		AbilitySystemComp->GetGameplayAttributeValueChangeDelegate(UCharacterAttributeSet::GetMaxSpeedAttribute()).AddUObject(this, &UCharacterAttributeManagerComp::MaxSpeed_AttributeChanged);
		FOnAttributeChangeData data;
		bool bFound;
		data.NewValue= AbilitySystemComp->GetGameplayAttributeValue(UCharacterAttributeSet::GetMaxSpeedAttribute(),bFound);
		MaxSpeed_AttributeChanged(data);
		// GravityScale
		AbilitySystemComp->GetGameplayAttributeValueChangeDelegate(UCharacterAttributeSet::GetGravityScaleAttribute()).AddUObject(this, &UCharacterAttributeManagerComp::GravityScale_AttributeChanged);
		// MaxAcceleration
		AbilitySystemComp->GetGameplayAttributeValueChangeDelegate(UCharacterAttributeSet::GetMaxAccelerationAttribute()).AddUObject(this, &UCharacterAttributeManagerComp::MaxAcceleration_AttributeChanged);
		// MaxStepHeight
		AbilitySystemComp->GetGameplayAttributeValueChangeDelegate(UCharacterAttributeSet::GetMaxStepHeightAttribute()).AddUObject(this, &UCharacterAttributeManagerComp::MaxStepHeight_AttributeChanged);
		// WalkableFloorAngle
		AbilitySystemComp->GetGameplayAttributeValueChangeDelegate(UCharacterAttributeSet::GetWalkableFloorAngleAttribute()).AddUObject(this, &UCharacterAttributeManagerComp::WalkableFloorAngle_AttributeChanged);
		// AirControl
		AbilitySystemComp->GetGameplayAttributeValueChangeDelegate(UCharacterAttributeSet::GetAirControlAttribute()).AddUObject(this, &UCharacterAttributeManagerComp::AirControl_AttributeChanged);
		// RotationRate
		AbilitySystemComp->GetGameplayAttributeValueChangeDelegate(UCharacterAttributeSet::GetRotationRateAttribute()).AddUObject(this, &UCharacterAttributeManagerComp::RotationRate_AttributeChanged);
		// OrientRotationToMovement
		AbilitySystemComp->GetGameplayAttributeValueChangeDelegate(UCharacterAttributeSet::GetOrientRotationToMovementAttribute()).AddUObject(this, &UCharacterAttributeManagerComp::OrientRotationToMovement_AttributeChanged);
		// RootMotionScale
		AbilitySystemComp->GetGameplayAttributeValueChangeDelegate(UCharacterAttributeSet::GetRootMotionScaleAttribute()).AddUObject(this, &UCharacterAttributeManagerComp::RootMotionScale_AttributeChanged);
		// SlowWalkSpeedByDirection
		AbilitySystemComp->GetGameplayAttributeValueChangeDelegate(UCharacterAttributeSet::GetSlowWalkSpeedByDirectionAttribute()).AddUObject(this, &UCharacterAttributeManagerComp::SlowWalkSpeedByDirection_AttributeChanged);
		bAttributesBinded = true;
	}
}

void UCharacterAttributeManagerComp::Speed_AttributeChanged(const FOnAttributeChangeData& Data)
{
	Speed_AttributeChangeEvent(Data.OldValue, Data.NewValue);
}

void UCharacterAttributeManagerComp::MaxSpeed_AttributeChanged(const FOnAttributeChangeData& Data)
{
	MaxSpeed_AttributeChangeEvent(Data.OldValue, Data.NewValue);
	if (CharacterMovementComponent.IsValid())
	{
		if (!bIsFlyingByDefault)
			CharacterMovementComponent->MaxWalkSpeed = Data.NewValue;

		else
			CharacterMovementComponent->MaxFlySpeed = Data.NewValue;
	}
}

void UCharacterAttributeManagerComp::GravityScale_AttributeChanged(const FOnAttributeChangeData& Data)
{
	GravityScale_AttributeChangeEvent(Data.OldValue, Data.NewValue);

	if (CharacterMovementComponent.IsValid())
		CharacterMovementComponent->GravityScale = Data.NewValue;
}

void UCharacterAttributeManagerComp::MaxAcceleration_AttributeChanged(const FOnAttributeChangeData& Data)
{
	MaxAcceleration_AttributeChangeEvent(Data.OldValue, Data.NewValue);

	if (CharacterMovementComponent.IsValid())
		CharacterMovementComponent->MaxAcceleration = Data.NewValue;
}

void UCharacterAttributeManagerComp::MaxStepHeight_AttributeChanged(const FOnAttributeChangeData& Data)
{
	MaxStepHeight_AttributeChangeEvent(Data.OldValue, Data.NewValue);
	if (CharacterMovementComponent.IsValid())
		CharacterMovementComponent->MaxStepHeight = Data.NewValue;
}

void UCharacterAttributeManagerComp::WalkableFloorAngle_AttributeChanged(const FOnAttributeChangeData& Data)
{
	WalkableFloorAngle_AttributeChangeEvent(Data.OldValue, Data.NewValue);

	if (CharacterMovementComponent.IsValid())
		CharacterMovementComponent->SetWalkableFloorAngle(Data.NewValue);
}

void UCharacterAttributeManagerComp::AirControl_AttributeChanged(const FOnAttributeChangeData& Data)
{
	AirControl_AttributeChangeEvent(Data.OldValue, Data.NewValue);

	if (CharacterMovementComponent.IsValid())
		CharacterMovementComponent->AirControl = Data.NewValue;
}

void UCharacterAttributeManagerComp::RotationRate_AttributeChanged(const FOnAttributeChangeData& Data)
{
	RotationRate_AttributeChangeEvent(Data.OldValue, Data.NewValue);

	if (CharacterMovementComponent.IsValid())
		CharacterMovementComponent->RotationRate.Yaw = Data.NewValue;
}

void UCharacterAttributeManagerComp::OrientRotationToMovement_AttributeChanged(const FOnAttributeChangeData& Data)
{
	OrientRotationToMovement_AttributeChangeEvent(Data.OldValue, Data.NewValue);

	if (CharacterMovementComponent.IsValid())
		CharacterMovementComponent->bOrientRotationToMovement = Data.NewValue >= 0.1f;
}

void UCharacterAttributeManagerComp::RootMotionScale_AttributeChanged(const FOnAttributeChangeData& Data)
{
	RootMotionScale_AttributeChangeEvent(Data.OldValue, Data.NewValue);

	if (CharacterMovementComponent.IsValid())
		CharacterMovementComponent->GetCharacterOwner()->SetAnimRootMotionTranslationScale(Data.NewValue);
}

void UCharacterAttributeManagerComp::SlowWalkSpeedByDirection_AttributeChanged(const FOnAttributeChangeData& Data)
{
	SlowWalkSpeedByDirection_AttributeChangeEvent(Data.OldValue, Data.NewValue);
	SetSlowWalkSpeedByDirection(Data.NewValue >= 0.1f);
}
