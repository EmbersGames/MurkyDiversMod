// Fill out your copyright notice in the Description page of Project Settings.


#include "BehaviourTree/Tasks/BT_Task_Blueprint_Base.h"

#include "AIController.h"
#include "BlueprintNodeHelpers.h"
#include "BehaviorTree/BTFunctionLibrary.h"
#include "BehaviourTree/Composite_Sequences/BT_Composite_Sequence_Blueprint.h"


UBT_Task_Blueprint_Base::UBT_Task_Blueprint_Base(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bShowPropertyDetails = false;
	bAbortLatentActionWhenReceiveAbort = false;
    bForceThisTask = false;
}

void UBT_Task_Blueprint_Base::SetNodeName(FString NewName)
{
    NodeName = NewName;
}

EBTNodeResult::Type UBT_Task_Blueprint_Base::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{

    if(bAbortLatentActionWhenReceiveAbort){
    // force dropping all pending latent actions associated with this blueprint
    // we can't have those resuming activity when node is/was aborted
        BlueprintNodeHelpers::AbortLatentActions(OwnerComp, *this);
    }
    CurrentCallResult = ReceiveAbortImplementations != 0 ? EBTNodeResult::InProgress : EBTNodeResult::Aborted;
    bIsAborting = true;

    if (ReceiveAbortImplementations != FBTNodeBPImplementationHelper::NoImplementation)
    {
        bStoreFinishResult = true;

        if (AIOwner != nullptr && (ReceiveAbortImplementations & FBTNodeBPImplementationHelper::AISpecific))
        {
            ReceiveAbortAI(AIOwner, AIOwner->GetPawn());
        }
        else if (ReceiveAbortImplementations & FBTNodeBPImplementationHelper::Generic)
        {
            ReceiveAbort(ActorOwner);
        }

        bStoreFinishResult = false;
    }

    return CurrentCallResult;
}

FString UBT_Task_Blueprint_Base::GetStaticDescription() const
{
    FString ReturnDesc;
    if (!bShowPropertyDetails)
        ReturnDesc = const_cast<UBT_Task_Blueprint_Base*>(this)->GetCustomDescription();
    else
        ReturnDesc = const_cast<UBT_Task_Blueprint_Base*>(this)->GetCustomDescription() + Super::GetStaticDescription();
    return ReturnDesc;
}

UBT_Composite_Sequence_Blueprint* UBT_Task_Blueprint_Base::GetActionNode()
{
    
    if (IsValid(Saved_Aether_Action_SequenceNode))
        return Saved_Aether_Action_SequenceNode;
    
    UBTCompositeNode* NodeParent = static_cast<UBTCompositeNode*>((GetParentNode()));
    if (IsValid(NodeParent))
    {
        UBT_Composite_Sequence_Blueprint* SequenceNode = Cast<UBT_Composite_Sequence_Blueprint>(NodeParent);
        if (IsValid(SequenceNode) )
        {
            Saved_Aether_Action_SequenceNode = SequenceNode;
            return SequenceNode;
        }
    }
    return nullptr;
}

#if WITH_EDITOR
void UBT_Task_Blueprint_Base::OnNodeCreated()
{
    Super::OnNodeCreated();
    OnNodeCreatedBP();
}

FName UBT_Task_Blueprint_Base::GetNodeIconName() const
{
    const FName CustomIconName = GetNodeIconBpName();
    if (!(CustomIconName.IsNone()))
        return CustomIconName;
    return Super::GetNodeIconName();
}

#endif
void UBT_Task_Blueprint_Base::SetForceThisTask(const bool NewValue)
{
    bForceThisTask = NewValue;
    if(UBT_Composite_Sequence_Blueprint* ActionNode = GetActionNode())
    {
        ActionNode->bForceThisAction = NewValue;
    }
}


bool UBT_Task_Blueprint_Base::GetForceThisTask_Implementation(UAbilitySystemComponentBase* Comp)
{
    return bForceThisTask;
}


FName UBT_Task_Blueprint_Base::GetNodeIconBpName_Implementation() const
{
    return FName();
}

void UBT_Task_Blueprint_Base::OnNodeCreatedBP_Implementation()
{
}

FString UBT_Task_Blueprint_Base::GetCustomDescription_Implementation()
{
    return "";
}
