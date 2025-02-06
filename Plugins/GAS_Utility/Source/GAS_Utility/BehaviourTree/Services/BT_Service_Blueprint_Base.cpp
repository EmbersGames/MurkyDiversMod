// Fill out your copyright notice in the Description page of Project Settings.


#include "BehaviourTree/Services/BT_Service_Blueprint_Base.h"

#include "BehaviorTree/BTCompositeNode.h"
#include "BehaviorTree/BTDecorator.h"
#include "Kismet/KismetMathLibrary.h"


void UBT_Service_Blueprint_Base::InitializeFromAsset(UBehaviorTree& Asset)
{
    Super::InitializeFromAsset(Asset);
    UBTCompositeNode* Parent = GetParentNode();
    if (IsValid(Parent))
    {
        InitializeInBlueprint(ActorOwner, Parent);
    }
}

void UBT_Service_Blueprint_Base::InitializeInBlueprint_Implementation(AActor* Controller, UObject* InParentNode)
{
}

UObject* UBT_Service_Blueprint_Base::GetFirstParentNodeDecoratorOfClass(UClass* ParentClass)
{
    UBTCompositeNode* Parent = GetParentNode();
    UBTDecorator* ResultVar = nullptr;
    if (IsValid(Parent))
    {
        UBTCompositeNode* ParentParent = Parent->GetParentNode();
        if (GetChildIndex() != MAX_uint8)
        {
            TArray<UBTDecorator*> Decorators = Parent->Children[GetChildIndex()].Decorators;
            for (UBTDecorator* Element : Decorators)
            {
                
                if (!ResultVar && IsValid(Element) && UKismetMathLibrary::ClassIsChildOf(Element->GetClass(),ParentClass))
                {
                    ResultVar = Element;
                }
            }
        }
        if (!ResultVar && IsValid(ParentParent))
        {
            ResultVar = static_cast<UBTDecorator*>(RecursiveSearchParentNodeDecoratorOfClass(Parent, ParentParent, ParentClass));
        }
    }


    const UBehaviorTreeComponent* OwnerComp = Cast<UBehaviorTreeComponent>(GetOuter());
    if (!IsValid(OwnerComp) || !IsValid(Parent))
        return ResultVar;
    const int32 InstanceIdx = OwnerComp->FindInstanceContainingNode(GetParentNode());
    if (InstanceIdx == INDEX_NONE)
        return ResultVar;
    if (IsValid(ResultVar))
    {
        uint8* NodeMemory = OwnerComp->GetNodeMemory(ResultVar, InstanceIdx);
        UBTDecorator* NodeOb = static_cast<UBTDecorator*>(ResultVar->GetNodeInstance(*OwnerComp, NodeMemory));
        if (IsValid(NodeOb))
            return NodeOb;
    }
    return ResultVar;
}

void UBT_Service_Blueprint_Base::SetNodeName(FString NewName)
{
    NodeName = NewName;
}

FString UBT_Service_Blueprint_Base::GetStaticDescription() const
{
    return const_cast<UBT_Service_Blueprint_Base*>(this)->GetCustomDescription();
}

UObject* UBT_Service_Blueprint_Base::GetInstanceParentNode()
{
    UBTCompositeNode* ResultVar = static_cast<UBTCompositeNode*>(const_cast<UBTNode*>(GetMyNode()));
    return ResultVar;
}

UObject* UBT_Service_Blueprint_Base::RecursiveSearchParentNodeDecoratorOfClass(UBTCompositeNode* Node, UBTCompositeNode* CurParentNode, UClass* ParentClass)
{
    if (IsValid(Node) && IsValid(CurParentNode))
    {
        const int32 ResIndex = CurParentNode->GetChildIndex(*Node);
        if (ResIndex >= 0)
        {
            TArray<UBTDecorator*> Decorators = CurParentNode->Children[ResIndex].Decorators;
            for (UBTDecorator* Element : Decorators)
            {
                if (IsValid(Element))
                {
                    if (UKismetMathLibrary::ClassIsChildOf(Element->GetClass(),ParentClass))
                    {
                        return Element;
                    }
                }
            }
            return RecursiveSearchParentNodeDecoratorOfClass(CurParentNode, CurParentNode->GetParentNode(), ParentClass);
        }
    }
    return nullptr;
}

FString UBT_Service_Blueprint_Base::GetCustomDescription_Implementation()
{
    return Super::GetStaticDescription();
}
