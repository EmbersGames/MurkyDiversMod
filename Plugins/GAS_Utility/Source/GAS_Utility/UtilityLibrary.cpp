// Fill out your copyright notice in the Description page of Project Settings.


#include "UtilityLibrary.h"


UClass* UUtilityLibrary::LoadSoftClassFromPath(const FString& Path)
{
	if (Path.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("[Mod] LoadSoftClassFromPath: Path is empty."));
		return nullptr;
	}

	FSoftClassPath SoftClassPath(Path);

	if (!SoftClassPath.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("[Mod] LoadSoftClassFromPath: Invalid SoftClassPath: %s"), *Path);
		return nullptr;
	}

	UClass* Class = SoftClassPath.TryLoadClass<UObject>();
	if (!Class)
	{
		UE_LOG(LogTemp, Error, TEXT("[Mod] LoadSoftClassFromPath: Failed to load class from path: %s"), *Path);
		return nullptr;
	}

	UE_LOG(LogTemp, Log, TEXT("[Mod] LoadSoftClassFromPath: Successfully loaded class: %s"), *Path);
	return Class;
}