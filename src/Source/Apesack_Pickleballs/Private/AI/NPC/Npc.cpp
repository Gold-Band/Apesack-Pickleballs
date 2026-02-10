// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/NPC/Npc.h"
#include "PaperSpriteComponent.h"
#include "StatsComponent.h"
#include "AI/HTN/HTNComponent.h"
#include "GameModes/DefaultGameMode.h"
#include "Managers/NpcManager.h"
#include "Movement/CircularPawnMovementComponent.h"

// Sets default values
ANpc::ANpc()
{
	PrimaryActorTick.bCanEverTick = true;
	
	Root = CreateDefaultSubobject<USceneComponent>(FName("Root"));
	SetRootComponent(Root);
	Root->Mobility = EComponentMobility::Movable;
	
	MovementComp = CreateDefaultSubobject<UCircularPawnMovementComponent>(TEXT("Move"));
	MovementComp->MaxSpeed = 200;
	
	HtnDomain = CreateDefaultSubobject<UHTNComponent>(TEXT("HTN"));
}

void ANpc::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	// where am i?
	if (GetSideCheckCondition() && (GetSideTimer += DeltaSeconds) >= GetSideInterval)
	{
		GetSideTimer = 0;
		const float DistanceFromOrigin = ADefaultGameMode::GetDistanceToOrigin(GetActorLocation());
		MainSide = DistanceFromOrigin < 0? EOriginSide::Left : EOriginSide::Right;
	}
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

bool ANpc::GetSideCheckCondition()
{
	return true;
}

void ANpc::OnClicked()
{
	OnActorClicked();
}

FString ANpc::GetActorName() const
{
	return GetCharacterName();
}

void ANpc::OnDeath_Implementation()
{
	if (NpcManager) NpcManager->RemoveNpc(this, NpcType, MainSide);
	HtnDomain->SetComponentTickEnabled(false);
}
