// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraArmAttributeManagerComp.h"

#include "CameraArmAttributeSet.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"

void UCameraArmAttributeManagerComp::BeginPlay()
{
	Super::BeginPlay();
	bAttributesBinded = false;
	InitAttributeBinding();
}

void UCameraArmAttributeManagerComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bAttributesBinded)
		InitAttributeBinding();
}

void UCameraArmAttributeManagerComp::InitAttributeBinding()
{
	if (APawn* PawnOwner = GetPawn<APawn>())
	{
		if (const auto CharacterOwner = Cast<ACharacter>(PawnOwner))
		{
			const auto LSpringArmComponent = CharacterOwner->GetComponentByClass(USpringArmComponent::StaticClass());
			if (!LSpringArmComponent)
				return;
			SpringArmComponent = Cast<USpringArmComponent>(LSpringArmComponent);

			const auto LCameraComponent = CharacterOwner->GetComponentByClass(UCameraComponent::StaticClass());
			if (!LCameraComponent)
				return;
			CameraComponent = Cast<UCameraComponent>(LCameraComponent);

			const auto CastedController = Cast<IAbilitySystemInterface>(PawnOwner);
			if (UAbilitySystemComponent* AbilitySystemComp = CastedController->GetAbilitySystemComponent())
			{
				AbilitySystemComp->GetGameplayAttributeValueChangeDelegate(UCameraArmAttributeSet::GetFovAttribute()).AddUObject(this, &UCameraArmAttributeManagerComp::Fov_AttributeChanged);
				AbilitySystemComp->GetGameplayAttributeValueChangeDelegate(UCameraArmAttributeSet::GetArmLengthAttribute()).AddUObject(this, &UCameraArmAttributeManagerComp::ArmLength_AttributeChanged);
				AbilitySystemComp->GetGameplayAttributeValueChangeDelegate(UCameraArmAttributeSet::GetSocketOffsetXAttribute()).AddUObject(this, &UCameraArmAttributeManagerComp::SocketOffsetX_AttributeChanged);
				AbilitySystemComp->GetGameplayAttributeValueChangeDelegate(UCameraArmAttributeSet::GetSocketOffsetYAttribute()).AddUObject(this, &UCameraArmAttributeManagerComp::SocketOffsetY_AttributeChanged);
				AbilitySystemComp->GetGameplayAttributeValueChangeDelegate(UCameraArmAttributeSet::GetSocketOffsetZAttribute()).AddUObject(this, &UCameraArmAttributeManagerComp::SocketOffsetZ_AttributeChanged);
				AbilitySystemComp->GetGameplayAttributeValueChangeDelegate(UCameraArmAttributeSet::GetTargetOffsetXAttribute()).AddUObject(this, &UCameraArmAttributeManagerComp::TargetOffsetX_AttributeChanged);
				AbilitySystemComp->GetGameplayAttributeValueChangeDelegate(UCameraArmAttributeSet::GetTargetOffsetYAttribute()).AddUObject(this, &UCameraArmAttributeManagerComp::TargetOffsetY_AttributeChanged);
				AbilitySystemComp->GetGameplayAttributeValueChangeDelegate(UCameraArmAttributeSet::GetTargetOffsetZAttribute()).AddUObject(this, &UCameraArmAttributeManagerComp::TargetOffsetZ_AttributeChanged);
				AbilitySystemComp->GetGameplayAttributeValueChangeDelegate(UCameraArmAttributeSet::GetMaxPitchAttribute()).AddUObject(this, &UCameraArmAttributeManagerComp::MaxPitch_AttributeChanged);
				AbilitySystemComp->GetGameplayAttributeValueChangeDelegate(UCameraArmAttributeSet::GetMinPitchAttribute()).AddUObject(this, &UCameraArmAttributeManagerComp::MinPitch_AttributeChanged);
				AbilitySystemComp->GetGameplayAttributeValueChangeDelegate(UCameraArmAttributeSet::GetRollAttribute()).AddUObject(this, &UCameraArmAttributeManagerComp::Roll_AttributeChanged);
				AbilitySystemComp->GetGameplayAttributeValueChangeDelegate(UCameraArmAttributeSet::GetLookUpRateAttribute()).AddUObject(this, &UCameraArmAttributeManagerComp::LookUpRate_AttributeChanged);
				AbilitySystemComp->GetGameplayAttributeValueChangeDelegate(UCameraArmAttributeSet::GetTurnRateAttribute()).AddUObject(this, &UCameraArmAttributeManagerComp::TurnRate_AttributeChanged);
				AbilitySystemComp->GetGameplayAttributeValueChangeDelegate(UCameraArmAttributeSet::GetOrientRotationToMovementAttribute()).AddUObject(this, &UCameraArmAttributeManagerComp::OrientRotationToMovement_AttributeChanged);
				AbilitySystemComp->GetGameplayAttributeValueChangeDelegate(UCameraArmAttributeSet::GetCameraLagAttribute()).AddUObject(this, &UCameraArmAttributeManagerComp::CameraLag_AttributeChanged);
				AbilitySystemComp->GetGameplayAttributeValueChangeDelegate(UCameraArmAttributeSet::GetCameraLagRotationAttribute()).AddUObject(this, &UCameraArmAttributeManagerComp::CameraLagRotation_AttributeChanged);
				bAttributesBinded = true;
			}
		}
	}
}

void UCameraArmAttributeManagerComp::Fov_AttributeChanged(const FOnAttributeChangeData& Data)
{
	Fov_AttributeChangeEvent(Data.OldValue, Data.NewValue);
	if (CameraComponent.IsValid())
		CameraComponent->SetFieldOfView(Data.NewValue);
}

void UCameraArmAttributeManagerComp::ArmLength_AttributeChanged(const FOnAttributeChangeData& Data)
{
	ArmLength_AttributeChangeEvent(Data.OldValue, Data.NewValue);

	if (SpringArmComponent.IsValid())
		SpringArmComponent->TargetArmLength = Data.NewValue;
}

void UCameraArmAttributeManagerComp::SocketOffsetX_AttributeChanged(const FOnAttributeChangeData& Data)
{
	SocketOffsetX_AttributeChangeEvent(Data.OldValue, Data.NewValue);

	if (SpringArmComponent.IsValid())
		SpringArmComponent->SocketOffset.X = Data.NewValue;
}

void UCameraArmAttributeManagerComp::SocketOffsetY_AttributeChanged(const FOnAttributeChangeData& Data)
{
	SocketOffsetY_AttributeChangeEvent(Data.OldValue, Data.NewValue);

	if (SpringArmComponent.IsValid())
		SpringArmComponent->SocketOffset.Y = Data.NewValue;
}

void UCameraArmAttributeManagerComp::SocketOffsetZ_AttributeChanged(const FOnAttributeChangeData& Data)
{
	SocketOffsetZ_AttributeChangeEvent(Data.OldValue, Data.NewValue);

	if (SpringArmComponent.IsValid())
		SpringArmComponent->SocketOffset.Z = Data.NewValue;
}

void UCameraArmAttributeManagerComp::TargetOffsetX_AttributeChanged(const FOnAttributeChangeData& Data)
{
	TargetOffsetX_AttributeChangeEvent(Data.OldValue, Data.NewValue);

	if (SpringArmComponent.IsValid())
		SpringArmComponent->TargetOffset.X = Data.NewValue;
}

void UCameraArmAttributeManagerComp::TargetOffsetY_AttributeChanged(const FOnAttributeChangeData& Data)
{
	TargetOffsetY_AttributeChangeEvent(Data.OldValue, Data.NewValue);

	if (SpringArmComponent.IsValid())
		SpringArmComponent->TargetOffset.Y = Data.NewValue;
}

void UCameraArmAttributeManagerComp::TargetOffsetZ_AttributeChanged(const FOnAttributeChangeData& Data)
{
	TargetOffsetZ_AttributeChangeEvent(Data.OldValue, Data.NewValue);

	if (SpringArmComponent.IsValid())
		SpringArmComponent->TargetOffset.Z = Data.NewValue;
}

void UCameraArmAttributeManagerComp::MaxPitch_AttributeChanged(const FOnAttributeChangeData& Data)
{
	MaxPitch_AttributeChangeEvent(Data.OldValue, Data.NewValue);

	//if(SpringArmComponent.IsValid())
	//SpringArmComponent->MaxPitch = Data.NewValue;
}

void UCameraArmAttributeManagerComp::MinPitch_AttributeChanged(const FOnAttributeChangeData& Data)
{
	MinPitch_AttributeChangeEvent(Data.OldValue, Data.NewValue);
	
	//if(SpringArmComponent.IsValid())
	//SpringArmComponent->MaxPitch = Data.NewValue;
}

void UCameraArmAttributeManagerComp::Roll_AttributeChanged(const FOnAttributeChangeData& Data)
{
	Roll_AttributeChangeEvent(Data.OldValue, Data.NewValue);

	//if(SpringArmComponent.IsValid())
	//SpringArmComponent->roll.X = Data.NewValue;
}

void UCameraArmAttributeManagerComp::LookUpRate_AttributeChanged(const FOnAttributeChangeData& Data)
{
	LookUpRate_AttributeChangeEvent(Data.OldValue, Data.NewValue);

	//if(SpringArmComponent.IsValid())
	//SpringArmComponent->LookUpRate = Data.NewValue;
}

void UCameraArmAttributeManagerComp::TurnRate_AttributeChanged(const FOnAttributeChangeData& Data)
{
	TurnRate_AttributeChangeEvent(Data.OldValue, Data.NewValue);
	//if(SpringArmComponent.IsValid())
	//SpringArmComponent->TurnRate = Data.NewValue;
}

void UCameraArmAttributeManagerComp::OrientRotationToMovement_AttributeChanged(const FOnAttributeChangeData& Data)
{
	OrientRotationToMovement_AttributeChangeEvent(Data.OldValue, Data.NewValue);

	//if(SpringArmComponent.IsValid())
	//SpringArmComponent->OrientRotationToMovement = Data.NewValue >= 0.1f;
}

void UCameraArmAttributeManagerComp::CameraLagRotation_AttributeChanged(const FOnAttributeChangeData& Data)
{
	CameraLagRotation_AttributeChangeEvent(Data.OldValue, Data.NewValue);
	
	if (SpringArmComponent.IsValid())
		SpringArmComponent->bEnableCameraRotationLag = Data.NewValue >= 0.1;
}

void UCameraArmAttributeManagerComp::CameraLag_AttributeChanged(const FOnAttributeChangeData& Data)
{
	CameraLag_AttributeChangeEvent(Data.OldValue, Data.NewValue);
	
	if (SpringArmComponent.IsValid())
		SpringArmComponent->bEnableCameraLag = Data.NewValue >= 0.1;
}
