// Fill out your copyright notice in the Description page of Project Settings.


#include "FindObjectTask.h"

#include "HTNBrain.h"
#include "Kismet/GameplayStatics.h"


void UFindObjectTask::Init(AActor* OwningActorRef, AActor* TargetRef)
{
	Super::Init(OwningActorRef, TargetRef);
}

ETaskCompleteType UFindObjectTask::Run()
{
	const ETaskCompleteType ReturnValue = RecieveRun();
	switch (ReturnValue)
	{
	case ETaskCompleteType::InProgress:
	case ETaskCompleteType::Success:
		break;
	case ETaskCompleteType::Failed:
		if (OwningActor.IsValid())
		{
			Cast<UHTNBrain>(OwningActor.Get()->GetComponentByClass(UHTNBrain::StaticClass()))->OnWorldStateVariableChanged(FWorldStateVariable(Requirement.Option, !Requirement.Value));
		}
		break;
	}
	return ReturnValue;
}

TArray<AActor*> UFindObjectTask::FindActorsWithTag(const FName& Tag) const
{
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsWithTag(OwningActor->GetWorld(), Tag, Actors);
	return Actors;
}
