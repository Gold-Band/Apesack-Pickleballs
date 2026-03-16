// Fill out your copyright notice in the Description page of Project Settings.


#include "Interfaces/ClickableActor.h"

bool IClickableActor::IsClickableActor() const
{
	return true;
}

void IClickableActor::OnClicked()
{

}

void IClickableActor::OnClickedAway()
{
	
}

// Add default functionality here for any IClickableActor functions that are not pure virtual.
FString IClickableActor::GetActorName() const
{
	return FString("Clickable Actor");
}

TArray<UListItemObject*> IClickableActor::GetInfo() const 
{
#if WITH_EDITOR
	UE_LOG(LogTemp, Warning, TEXT("Clickable Actor GetInfo() Not Overridden!"))
#endif
	
	return TArray<UListItemObject*>();
}

TArray<UListItemObject*> IClickableActor::GetActions()
{
#if WITH_EDITOR
	UE_LOG(LogTemp, Warning, TEXT("Clickable Actor GetActions() Not Overridden!"))
#endif
	
	return TArray<UListItemObject*>();
}
