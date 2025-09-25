// Fill out your copyright notice in the Description page of Project Settings.


#include "PrimitiveTask.h"

#include "HTNBrain.h"


ETaskCompleteType UPrimitiveTask::Run()
{
	const ETaskCompleteType ReturnValue = RecieveRun();
	if (ReturnValue == ETaskCompleteType::Success)
	{
		// change worldstate
		Cast<UHTNBrain>(OwningActor->GetComponentByClass(UHTNBrain::StaticClass()))->OnWorldStateVariableChanged(Effect);
	}
	return ReturnValue;
}

void UPrimitiveTask::Init(AActor* OwningActorRef, AActor* TargetRef)
{
	OwningActor = OwningActorRef;
	RecieveInit(OwningActorRef, TargetRef);
}
