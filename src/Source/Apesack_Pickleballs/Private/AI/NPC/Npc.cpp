// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/NPC/Npc.h"
#include "PaperSpriteComponent.h"
#include "StatsComponent.h"
#include "AI/HTN/HTNComponent.h"
#include "AI/HTN/ListItemObject.h"
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
}

void ANpc::OnClicked()
{
	OnActorClicked();
}

FString ANpc::GetActorName() const
{
	return GetCharacterName();
}

TArray<UListItemObject*> ANpc::GetInfo() const 
{
	TArray<UListItemObject*> Info{};
	
	// hp
	UListItemObject* HpInfo = NewObject<UListItemObject>();
	HpInfo->DisplayText = FText::FromString(FString::Printf(TEXT("Hp: %i/%i"), FMath::RoundToInt(Stats->GetHealth()), FMath::RoundToInt(Stats->GetMaxHealth())));
	
	// class
	UListItemObject* ClassInfo = NewObject<UListItemObject>();
	ClassInfo->DisplayText = FText::FromString(FString::Printf(TEXT("Class: %s"), TEXT("Peasant")));
	
	// rank
	UListItemObject* RankInfo = NewObject<UListItemObject>();
	RankInfo->DisplayText = FText::FromString(FString::Printf(TEXT("Rank: %s"), TEXT("None")));
	
	// proficiency
	UListItemObject* ProficiencyInfo = NewObject<UListItemObject>();
	ProficiencyInfo->DisplayText = FText::FromString(FString::Printf(TEXT("Proficient As: %s"), TEXT("Builder")));
	
	//sum
	Info.Add(HpInfo);
	Info.Add(ClassInfo);
	Info.Add(RankInfo);
	Info.Add(ProficiencyInfo);
	
	return Info;
}

TArray<UListItemObject*> ANpc::GetActions() 
{
	TArray<UListItemObject*> Actions{};
	
	// orders
	// upgrades
	
	/*int i = 0;
	for (auto Option : CompatibleBuildings)
	{
		if (!Option) continue;
		UListItemObject* Action = NewObject<UListItemObject>();
		Action->DisplayText = FText::FromString(Option.GetDefaultObject()->GetActorName());
		Action->Cost = Option.GetDefaultObject()->GetBuildCost();
		Action->ContextActor = this;

		const TFunction<void()> Func = [&, i](){SpawnBuilding(i);};
		Action->OnActionCalledFunction = Func;

		Actions.Add(Action);
		i++;
	}*/

	{ // set peasant
		UListItemObject* Action = NewObject<UListItemObject>();
		Action->DisplayText = FText::FromString(TEXT("Set Peasant"));
		Action->ContextActor = this;
		const TFunction<void()> Func = [&](){CharacterClass = ECharacterType::Peasant;};
		Action->OnActionCalledFunction = Func;
		Actions.Add(Action);
	}
	{ // set archer
		UListItemObject* Action = NewObject<UListItemObject>();
		Action->DisplayText = FText::FromString(TEXT("Set Archer"));
		Action->ContextActor = this;
		const TFunction<void()> Func = [&](){CharacterClass = ECharacterType::Archer;};
		Action->OnActionCalledFunction = Func;
		Actions.Add(Action);
	}
	{ // set fighter
		UListItemObject* Action = NewObject<UListItemObject>();
		Action->DisplayText = FText::FromString(TEXT("Set Fighter"));
		Action->ContextActor = this;
		const TFunction<void()> Func = [&](){CharacterClass = ECharacterType::Fighter;};
		Action->OnActionCalledFunction = Func;
		Actions.Add(Action);
	}
	{ // set builder
		UListItemObject* Action = NewObject<UListItemObject>();
		Action->DisplayText = FText::FromString(TEXT("Set Builder"));
		Action->ContextActor = this;
		const TFunction<void()> Func = [&](){CharacterClass = ECharacterType::Builder;};
		Action->OnActionCalledFunction = Func;
		Actions.Add(Action);
	}
	
	return Actions;
}
