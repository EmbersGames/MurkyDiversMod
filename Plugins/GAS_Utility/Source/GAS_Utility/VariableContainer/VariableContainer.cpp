// Fill out your copyright notice in the Description page of Project Settings.


#include "VariableContainer/VariableContainer.h"

#include "Net/UnrealNetwork.h"

/*
UVariableContainer::UVariableContainer(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
}*/

UVariableContainer::UVariableContainer(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
}

FString UVariableContainer::GetFieldTypeString(const FString& FieldName) const
{
	const FGAS_Utility_Variable* Result = Variables.Find(FieldName);
	if (Result)
	{
		switch (Result->LastTypeSet)
		{
		case EGAS_Utility_VariableType::Variable_Bool:
			return "Boolean";
		case EGAS_Utility_VariableType::Variable_Number:
			return "Number";
		case EGAS_Utility_VariableType::Variable_String:
			return "String";
		case EGAS_Utility_VariableType::Variable_Object:
			return "Object";
		case EGAS_Utility_VariableType::Variable_Vector:
			return "Vector";
		case EGAS_Utility_VariableType::Variable_Null:
		default:
			return "NULL";
		}
	}
	return "NULL";
}

TArray<FString> UVariableContainer::GetFieldNames()
{
	TArray<FString> Result;
	Variables.GenerateKeyArray(Result);
	return Result;
}

bool UVariableContainer::HasField(const FString& FieldName) const
{
	return Variables.Contains(FieldName);
}

void UVariableContainer::RemoveField(const FString& FieldName)
{
	Variables.Remove(FieldName);
}

int UVariableContainer::Num()
{
	return Variables.Num();
}

float UVariableContainer::GetNumberField(const FString& FieldName) const
{
	const FGAS_Utility_Variable* Result = Variables.Find(FieldName);
	if (Result)
	{
		return Result->GetDoubleValue();
	}
	return 0.f;
}

void UVariableContainer::SetNumberField(const FString& FieldName, float Number)
{
	FGAS_Utility_Variable* Result = Variables.Find(FieldName);
	if (Result)
	{
		Result->SetDoubleValue(Number);
	}
	else
	{
		Variables.Add(FieldName, FGAS_Utility_Variable(Number));
	}
}

void UVariableContainer::SetNumberDoubleField(const FString& FieldName, double Number)
{
	FGAS_Utility_Variable* Result = Variables.Find(FieldName);
	if (Result)
	{
		Result->SetDoubleValue(Number);
	}
	else
	{
		Variables.Add(FieldName, FGAS_Utility_Variable(Number));
	}
}


void UVariableContainer::SetVectorField(const FString& FieldName, FVector Vector)
{
	FGAS_Utility_Variable* Result = Variables.Find(FieldName);
	if (Result)
	{
		Result->SetVectorValue(Vector);
	}
	else
	{
		Variables.Add(FieldName, FGAS_Utility_Variable(Vector));
	}
}

double UVariableContainer::GetNumberDoubleField(const FString& FieldName) const
{
	const FGAS_Utility_Variable* Result = Variables.Find(FieldName);
	if (Result)
	{
		return Result->GetDoubleValue();
	}
	return 0.f;
}

FVector UVariableContainer::GetVectorField(const FString& FieldName) const
{
	const FGAS_Utility_Variable* Result = Variables.Find(FieldName);
	if (Result)
	{
		return Result->GetVectorValue();
	}
	return FVector::ZeroVector;
}


int32 UVariableContainer::GetIntegerField(const FString& FieldName) const
{
	const FGAS_Utility_Variable* Result = Variables.Find(FieldName);
	if (Result)
	{
		return Result->GetDoubleValue();
	}
	return 0;
}

void UVariableContainer::SetIntegerField(const FString& FieldName, int32 Number)
{
	FGAS_Utility_Variable* Result = Variables.Find(FieldName);
	if (Result)
	{
		Result->SetDoubleValue(Number);
	}
	else
	{
		Variables.Add(FieldName, FGAS_Utility_Variable((double)Number));
	}
}

int64 UVariableContainer::GetInt64Field(const FString& FieldName) const
{
	const FGAS_Utility_Variable* Result = Variables.Find(FieldName);
	if (Result)
	{
		return Result->GetDoubleValue();
	}
	return 0;
}

void UVariableContainer::SetInt64Field(const FString& FieldName, int64 Number)
{
	FGAS_Utility_Variable* Result = Variables.Find(FieldName);
	if (Result)
	{
		Result->SetDoubleValue(Number);
	}
	else
	{
		Variables.Add(FieldName, FGAS_Utility_Variable((double)Number));
	}
}

FString UVariableContainer::GetStringField(const FString& FieldName) const
{
	const FGAS_Utility_Variable* Result = Variables.Find(FieldName);
	if (Result)
	{
		return Result->GetStringValue();
	}
	return "";
}

void UVariableContainer::SetStringField(const FString& FieldName, const FString& StringValue)
{
	FGAS_Utility_Variable* Result = Variables.Find(FieldName);
	if (Result)
	{
		Result->SetStringValue(StringValue);
	}
	else
	{
		Variables.Add(FieldName, FGAS_Utility_Variable(StringValue));
	}
}

bool UVariableContainer::GetBoolField(const FString& FieldName) const
{
	const FGAS_Utility_Variable* Result = Variables.Find(FieldName);
	if (Result)
	{
		return Result->GetBoolValue();
	}
	return false;
}

void UVariableContainer::SetBoolField(const FString& FieldName, bool InValue)
{
	FGAS_Utility_Variable* Result = Variables.Find(FieldName);
	if (Result)
	{
		Result->SetBoolValue(InValue);
	}
	else
	{
		Variables.Add(FieldName, FGAS_Utility_Variable(InValue));
	}
}

UObject* UVariableContainer::GetObjectField(const FString& FieldName) const
{
	const FGAS_Utility_Variable* Result = Variables.Find(FieldName);
	if (Result)
	{
		return Result->GetObjectValue().Get();
	}
	return nullptr;
}

void UVariableContainer::SetObjectField(const FString& FieldName, UObject* Object)
{
	FGAS_Utility_Variable* Result = Variables.Find(FieldName);
	if (Result)
	{
		Result->SetObjectValue(Object);
	}
	else
	{
		Variables.Add(FieldName, FGAS_Utility_Variable(TWeakObjectPtr<UObject>(Object)));
	}
}

void UVariableContainer::SetTagField(const FString& FieldName, FGameplayTag Tag)
{
	FGAS_Utility_Variable* Result = Variables.Find(FieldName);
	if (Result)
	{
		Result->SetTagValue(Tag);
	}
	else
	{
		Variables.Add(FieldName, FGAS_Utility_Variable(Tag));
	}
}

FGameplayTag UVariableContainer::GetTagField(const FString& FieldName) const
{
	const FGAS_Utility_Variable* Result = Variables.Find(FieldName);
	if (Result)
	{
		return Result->GetTagValue();
	}
	return FGameplayTag();
}

void UVariableContainer::SetMapFields_Tag(const TMap<FString, FGameplayTag>& Fields)
{
	for (auto field : Fields)
	{
		SetTagField(field.Key, field.Value);
	}
}

void UVariableContainer::SetMapFields_string(const TMap<FString, FString>& Fields)
{
	for (auto field : Fields)
	{
		SetStringField(field.Key, field.Value);
	}
}

void UVariableContainer::SetMapFields_uint8(const TMap<FString, uint8>& Fields)
{
	for (auto field : Fields)
	{
		SetNumberField(field.Key, field.Value);
	}
}

void UVariableContainer::SetMapFields_int32(const TMap<FString, int32>& Fields)
{
	for (auto field : Fields)
	{
		SetNumberField(field.Key, field.Value);
	}
}

void UVariableContainer::SetMapFields_int64(const TMap<FString, int64>& Fields)
{
	for (auto field : Fields)
	{
		SetNumberField(field.Key, field.Value);
	}
}

void UVariableContainer::SetMapFields_bool(const TMap<FString, bool>& Fields)
{
	for (auto field : Fields)
	{
		SetBoolField(field.Key, field.Value);
	}
}

TArray<float> UVariableContainer::GetNumberArrayField(const FString& FieldName) const
{
	auto ArrayRes = TArray<float>();
	const FGAS_Utility_Variable* Result = Variables.Find(FieldName);
	if (Result)
	{
		for (auto NumberField : Result->GetDoubleValues())
		{
			ArrayRes.Add(NumberField);
		}
	}
	return ArrayRes;
}

TArray<FVector> UVariableContainer::GetVectorArrayField(const FString& FieldName) const
{
	auto ArrayRes = TArray<FVector>();
	const FGAS_Utility_Variable* Result = Variables.Find(FieldName);
	if (Result)
	{
		for (auto NumberField : Result->GetVectorValues())
		{
			ArrayRes.Add(NumberField);
		}
	}
	return ArrayRes;
}

TArray<int32> UVariableContainer::GetIntegerArrayField(const FString& FieldName) const
{
	auto ArrayRes = TArray<int32>();
	const FGAS_Utility_Variable* Result = Variables.Find(FieldName);
	if (Result)
	{
		for (auto NumberField : Result->GetDoubleValues())
		{
			ArrayRes.Add(NumberField);
		}
	}
	return ArrayRes;
}

void UVariableContainer::SetNumberArrayField(const FString& FieldName, const TArray<float>& NumberArray)
{
	TArray<double> Value;

	for (auto FieldValue : NumberArray)
	{
		Value.Add(FieldValue);
	}
	FGAS_Utility_Variable* Result = Variables.Find(FieldName);
	if (Result)
	{
		Result->SetDoubleValues(Value);
	}
	else
	{
		Variables.Add(FieldName, FGAS_Utility_Variable(Value));
	}
}

void UVariableContainer::SetVectorArrayField(const FString& FieldName, const TArray<FVector>& NumberArray)
{
	TArray<FVector> Value;

	for (auto FieldValue : NumberArray)
	{
		Value.Add(FieldValue);
	}
	FGAS_Utility_Variable* Result = Variables.Find(FieldName);
	if (Result)
	{
		Result->SetVectorValues(Value);
	}
	else
	{
		Variables.Add(FieldName, FGAS_Utility_Variable(Value));
	}
}

void UVariableContainer::SetNumberDoubleArrayField(const FString& FieldName, const TArray<double>& NumberArray)
{
	FGAS_Utility_Variable* Result = Variables.Find(FieldName);
	if (Result)
	{
		Result->SetDoubleValues(NumberArray);
	}
	else
	{
		Variables.Add(FieldName, FGAS_Utility_Variable(NumberArray));
	}
}

TArray<double> UVariableContainer::GetNumberDoubleArrayField(const FString& FieldName) const
{
	auto ArrayRes = TArray<double>();
	const FGAS_Utility_Variable* Result = Variables.Find(FieldName);
	if (Result)
	{
		return Result->GetDoubleValues();
	}
	return ArrayRes;
}

TArray<FString> UVariableContainer::GetStringArrayField(const FString& FieldName) const
{
	const FGAS_Utility_Variable* Result = Variables.Find(FieldName);
	if (Result)
	{
		return Result->GetStringValues();
	}
	return TArray<FString>();
}

void UVariableContainer::SetStringArrayField(const FString& FieldName, const TArray<FString>& StringArray)
{
	FGAS_Utility_Variable* Result = Variables.Find(FieldName);
	if (Result)
	{
		Result->SetStringValues(StringArray);
	}
	else
	{
		Variables.Add(FieldName, FGAS_Utility_Variable(StringArray));
	}
}

TArray<bool> UVariableContainer::GetBoolArrayField(const FString& FieldName) const
{
	const FGAS_Utility_Variable* Result = Variables.Find(FieldName);
	if (Result)
	{
		return Result->GetBoolValues();
	}
	return TArray<bool>();
}

void UVariableContainer::SetBoolArrayField(const FString& FieldName, const TArray<bool>& BoolArray)
{
	FGAS_Utility_Variable* Result = Variables.Find(FieldName);
	if (Result)
	{
		Result->SetBoolValues(BoolArray);
	}
	else
	{
		Variables.Add(FieldName, FGAS_Utility_Variable(BoolArray));
	}
}

TArray<UObject*> UVariableContainer::GetObjectArrayField(const FString& FieldName) const
{
	const FGAS_Utility_Variable* Result = Variables.Find(FieldName);
	if (Result)
	{
		return Result->GetObjectValues();
	}
	return TArray<UObject*>();
}

void UVariableContainer::SetObjectArrayField(const FString& FieldName, const TArray<UObject*>& ObjectArray)
{
	FGAS_Utility_Variable* Result = Variables.Find(FieldName);
	if (Result)
	{
		Result->SetObjectValues(ObjectArray);
	}
	else
	{
		Variables.Add(FieldName, FGAS_Utility_Variable(ObjectArray));
	}
}

FGameplayTagContainer UVariableContainer::GetTagArrayField(const FString& FieldName) const
{
	const FGAS_Utility_Variable* Result = Variables.Find(FieldName);
	if (Result)
	{
		return Result->GetTagValues();
	}
	return FGameplayTagContainer();
}

void UVariableContainer::SetTagArrayField(const FString& FieldName, FGameplayTagContainer Tags)
{
	FGAS_Utility_Variable* Result = Variables.Find(FieldName);
	if (Result)
	{
		Result->SetTagValues(Tags);
	}
	else
	{
		Variables.Add(FieldName, FGAS_Utility_Variable(Tags));
	}
}

void UVariableContainer::SetupFromArrays(TArray<FString> Names, TArray<FGAS_Utility_Variable> Values)
{
	for (int i = 0; i < Names.Num(); ++i)
	{
		if (Values.IsValidIndex(i))
		{
			Variables.Add(Names[i], Values[i]);
		}
	}
}

void UVariableContainer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	UObject::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;

	Params.Condition = COND_None;
	DOREPLIFETIME_WITH_PARAMS_FAST(UVariableContainer, VariablesNames, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(UVariableContainer, VariablesValues, Params);
}

void UVariableContainer::PostNetReceive()
{
	for (int i = 0; i < VariablesNames.Num(); ++i)
	{
		if (VariablesValues.IsValidIndex(i))
		{
			Variables.Add(VariablesNames[i], VariablesValues[i]);
		}
	}
	UObject::PostNetReceive();
}

void UVariableContainer::Serialize(FArchive& Ar)
{
	if (Ar.IsSaving())
	{
		VariablesNames.Empty();
		VariablesValues.Empty();
		for (TTuple<FString, FGAS_Utility_Variable> Variable : Variables)
		{
			VariablesNames.Add(Variable.Key);
			VariablesValues.Add(Variable.Value);
		}
	}

	UObject::Serialize(Ar);
	if (Ar.IsLoading())
	{
		for (int i = 0; i < VariablesNames.Num(); ++i)
		{
			if (VariablesValues.IsValidIndex(i))
			{
				Variables.Add(VariablesNames[i], VariablesValues[i]);
			}
		}
	}
}

bool UVariableContainer::IsNameStableForNetworking() const
{
	return false;
}

bool UVariableContainer::IsSupportedForNetworking() const
{
	return true;
}
