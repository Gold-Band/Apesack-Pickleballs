// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/NPC/Npc.h"
#include "PaperSpriteComponent.h"
#include "StatsComponent.h"
#include "AI/Actions/Action.h"
#include "AI/HTN/HTNComponent.h"
#include "AI/HTN/ListItemObject.h"
#include "GameModes/DefaultGameMode.h"
#include "Managers/NpcManager.h"
#include "Movement/CircularPawnMovementComponent.h"

// Sets default values
ANpc::ANpc()
{
	PrimaryActorTick.bCanEverTick = false;
	
	Root = CreateDefaultSubobject<USceneComponent>(FName("Root"));
	SetRootComponent(Root);
	Root->Mobility = EComponentMobility::Movable;
	
	SpriteComp = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Sprite"));
	SpriteComp->SetupAttachment(Root);

	MovementComp = CreateDefaultSubobject<UCircularPawnMovementComponent>(TEXT("Movement"));
	MovementComp->MaxSpeed = 200;
	
	HtnDomain = CreateDefaultSubobject<UHTNComponent>(TEXT("HTN"));
}

float ANpc::GetCharacterPreferredRadius() const
{
	return Radius;
}

float ANpc::GetDirectionToTown()
{
	return OriginDirection;
	//OriginDirection = ADefaultGameMode
}

UStatsComponent* ANpc::GetStats()
{
	return Stats;
}

FVector ANpc::GetProjectileSpawnLocation_Implementation() const
{
	return FVector::Zero();
}

void ANpc::MoveForwardScaled(float Scale)
{
	AddMovementInput(GetActorForwardVector(), Scale);
}

void ANpc::BeginPlay()
{
	Super::BeginPlay();
	NpcManager = UNpcManager::Get(GetWorld());
	Radius = GetActorLocation().Size2D();
	Stats = Cast<UStatsComponent>(GetComponentByClass<UStatsComponent>());
	
	// set mainside
	const float DistanceToOrigin = ADefaultGameMode::GetDistanceToOrigin(GetActorLocation());
	if (DistanceToOrigin <= 0) MainSide = EOriginSide::Left;
	else MainSide = EOriginSide::Right;
	
	if (NpcManager) NpcManager->AddNpc(this, NpcType, MainSide);
	
	BindActions();
	CreateBehaviours();
}

void ANpc::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (NpcManager) NpcManager->RemoveNpc(this, NpcType, MainSide);
	Super::EndPlay(EndPlayReason);
}

void ANpc::BindActions()
{
}

void ANpc::CreateBehaviours()
{
}

void ANpc::OnClicked()
{
	OnActorClicked();
}

FString ANpc::GetActorName() const
{
	return GetCharacterName();
}
