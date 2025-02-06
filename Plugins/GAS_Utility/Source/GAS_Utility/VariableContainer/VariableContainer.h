// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/NoExportTypes.h"
#include "VariableContainer.generated.h"

UENUM(BlueprintType)
enum class EGAS_Utility_VariableType : uint8
{
	Variable_Null UMETA(DisplayName = "Null"),
	Variable_Bool UMETA(DisplayName = "Boolean"),
	Variable_Number UMETA(DisplayName = "Number"),
	Variable_String UMETA(DisplayName = "String"),
	Variable_Object UMETA(DisplayName = "Object"),
	Variable_Vector UMETA(DisplayName = "Vector"),
	Variable_Tag UMETA(DisplayName = "Tag")
};

USTRUCT(BlueprintType)
struct GAS_UTILITY_API FGAS_Utility_Variable
{
	GENERATED_BODY()

public:
	FGAS_Utility_Variable()
	{
		LastTypeSet = EGAS_Utility_VariableType::Variable_Null;
	}

	FGAS_Utility_Variable(double NewValue)
	{
		SetDoubleValue(NewValue);
	}

	FGAS_Utility_Variable(FVector NewValue)
	{
		SetVectorValue(NewValue);
	}

	FGAS_Utility_Variable(FString NewValue)
	{
		SetStringValue(NewValue);
	}

	FGAS_Utility_Variable(bool NewValue)
	{
		SetBoolValue(NewValue);
	}

	FGAS_Utility_Variable(TWeakObjectPtr<UObject> NewValue)
	{
		SetObjectValue(NewValue);
	}

	FGAS_Utility_Variable(FGameplayTag NewValue)
	{
		SetTagValue(NewValue);
	}

	FGAS_Utility_Variable(FGameplayTagContainer NewValue)
	{
		SetTagValues(NewValue);
	}

	FGAS_Utility_Variable(TArray<double> NewValue)
	{
		SetDoubleValues(NewValue);
	}

	FGAS_Utility_Variable(TArray<FVector> NewValue)
	{
		SetVectorValues(NewValue);
	}

	FGAS_Utility_Variable(TArray<FString> NewValue)
	{
		SetStringValues(NewValue);
	}

	FGAS_Utility_Variable(TArray<bool> NewValue)
	{
		SetBoolValues(NewValue);
	}

	FGAS_Utility_Variable(TArray<UObject*> NewValue)
	{
		SetObjectValues(NewValue);
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Values")
	TArray<FString> StringValues;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Values")
	TArray<double> DoubleValues;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Values")
	TArray<FVector> VectorValues;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Values")
	TArray<bool> BoolValues;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Values")
	TArray<UObject*> ObjectValues;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Values")
	FGameplayTagContainer TagValues;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Values")
	FGameplayTag TagValue;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Values")
	EGAS_Utility_VariableType LastTypeSet;

	EGAS_Utility_VariableType GetLastTypeSet() const
	{
		return LastTypeSet;
	}

	void SetStringValue(const FString NewValue)
	{
		StringValues.Empty();
		StringValues.Add(NewValue);
		LastTypeSet = EGAS_Utility_VariableType::Variable_String;
	}

	void SetDoubleValue(const double NewValue)
	{
		DoubleValues.Empty();
		DoubleValues.Add(NewValue);
		LastTypeSet = EGAS_Utility_VariableType::Variable_Number;
	}

	void SetVectorValue(const FVector NewValue)
	{
		VectorValues.Empty();
		VectorValues.Add(NewValue);
		LastTypeSet = EGAS_Utility_VariableType::Variable_Vector;
	}

	void SetBoolValue(const bool NewValue)
	{
		BoolValues.Empty();
		BoolValues.Add(NewValue);
		LastTypeSet = EGAS_Utility_VariableType::Variable_Bool;
	}

	void SetObjectValue(const TWeakObjectPtr<UObject> NewValue)
	{
		ObjectValues.Empty();
		ObjectValues.Add(NewValue.Get());
		LastTypeSet = EGAS_Utility_VariableType::Variable_Object;
	}

	void SetTagValue(const FGameplayTag NewValue)
	{
		TagValue = NewValue;
		LastTypeSet = EGAS_Utility_VariableType::Variable_Tag;
	}

	void SetTagValues(const FGameplayTagContainer NewValue)
	{
		TagValues = NewValue;
		LastTypeSet = EGAS_Utility_VariableType::Variable_Tag;
	}

	void SetStringValues(const TArray<FString>& NewValue)
	{
		StringValues = NewValue;
		LastTypeSet = EGAS_Utility_VariableType::Variable_String;
	}

	void SetDoubleValues(const TArray<double>& NewValue)
	{
		DoubleValues = NewValue;
		LastTypeSet = EGAS_Utility_VariableType::Variable_Number;
	}

	void SetVectorValues(const TArray<FVector>& NewValue)
	{
		VectorValues = NewValue;
		LastTypeSet = EGAS_Utility_VariableType::Variable_Vector;
	}

	void SetBoolValues(const TArray<bool>& NewValue)
	{
		BoolValues = NewValue;
		LastTypeSet = EGAS_Utility_VariableType::Variable_Bool;
	}

	void SetObjectValues(const TArray<UObject*>& NewValue)
	{
		ObjectValues = NewValue;
		LastTypeSet = EGAS_Utility_VariableType::Variable_Object;
	}

	FString GetStringValue() const
	{
		if (StringValues.Num())
			return StringValues[0];
		return "";
	}

	double GetDoubleValue() const
	{
		if (DoubleValues.Num())
			return DoubleValues[0];
		return 0.0;
	}

	FVector GetVectorValue() const
	{
		if (VectorValues.Num())
			return VectorValues[0];
		return FVector::ZeroVector;
	}

	bool GetBoolValue() const
	{
		if (BoolValues.Num())
			return BoolValues[0];
		return false;
	}

	TWeakObjectPtr<UObject> GetObjectValue() const
	{
		if (ObjectValues.Num())
			return ObjectValues[0];
		return nullptr;
	}

	FGameplayTagContainer GetTagValues() const
	{
		return TagValues;
	}

	FGameplayTag GetTagValue() const
	{
		return TagValue;
	}

	TArray<FString> GetStringValues() const
	{
		return StringValues;
	}

	TArray<double> GetDoubleValues() const
	{
		return DoubleValues;
	}

	TArray<FVector> GetVectorValues() const
	{
		return VectorValues;
	}

	TArray<bool> GetBoolValues() const
	{
		return BoolValues;
	}

	TArray<UObject*> GetObjectValues() const
	{
		return ObjectValues;
	}
};


/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class GAS_UTILITY_API UVariableContainer : public UObject
{
	GENERATED_BODY()

public :
	UVariableContainer(const FObjectInitializer& ObjectInitializer);
	//UVariableContainer(const FObjectInitializer& ObjectInitializer);

	/** Gets the type of value as string for a given field */
	UFUNCTION(BlueprintPure, Category = "GAS_Utility|VariableContainer")
	FString GetFieldTypeString(const FString& FieldName) const;

	/** Returns a list of field names that exist in the object */
	UFUNCTION(BlueprintPure, Category = "GAS_Utility|VariableContainer")
	TArray<FString> GetFieldNames();

	/** Checks to see if the FieldName exists in the object */
	UFUNCTION(BlueprintCallable, Category = "GAS_Utility|VariableContainer")
	bool HasField(const FString& FieldName) const;

	/** Remove field named FieldName */
	UFUNCTION(BlueprintCallable, Category = "GAS_Utility|VariableContainer")
	void RemoveField(const FString& FieldName);

	/** Remove field named FieldName */
	UFUNCTION(BlueprintCallable, Category = "GAS_Utility|VariableContainer")
	int Num();

	//////////////////////////////////////////////////////////////////////////
	// FJsonObject API Helpers (easy to use with simple Json objects)

	/** Get the field named FieldName as a number. Ensures that the field is present and is of type Json number.
	 * Attn.!! float used instead of double to make the function blueprintable! */
	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe), Category = "GAS_Utility|VariableContainer")
	float GetNumberField(const FString& FieldName) const;

	/** Add a field named FieldName with Number as value
	 * Attn.!! float used instead of double to make the function blueprintable! */
	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe), Category = "GAS_Utility|VariableContainer")
	void SetNumberField(const FString& FieldName, float Number);

	/** Add a field named FieldName with Number as value
	 * Attn.!! float used instead of double to make the function blueprintable! */
	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe), Category = "GAS_Utility|VariableContainer")
	void SetNumberDoubleField(const FString& FieldName, double Number);

	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe), Category = "GAS_Utility|VariableContainer")
	void SetVectorField(const FString& FieldName, FVector Vector);

	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe), Category = "GAS_Utility|VariableContainer")
	double GetNumberDoubleField(const FString& FieldName) const;

	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe), Category = "GAS_Utility|VariableContainer")
	FVector GetVectorField(const FString& FieldName) const;

	/** Get the field named FieldName as an Integer. Ensures that the field is present and is of type Json number. */
	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe), Category = "GAS_Utility|VariableContainer")
	int32 GetIntegerField(const FString& FieldName) const;

	/** Add a field named FieldName with Integer as value. */
	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe), Category = "GAS_Utility|VariableContainer")
	void SetIntegerField(const FString& FieldName, int32 Number);

	/** Get the field named FieldName as an Int64. Ensures that the field is present and is of type Json number. */
	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe), Category = "GAS_Utility|VariableContainer")
	int64 GetInt64Field(const FString& FieldName) const;

	/** Add a field named FieldName with Int64 as value. */
	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe), Category = "GAS_Utility|VariableContainer")
	void SetInt64Field(const FString& FieldName, int64 Number);

	/** Get the field named FieldName as a string. */
	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe), Category = "GAS_Utility|VariableContainer")
	FString GetStringField(const FString& FieldName) const;

	/** Add a field named FieldName with value of StringValue */
	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe), Category = "GAS_Utility|VariableContainer")
	void SetStringField(const FString& FieldName, const FString& StringValue);

	/** Get the field named FieldName as a boolean. */
	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe), Category = "GAS_Utility|VariableContainer")
	bool GetBoolField(const FString& FieldName) const;

	/** Set a boolean field named FieldName and value of InValue */
	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe), Category = "GAS_Utility|VariableContainer")
	void SetBoolField(const FString& FieldName, bool InValue);

	/** Get the field named FieldName as a Json object. */
	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe), Category = "GAS_Utility|VariableContainer")
	UObject* GetObjectField(const FString& FieldName) const;

	/** Set an ObjectField named FieldName and value of JsonObject */
	UFUNCTION(BlueprintCallable, Category = "GAS_Utility|VariableContainer")
	void SetObjectField(const FString& FieldName, UObject* Object);

	UFUNCTION(BlueprintCallable, Category = "GAS_Utility|VariableContainer")
	void SetTagField(const FString& FieldName, FGameplayTag Tag);


	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe), Category = "GAS_Utility|VariableContainer")
	FGameplayTag GetTagField(const FString& FieldName) const;

	/** Set a map of fields with String values */

	UFUNCTION(BlueprintCallable, Category = "GAS_Utility|VariableContainer")
	void SetMapFields_Tag(const TMap<FString, FGameplayTag>& Fields);

	UFUNCTION(BlueprintCallable, Category = "GAS_Utility|VariableContainer")
	void SetMapFields_string(const TMap<FString, FString>& Fields);

	/** Set a map of fields with uint8 values */
	UFUNCTION(BlueprintCallable, Category = "GAS_Utility|VariableContainer")
	void SetMapFields_uint8(const TMap<FString, uint8>& Fields);

	/** Set a map of fields with int32 values */
	UFUNCTION(BlueprintCallable, Category = "GAS_Utility|VariableContainer")
	void SetMapFields_int32(const TMap<FString, int32>& Fields);

	/** Set a map of fields with int64 values */
	UFUNCTION(BlueprintCallable, Category = "GAS_Utility|VariableContainer")
	void SetMapFields_int64(const TMap<FString, int64>& Fields);

	/** Set a map of fields with bool values */
	UFUNCTION(BlueprintCallable, Category = "GAS_Utility|VariableContainer")
	void SetMapFields_bool(const TMap<FString, bool>& Fields);

public:
	/** Get the field named FieldName as a Number Array. Use it only if you're sure that array is uniform!
	 * Attn.!! float used instead of double to make the function blueprintable! */
	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe), Category = "VaRest|Json")
	TArray<float> GetNumberArrayField(const FString& FieldName) const;

	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe), Category = "VaRest|Json")
	TArray<FVector> GetVectorArrayField(const FString& FieldName) const;

	/** Get the field named FieldName as a Number Array. Use it only if you're sure that array is uniform! */
	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe), Category = "VaRest|Json")
	TArray<int32> GetIntegerArrayField(const FString& FieldName) const;

	/** Set an ObjectField named FieldName and value of Number Array
	 * Attn.!! float used instead of double to make the function blueprintable! */
	UFUNCTION(BlueprintCallable, Category = "VaRest|Json")
	void SetNumberArrayField(const FString& FieldName, const TArray<float>& NumberArray);

	UFUNCTION(BlueprintCallable, Category = "VaRest|Json")
	void SetVectorArrayField(const FString& FieldName, const TArray<FVector>& NumberArray);


	UFUNCTION(BlueprintCallable, Category = "VaRest|Json")
	void SetNumberDoubleArrayField(const FString& FieldName, const TArray<double>& NumberArray);

	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe), Category = "VaRest|Json")
	TArray<double> GetNumberDoubleArrayField(const FString& FieldName) const;

	/** Get the field named FieldName as a String Array. Use it only if you're sure that array is uniform! */
	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe), Category = "VaRest|Json")
	TArray<FString> GetStringArrayField(const FString& FieldName) const;

	/** Set an ObjectField named FieldName and value of String Array */
	UFUNCTION(BlueprintCallable, Category = "VaRest|Json")
	void SetStringArrayField(const FString& FieldName, const TArray<FString>& StringArray);

	/** Get the field named FieldName as a Bool Array. Use it only if you're sure that array is uniform! */
	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe), Category = "VaRest|Json")
	TArray<bool> GetBoolArrayField(const FString& FieldName) const;

	/** Set an ObjectField named FieldName and value of Bool Array */
	UFUNCTION(BlueprintCallable, Category = "VaRest|Json")
	void SetBoolArrayField(const FString& FieldName, const TArray<bool>& BoolArray);

	/** Get the field named FieldName as an Object Array. Use it only if you're sure that array is uniform! */
	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe), Category = "VaRest|Json")
	TArray<UObject*> GetObjectArrayField(const FString& FieldName) const;

	/** Set an ObjectField named FieldName and value of Ob Array */
	UFUNCTION(BlueprintCallable, Category = "VaRest|Json")
	void SetObjectArrayField(const FString& FieldName, const TArray<UObject*>& ObjectArray);

	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe), Category = "VaRest|Json")
	FGameplayTagContainer GetTagArrayField(const FString& FieldName) const;

	UFUNCTION(BlueprintCallable, Category = "VaRest|Json")
	void SetTagArrayField(const FString& FieldName, FGameplayTagContainer Tags);


	void SetupFromArrays(TArray<FString> Names, TArray<FGAS_Utility_Variable> Values);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostNetReceive() override;
	virtual void Serialize(FArchive& Ar) override;
	bool IsNameStableForNetworking() const override;
	bool IsSupportedForNetworking() const override;

public:
	TMap<FString, FGAS_Utility_Variable> Variables;
	UPROPERTY(Replicated)
	TArray<FString> VariablesNames;
	UPROPERTY(Replicated)
	TArray<FGAS_Utility_Variable> VariablesValues;
};


USTRUCT(BlueprintType)
struct GAS_UTILITY_API FGAS_Utility_VariableContainerStruct
{
	GENERATED_BODY()

public:
	FGAS_Utility_VariableContainerStruct()
	{
	}

	FGAS_Utility_VariableContainerStruct(TArray<FString> Names, TArray<FGAS_Utility_Variable> Values): VariablesNames(Names), VariablesValues(Values)
	{
	}

	FGAS_Utility_VariableContainerStruct(UVariableContainer* Source)
	{
		if (Source)
		{
			for (auto Element : Source->Variables)
			{
				VariablesNames.Add(Element.Key);
				VariablesValues.Add(Element.Value);
			}
		}
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Values")
	TArray<FString> VariablesNames;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Values")
	TArray<FGAS_Utility_Variable> VariablesValues;
};
