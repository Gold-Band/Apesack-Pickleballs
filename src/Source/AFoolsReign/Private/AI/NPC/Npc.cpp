// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/NPC/Npc.h"

#include "FCTween.h"
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
		MainSide = ADefaultGameMode::GetActorSideFromOrigin(this); 
	}
}

float ANpc::GetDirectionTo(const FVector& Location) const
{
	return FVector::DotProduct(Location - GetActorLocation(), GetActorForwardVector()) > 0 ? 1.0f : -1.0f;
}

float ANpc::GetSpeed() const
{
	if (bIsLerping) return 10;
	if (!MovementComp) return 0;
	return MovementComp->Velocity.SquaredLength() / 100;
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
	Stats = Cast<UStatsComponent>(GetComponentByClass<UStatsComponent>());
	
	if (GetSideCheckCondition()) MainSide = ADefaultGameMode::GetActorSideFromOrigin(this); 
	
	if (NpcManager) NpcManager->AddNpc(this, NpcType, MainSide);
	
	CreateBehaviours();
	
	if (Stats)
	{
		Stats->OnDeathDelegate.AddUniqueDynamic(this, &ThisClass::OnDeath);
		Stats->OnDamagedDelegate.AddUniqueDynamic(this, &ThisClass::OnDamaged);
	}
}

void ANpc::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (NpcManager) NpcManager->RemoveNpc(this, NpcType);
	Super::EndPlay(EndPlayReason);
}

void ANpc::CreateBehaviours()
{
	FAction WaitAction{FString("Wait")};
	WaitAction.Func = [&](const float DeltaTime){return Wait(DeltaTime);};
	
	// Wait
	WaitTask.Actions.Add(WaitAction);
	HtnDomain->AssignTask(&WaitTask);
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

EActionState ANpc::Wait(float DeltaTime)
{
	return EActionState::Succeeded;
}

void ANpc::OnDeath_Implementation()
{
	if (NpcManager) NpcManager->RemoveNpc(this, NpcType);
	HtnDomain->SetComponentTickEnabled(false);
}

void ANpc::OnDamaged(float DamageRecieved, float UpdatedHealth, int DamageType, AActor* InstigatorActor)
{
	if (bWasHit) return; // once at a time
	bWasHit = true;
	
	if (bPrintDebug_Knockback) 
	{
		//UE_LOG(LogTemp, Warning, TEXT("Knockback"));
		//UE_LOG(LogTemp, Warning, TEXT("DamageType = %i"), DamageType);
		if (InstigatorActor) UE_LOG(LogTemp, Warning, TEXT("Hit by = %s"), *InstigatorActor->GetActorNameOrLabel());
	}
	
	if (!InstigatorActor) return;

	const float Angle = ADefaultGameMode::GetAngleBetweenVectors(GetActorLocation(), InstigatorActor->GetActorLocation());
	const int InstigatorDirection = Angle>0? -1 : 1;
	
	
	const float KnockDistance = DamageType == 1? MeleeKnockbackParams.KnockedDistance : RangedKnockbackParams.KnockedDistance;
	const FVector Start = GetActorLocation();
	const FVector End = Start - GetActorForwardVector() * KnockDistance * InstigatorDirection;
	const float Duration = 0.3f; 
	
	// move back
	FCTween::Play(
	Start,
	End,
	[&](const FVector& t)
	{
		if (!this) return;
		SetActorLocation(t);
	},
	Duration,
	EFCEase::OutQuad)->SetOnComplete([&]()
	{
		if (!this) return;
		bWasHit = false;
	})->SetAutoDestroy(true);
	
	// jump
	FCTween::Play(
	Start,
	Start + FVector::UpVector * (DamageType == 1? 
		MeleeKnockbackParams.KnockedDistance: 
		RangedKnockbackParams.KnockedDistance), // more knockback on melee
	[&](const FVector& t)
	{
		if (!this) return;
		const FVector Location = GetActorLocation();
		SetActorLocation(FVector{Location.X, Location.Y, t.Z});
	},
	Duration/4,
	EFCEase::OutQuad)->SetYoyo(true)->SetAutoDestroy(true);
}
