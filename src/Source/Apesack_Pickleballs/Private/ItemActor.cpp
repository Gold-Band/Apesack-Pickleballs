// Fill out your copyright notice in the Description page of Project Settings.

#include "ItemActor.h"
#include "PaperSpriteComponent.h"
#include "Components/BoxComponent.h"

// Sets default values
AItemActor::AItemActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AItemActor::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	TagContainer = ItemTags;
}

void AItemActor::PickedUp(AActor* ActorPickingUp)
{
	OnPickedUp(ActorPickingUp);
}
