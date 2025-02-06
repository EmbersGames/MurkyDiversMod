// Fill out your copyright notice in the Description page of Project Settings.


#include "LyraCharacter.h"

#include "LyraCharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "GAS_Utility/Components/CharacterAttributeManagerComp.h"
#include "Net/UnrealNetwork.h"

static FName NAME_LyraCharacterCollisionProfile_Capsule(TEXT("LyraPawnCapsule"));
static FName NAME_LyraCharacterCollisionProfile_Mesh(TEXT("LyraPawnMesh"));

FName ALyraCharacter::CharacterAttributeManagerComponentName(TEXT("CharacterAttributeManager"));

ALyraCharacter::ALyraCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<ULyraCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	
	CharacterAttributeManager = CreateDefaultSubobject<UCharacterAttributeManagerComp>(ALyraCharacter::CharacterAttributeManagerComponentName);
	CharacterAttributeManagerBP_EDIT = CharacterAttributeManager;

	
	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	check(CapsuleComp);
	CapsuleComp->InitCapsuleSize(40.0f, 90.0f);
	CapsuleComp->SetCollisionProfileName(NAME_LyraCharacterCollisionProfile_Capsule);

	USkeletalMeshComponent* MeshComp = GetMesh();
	check(MeshComp);
	MeshComp->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f)); // Rotate mesh to be X forward since it is exported as Y forward.
	MeshComp->SetCollisionProfileName(NAME_LyraCharacterCollisionProfile_Mesh);

	
	ULyraCharacterMovementComponent* LyraMoveComp = CastChecked<ULyraCharacterMovementComponent>(GetCharacterMovement());

}

void ALyraCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME_CONDITION(ThisClass, ReplicatedAcceleration, COND_SimulatedOnly);
	//DOREPLIFETIME(ThisClass, MyTeamID)
	DOREPLIFETIME(ThisClass, PlayerUniqueIndex)
}