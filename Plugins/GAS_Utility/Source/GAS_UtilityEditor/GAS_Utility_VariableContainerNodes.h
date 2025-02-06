

#pragma once

#include "Runtime/Launch/Resources/Version.h"

#include "CoreMinimal.h"
#include "K2Node.h"

#include "GAS_Utility_VariableContainerNodes.generated.h"

UENUM(BlueprintType)
enum class EGAS_UtilityEditor_VariableType : uint8
{
	// Variable_Null UMETA(DisplayName = "Null"),
	Variable_Bool UMETA(DisplayName = "Boolean"),
	Variable_Number UMETA(DisplayName = "Number"),
	Variable_String UMETA(DisplayName = "String"),
	Variable_Object UMETA(DisplayName = "Object"),
	Variable_Vector UMETA(DisplayName = "Vector"),
	Variable_Tag UMETA(DisplayName = "Tag")
};

USTRUCT(BlueprintType)
struct FGAS_Utility_NamedType
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = NamedType)
	FName Name;

	UPROPERTY(EditAnywhere, Category = NamedType)
	EGAS_UtilityEditor_VariableType Type;

	UPROPERTY(EditAnywhere, Category = NamedType)
	bool bIsArray;

	FGAS_Utility_NamedType()
		: Type(EGAS_UtilityEditor_VariableType::Variable_String)
		, bIsArray(false)
	{
	}
};

UCLASS(BlueprintType, Blueprintable)
class GAS_UTILITYEDITOR_API UGAS_Utility_MakeVariableContainerNode : public UK2Node
{
	GENERATED_UCLASS_BODY()

public:
	// Begin UEdGraphNode interface.
	virtual void AllocateDefaultPins() override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	// End UEdGraphNode interface.

	// Begin UK2Node interface
	virtual bool IsNodePure() const { return true; }
	virtual bool ShouldShowNodeProperties() const { return true; }
	void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual FText GetMenuCategory() const override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual class FNodeHandlingFunctor* CreateNodeHandler(class FKismetCompilerContext& CompilerContext) const override;
	// End UK2Node interface.

protected:
	virtual void CreateProjectionPins(UEdGraphPin* Source);

public:
	UPROPERTY(EditAnywhere, Category = PinOptions)
	TArray<FGAS_Utility_NamedType> Inputs;
};

UCLASS(BlueprintType, Blueprintable)
class GAS_UTILITYEDITOR_API UGAS_Utility_BreakVariableContainerNode : public UK2Node
{
	GENERATED_UCLASS_BODY()

public:
	// Begin UEdGraphNode interface.
	virtual void AllocateDefaultPins() override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	// End UEdGraphNode interface.

	// Begin UK2Node interface
	virtual bool IsNodePure() const { return true; }
	virtual bool ShouldShowNodeProperties() const { return true; }
	void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual FText GetMenuCategory() const override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual class FNodeHandlingFunctor* CreateNodeHandler(class FKismetCompilerContext& CompilerContext) const override;
	// End UK2Node interface.

protected:
	virtual void CreateProjectionPins(UEdGraphPin* Source);

public:
	UPROPERTY(EditAnywhere, Category = PinOptions)
	TArray<FGAS_Utility_NamedType> Outputs;
};
