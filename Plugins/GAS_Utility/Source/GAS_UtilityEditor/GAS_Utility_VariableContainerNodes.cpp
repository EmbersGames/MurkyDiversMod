// Copyright 2014-2019 Vladimir Alyamkin. All Rights Reserved.
// Original code by https://github.com/unktomi

#include "GAS_Utility_VariableContainerNodes.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphNodeUtils.h" // for FNodeTextCache
#include "EdGraphSchema_K2.h"
#include "EdGraphUtilities.h"
#include "EditorCategoryUtils.h"
#include "GameplayTagContainer.h"
#include "KismetCompiler.h"
#include "Runtime/Launch/Resources/Version.h"

#define LOCTEXT_NAMESPACE "GAS_Utility_VariableContainerNodes"

class FKCHandler_BreakVariableContainerNode : public FNodeHandlingFunctor
{
public:
	FKCHandler_BreakVariableContainerNode(FKismetCompilerContext& InCompilerContext)
		: FNodeHandlingFunctor(InCompilerContext)
	{
	}

	virtual void Compile(FKismetFunctionContext& Context, UEdGraphNode* Node) override
	{
		UEdGraphPin* InputPin = nullptr;

		for (int32 PinIndex = 0; PinIndex < Node->Pins.Num(); ++PinIndex)
		{
			UEdGraphPin* Pin = Node->Pins[PinIndex];
			if (Pin && (EGPD_Input == Pin->Direction))
			{
				InputPin = Pin;
				break;
			}
		}

		UEdGraphPin* InNet = FEdGraphUtilities::GetNetFromPin(InputPin);
		UClass* Class = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), nullptr, TEXT("class'GAS_Utility.VariableContainer'")));

		FBPTerminal** SourceTerm = Context.NetMap.Find(InNet);
		if (SourceTerm == nullptr)
		{
			return;
		}

		for (int32 PinIndex = 0; PinIndex < Node->Pins.Num(); ++PinIndex)
		{
			UEdGraphPin* Pin = Node->Pins[PinIndex];
			if (Pin && (EGPD_Output == Pin->Direction))
			{
				if (Pin->LinkedTo.Num() < 1)
				{
					continue;
				}

				FBPTerminal** Target = Context.NetMap.Find(Pin);

				const FName& FieldName = Pin->PinName;
				const FName& FieldType = Pin->PinType.PinCategory;

				FBPTerminal* FieldNameTerm = Context.CreateLocalTerminal(ETerminalSpecification::TS_Literal);
				FieldNameTerm->Type.PinCategory = CompilerContext.GetSchema()->PC_String;
				FieldNameTerm->SourcePin = Pin;

				FieldNameTerm->Name = FieldName.ToString();
				FieldNameTerm->TextLiteral = FText::FromName(FieldName);

				FBlueprintCompiledStatement& Statement = Context.AppendStatementForNode(Node);
				FName FunctionName;

				const bool bIsArray = Pin->PinType.ContainerType == EPinContainerType::Array;
				if (FieldType == CompilerContext.GetSchema()->PC_Boolean)
				{
					FunctionName = bIsArray ? TEXT("GetBoolArrayField") : TEXT("GetBoolField");
				}
				else if (FieldType == CompilerContext.GetSchema()->PC_Real && Pin->PinType.PinSubCategory == CompilerContext.GetSchema()->PC_Double)
				{
					FunctionName = bIsArray ? TEXT("GetNumberDoubleArrayField") : TEXT("GetNumberDoubleField");
				}
				else if (FieldType == CompilerContext.GetSchema()->PC_Real)
				{
					FunctionName = bIsArray ? TEXT("GetNumberArrayField") : TEXT("GetNumberField");
				}
				else if (FieldType == CompilerContext.GetSchema()->PC_String)
				{
					FunctionName = bIsArray ? TEXT("GetStringArrayField") : TEXT("GetStringField");
				}
				else if (FieldType == CompilerContext.GetSchema()->PC_Object)
				{
					FunctionName = bIsArray ? TEXT("GetObjectArrayField") : TEXT("GetObjectField");
				}
				else if (FieldType == CompilerContext.GetSchema()->PC_Struct)
				{
					if (Pin->PinType.PinSubCategoryObject == TBaseStructure<FVector>::Get())
						FunctionName = bIsArray ? TEXT("GetVectorArrayField") : TEXT("GetVectorField");
					else if (Pin->PinType.PinSubCategoryObject == FGameplayTagContainer::StaticStruct())
						FunctionName = TEXT("GetTagArrayField");
					else
						FunctionName = TEXT("GetTagField");
				}
				else
				{
					continue;
				}

				UFunction* FunctionPtr = Class->FindFunctionByName(FunctionName);
				Statement.Type = KCST_CallFunction;
				Statement.FunctionToCall = FunctionPtr;
				Statement.FunctionContext = *SourceTerm;
				Statement.bIsParentContext = false;
				Statement.LHS = *Target;
				Statement.RHS.Add(FieldNameTerm);
			}
		}
	}

	FBPTerminal* RegisterInputTerm(FKismetFunctionContext& Context, UGAS_Utility_BreakVariableContainerNode* Node)
	{
		// Find input pin
		UEdGraphPin* InputPin = nullptr;
		for (int32 PinIndex = 0; PinIndex < Node->Pins.Num(); ++PinIndex)
		{
			UEdGraphPin* Pin = Node->Pins[PinIndex];
			if (Pin && (EGPD_Input == Pin->Direction))
			{
				InputPin = Pin;
				break;
			}
		}
		check(NULL != InputPin);

		// Find structure source net
		UEdGraphPin* Net = FEdGraphUtilities::GetNetFromPin(InputPin);
		FBPTerminal** TermPtr = Context.NetMap.Find(Net);

		if (!TermPtr)
		{
			FBPTerminal* Term = Context.CreateLocalTerminalFromPinAutoChooseScope(Net, Context.NetNameMap->MakeValidName(Net));

			Context.NetMap.Add(Net, Term);

			return Term;
		}

		return *TermPtr;
	}

	void RegisterOutputTerm(FKismetFunctionContext& Context, UEdGraphPin* OutputPin, FBPTerminal* ContextTerm)
	{
		FBPTerminal* Term = Context.CreateLocalTerminalFromPinAutoChooseScope(OutputPin, Context.NetNameMap->MakeValidName(OutputPin));
		Context.NetMap.Add(OutputPin, Term);
	}

	virtual void RegisterNets(FKismetFunctionContext& Context, UEdGraphNode* InNode) override
	{
		UGAS_Utility_BreakVariableContainerNode* Node = Cast<UGAS_Utility_BreakVariableContainerNode>(InNode);
		FNodeHandlingFunctor::RegisterNets(Context, Node);

		check(NULL != Node);

		if (FBPTerminal* StructContextTerm = RegisterInputTerm(Context, Node))
		{
			for (int32 PinIndex = 0; PinIndex < Node->Pins.Num(); ++PinIndex)
			{
				UEdGraphPin* Pin = Node->Pins[PinIndex];
				if (nullptr != Pin && EGPD_Output == Pin->Direction)
				{
					RegisterOutputTerm(Context, Pin, StructContextTerm);
				}
			}
		}
	}
};

/**
 * Main node class
 */
UGAS_Utility_BreakVariableContainerNode::UGAS_Utility_BreakVariableContainerNode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

FNodeHandlingFunctor* UGAS_Utility_BreakVariableContainerNode::CreateNodeHandler(class FKismetCompilerContext& CompilerContext) const
{
	return new FKCHandler_BreakVariableContainerNode(CompilerContext);
}

void UGAS_Utility_BreakVariableContainerNode::AllocateDefaultPins()
{
	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();

	UClass* Class = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), nullptr, TEXT("class'GAS_Utility.VariableContainer'")));
	UEdGraphPin* Pin = CreatePin(EGPD_Input, K2Schema->PC_Object, TEXT(""), Class, TEXT("Target"));

	K2Schema->SetPinAutogeneratedDefaultValueBasedOnType(Pin);

	CreateProjectionPins(Pin);
}

FLinearColor UGAS_Utility_BreakVariableContainerNode::GetNodeTitleColor() const
{
	return FLinearColor(255.0f, 255.0f, 0.0f);
}

void UGAS_Utility_BreakVariableContainerNode::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	const FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	if (PropertyName == GET_MEMBER_NAME_STRING_CHECKED(UGAS_Utility_BreakVariableContainerNode, Outputs) ||
		PropertyName == GET_MEMBER_NAME_STRING_CHECKED(FGAS_Utility_NamedType, Name) ||
		PropertyName == GET_MEMBER_NAME_STRING_CHECKED(FGAS_Utility_NamedType, Type) ||
		PropertyName == GET_MEMBER_NAME_STRING_CHECKED(FGAS_Utility_NamedType, bIsArray))
	{
		ReconstructNode();
		GetGraph()->NotifyGraphChanged();
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}

void UGAS_Utility_BreakVariableContainerNode::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	// actions get registered under specific object-keys; the idea is that
	// actions might have to be updated (or deleted) if their object-key is
	// mutated (or removed)... here we use the node's class (so if the node
	// type disappears, then the action should go with it)
	UClass* ActionKey = GetClass();

	// to keep from needlessly instantiating a UBlueprintNodeSpawner, first
	// check to make sure that the registrar is looking for actions of this type
	// (could be regenerating actions for a specific asset, and therefore the
	// registrar would only accept actions corresponding to that asset)
	if (ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(NodeSpawner != nullptr);

		ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
	}
}

FText UGAS_Utility_BreakVariableContainerNode::GetMenuCategory() const
{
	static FNodeTextCache CachedCategory;

	if (CachedCategory.IsOutOfDate(this))
	{
		// FText::Format() is slow, so we cache this to save on performance
		CachedCategory.SetCachedText(FEditorCategoryUtils::BuildCategoryString(FCommonEditorCategory::Utilities, LOCTEXT("ActionMenuCategory", "GAS_Utility")), this);
	}
	return CachedCategory;
}

void UGAS_Utility_BreakVariableContainerNode::CreateProjectionPins(UEdGraphPin* Source)
{
	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();

	for (TArray<FGAS_Utility_NamedType>::TIterator it(Outputs); it; ++it)
	{
		FName Type;
		FName SubCategory;

		UObject* Subtype = nullptr;

		switch ((*it).Type)
		{
		case EGAS_UtilityEditor_VariableType::Variable_Bool:
			Type = K2Schema->PC_Boolean;
			break;

		case EGAS_UtilityEditor_VariableType::Variable_Number:
			Type = K2Schema->PC_Real;
			SubCategory = K2Schema->PC_Double;
			break;

		case EGAS_UtilityEditor_VariableType::Variable_String:
			Type = K2Schema->PC_String;
			break;

		case EGAS_UtilityEditor_VariableType::Variable_Object:
			Type = K2Schema->PC_Object;
			Subtype = UObject::StaticClass();
			break;

		case EGAS_UtilityEditor_VariableType::Variable_Vector:
			Type = K2Schema->PC_Struct;
			Subtype = TBaseStructure<FVector>::Get();
			break;

		case EGAS_UtilityEditor_VariableType::Variable_Tag:
			Type = K2Schema->PC_Struct;
			if ((*it).bIsArray)
				Subtype = FGameplayTagContainer::StaticStruct();
			else
				Subtype = FGameplayTag::StaticStruct();
			break;
		}

		UEdGraphNode::FCreatePinParams OutputPinParams;

		if ((*it).Type != EGAS_UtilityEditor_VariableType::Variable_Tag)
			OutputPinParams.ContainerType = (*it).bIsArray ? EPinContainerType::Array : EPinContainerType::None;
		UEdGraphPin* OutputPin = CreatePin(EGPD_Output, Type, SubCategory, Subtype, (*it).Name, OutputPinParams);
	}
}

FText UGAS_Utility_BreakVariableContainerNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("GAS_Utility_Break_VariableContainer.NodeTitle", "Break Variable Container");
}

class FKCHandler_MakeVariableContainerNode : public FNodeHandlingFunctor
{
public:
	FKCHandler_MakeVariableContainerNode(FKismetCompilerContext& InCompilerContext)
		: FNodeHandlingFunctor(InCompilerContext)
	{
	}

	virtual void Compile(FKismetFunctionContext& Context, UEdGraphNode* Node) override
	{
		UEdGraphPin* OutputPin = nullptr;

		for (int32 PinIndex = 0; PinIndex < Node->Pins.Num(); ++PinIndex)
		{
			UEdGraphPin* Pin = Node->Pins[PinIndex];
			if (Pin && (EGPD_Output == Pin->Direction))
			{
				OutputPin = Pin;
				break;
			}
		}

		UClass* Class = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), nullptr, TEXT("class'GAS_Utility.VariableContainer'")));

		FBPTerminal** TargetTerm = Context.NetMap.Find(OutputPin);
		if (TargetTerm == nullptr)
		{
			return;
		}

		{
			UClass* SubsystemClass = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), nullptr, TEXT("class'GAS_Utility.VariableContainerSubsystem'")));

			const FName FunctionName = TEXT("StaticConstructVariableContainer");
			UFunction* FunctionPtr = SubsystemClass->FindFunctionByName(FunctionName);
			FBlueprintCompiledStatement& Statement = Context.AppendStatementForNode(Node);
			Statement.Type = KCST_CallFunction;
			Statement.FunctionToCall = FunctionPtr;
			Statement.FunctionContext = nullptr;
			Statement.bIsParentContext = false;
			Statement.LHS = *TargetTerm;
			FBPTerminal* NullTerm = Context.CreateLocalTerminal(ETerminalSpecification::TS_Literal);
			NullTerm->Type.PinCategory = CompilerContext.GetSchema()->PC_Object;
			NullTerm->ObjectLiteral = nullptr;
			NullTerm->SourcePin = OutputPin;
			Statement.RHS.Add(NullTerm);
		}

		for (int32 PinIndex = 0; PinIndex < Node->Pins.Num(); ++PinIndex)
		{
			UEdGraphPin* Pin = Node->Pins[PinIndex];
			if (Pin && (EGPD_Input == Pin->Direction))
			{
				UEdGraphPin* FirstLink = nullptr;
				
				if(Pin->LinkedTo.IsValidIndex(0))
				{
					FirstLink = Pin->LinkedTo[0];
				}else
				{
					FirstLink = Pin->ParentPin;
				}
				FBPTerminal** Source = Context.NetMap.Find(FEdGraphUtilities::GetNetFromPin(Pin));

				const FName& FieldName = Pin->PinName;
				const FName& FieldType = Pin->PinType.PinCategory;

				FBPTerminal* FieldNameTerm = Context.CreateLocalTerminal(ETerminalSpecification::TS_Literal);
				FieldNameTerm->Type.PinCategory = CompilerContext.GetSchema()->PC_String;
				FieldNameTerm->SourcePin = Pin;

				FieldNameTerm->Name = FieldName.ToString();
				FieldNameTerm->TextLiteral = FText::FromName(FieldName);

				FBlueprintCompiledStatement& Statement = Context.AppendStatementForNode(Node);
				FName FunctionName;

				const bool bIsArray = Pin->PinType.ContainerType == EPinContainerType::Array;
				if (FieldType == CompilerContext.GetSchema()->PC_Boolean)
				{
					FunctionName = bIsArray ? TEXT("SetBoolArrayField") : TEXT("SetBoolField");
				}
				else if (FieldType == CompilerContext.GetSchema()->PC_Real  && FirstLink && FirstLink->PinType.PinSubCategory == CompilerContext.GetSchema()->PC_Double)
				{
					FunctionName = bIsArray ? TEXT("SetNumberDoubleArrayField") : TEXT("SetNumberDoubleField");
				}
				else if (FieldType == CompilerContext.GetSchema()->PC_Real && FirstLink && (FirstLink->PinType.PinSubCategory == CompilerContext.GetSchema()->PC_Real || FirstLink->PinType.PinSubCategory == CompilerContext.GetSchema()->PC_Float)) 
				{
					FunctionName = bIsArray ? TEXT("SetNumberArrayField") : TEXT("SetNumberField");
				}
				else if (FieldType == CompilerContext.GetSchema()->PC_Real)
				{
					FunctionName = bIsArray ? TEXT("SetNumberDoubleArrayField") : TEXT("SetNumberDoubleField");
				}
				else if (FieldType == CompilerContext.GetSchema()->PC_String)
				{
					FunctionName = bIsArray ? TEXT("SetStringArrayField") : TEXT("SetStringField");
				}
				else if (FieldType == CompilerContext.GetSchema()->PC_Object)
				{
					FunctionName = bIsArray ? TEXT("SetObjectArrayField") : TEXT("SetObjectField");
				}
				else if (FieldType == CompilerContext.GetSchema()->PC_Struct)
				{
					if (Pin->PinType.PinSubCategoryObject == TBaseStructure<FVector>::Get())
						FunctionName = bIsArray ? TEXT("SetVectorArrayField") : TEXT("SetVectorField");
					else if (Pin->PinType.PinSubCategoryObject == FGameplayTagContainer::StaticStruct())
						FunctionName = TEXT("SetTagArrayField");
					else
						FunctionName = TEXT("SetTagField");
				}
				else
				{
					continue;
				}

				UFunction* FunctionPtr = Class->FindFunctionByName(FunctionName);
				Statement.Type = KCST_CallFunction;
				Statement.FunctionToCall = FunctionPtr;
				Statement.FunctionContext = *TargetTerm;
				Statement.bIsParentContext = false;
				Statement.LHS = nullptr;
				Statement.RHS.Add(FieldNameTerm);
				Statement.RHS.Add(*Source);
			}
		}
	}

	FBPTerminal* RegisterInputTerm(FKismetFunctionContext& Context, UEdGraphPin* InputPin)
	{
		// Find structure source net
		UEdGraphPin* Net = FEdGraphUtilities::GetNetFromPin(InputPin);
		FBPTerminal** TermPtr = Context.NetMap.Find(Net);

		if (!TermPtr)
		{
			FBPTerminal* Term = Context.CreateLocalTerminalFromPinAutoChooseScope(Net, Context.NetNameMap->MakeValidName(Net));

			Context.NetMap.Add(Net, Term);

			return Term;
		}

		return *TermPtr;
	}

	void RegisterOutputTerm(FKismetFunctionContext& Context, UEdGraphPin* OutputPin)
	{
		FBPTerminal* Term = Context.CreateLocalTerminalFromPinAutoChooseScope(OutputPin, Context.NetNameMap->MakeValidName(OutputPin));
		Context.NetMap.Add(OutputPin, Term);
	}

	virtual void RegisterNets(FKismetFunctionContext& Context, UEdGraphNode* InNode) override
	{
		UGAS_Utility_MakeVariableContainerNode* Node = Cast<UGAS_Utility_MakeVariableContainerNode>(InNode);
		FNodeHandlingFunctor::RegisterNets(Context, Node);

		check(NULL != Node);
		{
			for (int32 PinIndex = 0; PinIndex < Node->Pins.Num(); ++PinIndex)
			{
				UEdGraphPin* Pin = Node->Pins[PinIndex];
				if (EGPD_Output == Pin->Direction)
				{
					RegisterOutputTerm(Context, Pin);
				}
				else
				{
					RegisterInputTerm(Context, Pin);
				}
			}
		}
	}
};

/**
 * Main node class
 */
UGAS_Utility_MakeVariableContainerNode::UGAS_Utility_MakeVariableContainerNode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

FNodeHandlingFunctor* UGAS_Utility_MakeVariableContainerNode::CreateNodeHandler(class FKismetCompilerContext& CompilerContext) const
{
	return new FKCHandler_MakeVariableContainerNode(CompilerContext);
}

void UGAS_Utility_MakeVariableContainerNode::AllocateDefaultPins()
{
	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();

	UClass* Class = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), nullptr, TEXT("class'GAS_Utility.VariableContainer'")));
	UEdGraphPin* Pin = CreatePin(EGPD_Output, K2Schema->PC_Object, TEXT(""), Class, TEXT("Target"));

	K2Schema->SetPinAutogeneratedDefaultValueBasedOnType(Pin);

	CreateProjectionPins(Pin);
}

FLinearColor UGAS_Utility_MakeVariableContainerNode::GetNodeTitleColor() const
{
	return FLinearColor(255.0f, 255.0f, 0.0f);
}

void UGAS_Utility_MakeVariableContainerNode::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	const FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	if (PropertyName == GET_MEMBER_NAME_STRING_CHECKED(UGAS_Utility_MakeVariableContainerNode, Inputs) ||
		PropertyName == GET_MEMBER_NAME_STRING_CHECKED(FGAS_Utility_NamedType, Name) ||
		PropertyName == GET_MEMBER_NAME_STRING_CHECKED(FGAS_Utility_NamedType, Type) ||
		PropertyName == GET_MEMBER_NAME_STRING_CHECKED(FGAS_Utility_NamedType, bIsArray))
	{
		ReconstructNode();
		GetGraph()->NotifyGraphChanged();
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}

void UGAS_Utility_MakeVariableContainerNode::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	// actions get registered under specific object-keys; the idea is that
	// actions might have to be updated (or deleted) if their object-key is
	// mutated (or removed)... here we use the node's class (so if the node
	// type disappears, then the action should go with it)
	UClass* ActionKey = GetClass();

	// to keep from needlessly instantiating a UBlueprintNodeSpawner, first
	// check to make sure that the registrar is looking for actions of this type
	// (could be regenerating actions for a specific asset, and therefore the
	// registrar would only accept actions corresponding to that asset)
	if (ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(NodeSpawner != nullptr);

		ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
	}
}

FText UGAS_Utility_MakeVariableContainerNode::GetMenuCategory() const
{
	static FNodeTextCache CachedCategory;

	if (CachedCategory.IsOutOfDate(this))
	{
		// FText::Format() is slow, so we cache this to save on performance
		CachedCategory.SetCachedText(FEditorCategoryUtils::BuildCategoryString(FCommonEditorCategory::Utilities, LOCTEXT("ActionMenuCategory", "GAS_Utility")), this);
	}
	return CachedCategory;
}

void UGAS_Utility_MakeVariableContainerNode::CreateProjectionPins(UEdGraphPin* Source)
{
	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();

	for (TArray<FGAS_Utility_NamedType>::TIterator it(Inputs); it; ++it)
	{
		FName Type;
		FName SubCategory;
		UObject* Subtype = nullptr;

		switch ((*it).Type)
		{
		case EGAS_UtilityEditor_VariableType::Variable_Bool:
			Type = K2Schema->PC_Boolean;
			break;

		case EGAS_UtilityEditor_VariableType::Variable_Number:
			Type = K2Schema->PC_Real;
			SubCategory = K2Schema->PC_Double;
			break;

		case EGAS_UtilityEditor_VariableType::Variable_String:
			Type = K2Schema->PC_String;
			break;

		case EGAS_UtilityEditor_VariableType::Variable_Object:
			Type = K2Schema->PC_Object;
			Subtype = UObject::StaticClass();
			break;

		case EGAS_UtilityEditor_VariableType::Variable_Vector:
			Type = K2Schema->PC_Struct;
			Subtype =TBaseStructure<FVector>::Get();
			//Subtype = TVariantStructure<FVector3f>::Get();
			
			break;
		case EGAS_UtilityEditor_VariableType::Variable_Tag:
			Type = K2Schema->PC_Struct;
			if ((*it).bIsArray)
				Subtype = FGameplayTagContainer::StaticStruct();
			else
				Subtype = FGameplayTag::StaticStruct();
			break;
		}

		UEdGraphNode::FCreatePinParams InputPinParams;
		if ((*it).Type != EGAS_UtilityEditor_VariableType::Variable_Tag)
			InputPinParams.ContainerType = (*it).bIsArray ? EPinContainerType::Array : EPinContainerType::None;
		UEdGraphPin* InputPin = CreatePin(EGPD_Input, Type, SubCategory, Subtype, (*it).Name, InputPinParams);

		InputPin->SetSavePinIfOrphaned(false);
	}
}

FText UGAS_Utility_MakeVariableContainerNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("GAS_Utility_Make_VariableContainer.NodeTitle", "Make VariableContainer");
}

#undef LOCTEXT_NAMESPACE
