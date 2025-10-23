// Fill out your copyright notice in the Description page of Project Settings.

#include "ItemActor.h"
#include "PaperSpriteComponent.h"
#include "Components/BoxComponent.h"

// Sets default values
AItemActor::AItemActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	ItemCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Collider"));
	ItemCollider->SetupAttachment(GetRootComponent());

	FCollisionResponseContainer CollisionResponse;
	CollisionResponse.SetAllChannels(ECR_Ignore);
	CollisionResponse.SetResponse(ECC_WorldStatic, ECR_Block);
	ItemCollider->SetCollisionResponseToChannels(CollisionResponse);
	ItemCollider->SetCollisionObjectType(ECC_WorldDynamic);
	ItemCollider->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
	ItemCollider->CanCharacterStepUpOn = ECB_No;
	ItemCollider->SetSimulatePhysics(true);
	ItemCollider->SetGenerateOverlapEvents(true);

	RenderComponent = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Sprite"));
	RenderComponent->SetupAttachment(ItemCollider);
}

void AItemActor::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	TagContainer = ItemTags;
}

void AItemActor::PickedUp(AActor* ActorPickingUp)
{
	OnPickedUp(ActorPickingUp);
}
