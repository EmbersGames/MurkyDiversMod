// Fill out your copyright notice in the Description page of Project Settings.


#include "AsyncTasks/Task_WaitEnhancedInputAction.h"
#include "GameFramework/PlayerController.h"


UTask_WaitEnhancedInputAction* UTask_WaitEnhancedInputAction::WaitEnhancedInputAction(UGameplayAbility* OwningAbility
                                                                                      , class UInputMappingContext* InMappingContext
                                                                                      , class UInputAction* InInputAction
                                                                                      , FName TaskInstanceName)
{
	UTask_WaitEnhancedInputAction* MyObj = NewAbilityTask<UTask_WaitEnhancedInputAction>(OwningAbility, TaskInstanceName);
	MyObj->MappingContext = InMappingContext;
	MyObj->InputAction = InInputAction;

	return MyObj;
}

void UTask_WaitEnhancedInputAction::Activate()
{
	Super::Activate();
	if (AbilitySystemComponent->GetNetMode() == ENetMode::NM_DedicatedServer)
	{
		return;
	}

	const APawn* Pawn = Cast<APawn>(GetAvatarActor());

	const APlayerController* PC = Pawn->GetController<APlayerController>();
	check(PC);

	const ULocalPlayer* LP = PC->GetLocalPlayer();
	check(LP);
	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);
	FModifyContextOptions Context;
	Context.bForceImmediately = true;
	Context.bIgnoreAllPressedKeysUntilRelease = false;
	Subsystem->AddMappingContext(MappingContext, 100, Context);

	UEnhancedInputComponent* ArcIC = Pawn->FindComponentByClass<UEnhancedInputComponent>();
	PressedHandle = ArcIC->BindAction(InputAction, ETriggerEvent::Started, this, &UTask_WaitEnhancedInputAction::HandleOnInputPressed).GetHandle();
	TriggeredHandle = ArcIC->BindAction(InputAction, ETriggerEvent::Triggered, this, &UTask_WaitEnhancedInputAction::HandleOnInputTriggered).GetHandle();
	ReleasedHandle = ArcIC->BindAction(InputAction, ETriggerEvent::Completed, this, &UTask_WaitEnhancedInputAction::HandleOnInputReleased).GetHandle();
}

void UTask_WaitEnhancedInputAction::OnDestroy(bool bInOwnerFinished)
{
	Super::OnDestroy(bInOwnerFinished);
	if (AbilitySystemComponent->GetNetMode() == ENetMode::NM_DedicatedServer)
	{
		return;
	}

	const APawn* Pawn = Cast<APawn>(GetAvatarActor());
	if (!IsValid(Pawn))
		return;
	const APlayerController* PC = Pawn->GetController<APlayerController>();
	if (PC == nullptr)
	{
		return;
	}
	check(PC);

	const ULocalPlayer* LP = PC->GetLocalPlayer();
	check(LP);
	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);


	UEnhancedInputComponent* ArcIC = Pawn->FindComponentByClass<UEnhancedInputComponent>();
	ArcIC->RemoveBindingByHandle(PressedHandle);
	ArcIC->RemoveBindingByHandle(TriggeredHandle);
	ArcIC->RemoveBindingByHandle(ReleasedHandle);

	FModifyContextOptions Context;
	Context.bForceImmediately = true;
	Context.bIgnoreAllPressedKeysUntilRelease = false;
	Subsystem->RemoveMappingContext(MappingContext, Context);
}

void UTask_WaitEnhancedInputAction::HandleOnInputPressed()
{
	OnInputPressed.Broadcast(0.0f);
}

void UTask_WaitEnhancedInputAction::HandleOnInputTriggered()
{
	OnInputTriggered.Broadcast(0.0f);
}

void UTask_WaitEnhancedInputAction::HandleOnInputReleased()
{
	OnInputReleased.Broadcast(0.0f);
}
