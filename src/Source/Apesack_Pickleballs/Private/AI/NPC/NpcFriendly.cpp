// Fill out your copyright notice in the Description page of Project Settings.
#include "AI/NPC/NpcFriendly.h"

#include "StatsComponent.h"
#include "AI/Actions/Action.h"
#include "AI/HTN/HTNComponent.h"
#include "Buildings/ArcherTower.h"
#include "GameModes/DefaultGameMode.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Managers/NpcManager.h"
#include "Projectiles/Arrow.h"

// Sets default values
ANpcFriendly::ANpcFriendly()
{
	PrimaryActorTick.bCanEverTick = false;
	
	
	NpcType = ENpcTag::Friendly;
}

void ANpcFriendly::BeginPlay()
{
	Super::BeginPlay();

	if (CharacterName.IsEmpty())
	{
		const ADefaultGameMode* GameMode = Cast<ADefaultGameMode>(GetWorld()->GetAuthGameMode());
		if (!GameMode)
		{
			UE_LOG(LogTemp, Error, TEXT("ANpcFriendly::ANpcFriendly - Failed to get the game mode!"))		
			return;
		}
		CharacterName = GameMode->GetRandomNpcName();
	}
	
	// bind to delegates
	UWorldClockSubsystem* ClockSubsystem = UWorldClockSubsystem::Get(GetWorld());
	ClockSubsystem->OnNightStartedDelegate.AddUniqueDynamic(this, &ThisClass::OnNightStarted);
	ClockSubsystem->OnNightEndedDelegate.AddUniqueDynamic(this, &ThisClass::OnNightEnded);
	
	UNpcManager::OnRaidDetectedDelegate.AddUObject(this, &ANpcFriendly::OnRaidDetected);
	
	BuildingsManager = UBuildingsManager::Get(GetWorld());
}

void ANpcFriendly::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// unbind from delegates
	UWorldClockSubsystem* ClockSubsystem = UWorldClockSubsystem::Get(GetWorld());
	ClockSubsystem->OnNightStartedDelegate.RemoveAll(this);
	ClockSubsystem->OnNightEndedDelegate.RemoveAll(this);

	UNpcManager::OnRaidDetectedDelegate.RemoveAll(this);
	
	Super::EndPlay(EndPlayReason);
}

void ANpcFriendly::BindActions()
{
	Super::BindActions();
	
	TargetPlayerAction.ExecutionDelegate.BindUObject(this, &ThisClass::TargetPlayer);
	
	MoveToAction.ConditionDelegate.BindUObject(this, &ThisClass::MoveToCondition);
	MoveToAction.ExecutionDelegate.BindUObject(this, &ThisClass::MoveTo);
	MoveToAction.ResetDelegate.BindUObject(this, &ThisClass::MoveToReset);
	
	TargetNearestEnemyAction.ExecutionDelegate.BindUObject(this, &ThisClass::TargetNearestEnemy);
	
	MeleeAttackAction.ConditionDelegate.BindUObject(this, &ThisClass::MeleeAttackCondition);
	MeleeAttackAction.ExecutionDelegate.BindUObject(this, &ThisClass::MeleeAttack);
	
	CooldownAction.ExecutionDelegate.BindUObject(this, &ThisClass::Cooldown);
	CooldownAction.ResetDelegate.BindUObject(this, &ThisClass::CooldownReset);
	
	TargetFarthestTowerAction.ConditionDelegate.BindUObject(this, &ThisClass::TargetFarthestTowerCondition);
	TargetFarthestTowerAction.ExecutionDelegate.BindUObject(this, &ThisClass::TargetFarthestTower);
		
	OccupyTowerAction.ConditionDelegate.BindUObject(this, &ThisClass::OccupyTowerCondition);
	OccupyTowerAction.ExecutionDelegate.BindUObject(this, &ThisClass::OccupyTower);
	
	RangedAttackAction.ConditionDelegate.BindUObject(this, &ThisClass::RangedAttackCondition);
	RangedAttackAction.ExecutionDelegate.BindUObject(this, &ThisClass::RangedAttack);
	
	MoveTimedAction.ConditionDelegate.BindUObject(this, &ThisClass::MoveTimedCondition);
	MoveTimedAction.ExecutionDelegate.BindUObject(this,&ThisClass::MoveTimed);
	MoveTimedAction.ResetDelegate.BindUObject(this, &ThisClass::MoveTimedReset);
	
}

void ANpcFriendly::CreateBehaviours()
{
	Super::CreateBehaviours();
	
	// Follow 
	FollowTask.Actions.Add(&TargetPlayerAction);
	FollowTask.Actions.Add(&MoveToAction);
	//HtnDomain->AssignTask(FollowTask, 0);
	
	// Melee Attack
	MeleeAttackTask.Actions.Add(&TargetNearestEnemyAction);
	MeleeAttackTask.Actions.Add(&MoveToAction);
	MeleeAttackTask.Actions.Add(&MeleeAttackAction);
	MeleeAttackTask.Actions.Add(&CooldownAction);
	HtnDomain->AssignTask(&MeleeAttackTask);
	
	// Occupy Tower
	OccupyTowerTask.Actions.Add(&TargetFarthestTowerAction);
	OccupyTowerTask.Actions.Add(&MoveToAction);
	OccupyTowerTask.Actions.Add(&OccupyTowerAction);
	HtnDomain->AssignTask(&OccupyTowerTask);
	
	
	// Ranged Attack
	RangedAttackTask.Actions.Add(&TargetNearestEnemyAction);
	RangedAttackTask.Actions.Add(&RangedAttackAction);
	RangedAttackTask.Actions.Add(&CooldownAction);
	RangedAttackTask.bPrintDebug = false;
	HtnDomain->AssignTask(&RangedAttackTask);
	
	// Wander
	WanderTask.Actions.Add(&MoveTimedAction);
	HtnDomain->AssignTask(&WanderTask);
}

void ANpcFriendly::OnNightStarted()
{
	
}

void ANpcFriendly::OnNightEnded()
{
	
}

void ANpcFriendly::OnRaidDetected(EOriginSide Side)
{
	if (Side != MainSide) return;
	bRaid = true;
	// code
}


//*
//* ACTIONS
//*


void ANpcFriendly::Cooldown(float DeltaTime)
{
	Timer += DeltaTime;
	if (Timer >= Delay) CooldownAction.State = EActionState::Succeeded;
}


void ANpcFriendly::CooldownReset()
{
	Timer = 0.0f;
}

void ANpcFriendly::MoveTimed(float DeltaTime)
{
	if (MoveTime == 0)
	{
		MoveTime = FMath::RandRange(WanderTimeMin, WanderTimeMax);
		MoveDirection = FMath::RandRange(-1, 1);
		if (bPrintDebug_MoveTimed) UE_LOG(LogTemp, Warning, TEXT("set move time to %fs"), MoveTime);
	}
	
	MoveForwardScaled(MoveDirection * WanderSpeed);
	
	Timer += DeltaTime;
	if (Timer >= MoveTime) MoveTimedAction.State = EActionState::Succeeded;
}

bool ANpcFriendly::MoveTimedCondition() const
{
	return bCanMove;	
}

void ANpcFriendly::MoveTimedReset()
{
	Timer = 0.0f;
	MoveTime = 0.0f;
}



void ANpcFriendly::MoveTo(float DeltaTime)
{
	// Is destination still valid?
	if (TargetActor == nullptr)
	{
		MoveToAction.State = EActionState::Failed;
		return;
	}
	
	// Are we there yet?
	const float DistanceSquared = FVector::DistSquaredXY(GetActorLocation(), TargetActor->GetActorLocation());
	if (Timer >= RaycastInterval && DistanceSquared <= StartRaycastingDistanceSquared)
	{
		Timer = 0;
		
		if (UKismetSystemLibrary::LineTraceMulti(
			GetWorld(), // world
			GetActorLocation(), // start 
			TargetActor->GetActorLocation(), // end 
			UEngineTypes::ConvertToTraceType(ECC_Visibility), // channel
			false,
			TArray<AActor*>{this}, // ignore 
			bPrintDebug_MoveTo? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None, // debug
			HitResults,
			true))
		{
			if (bPrintDebug_MoveTo)
			{
				FString HitActors;
				for (auto It = HitResults.CreateConstIterator(); It; ++It)
				{
					HitActors.Append(FString::Printf(TEXT(", %s"), *It->GetActor()->GetActorNameOrLabel()));
				}
				UE_LOG(LogTemp, Warning, TEXT("Num actors in sight = %i%s"), HitResults.Num(), *HitActors);
				UE_LOG(LogTemp, Warning, TEXT("Target = %s"), *TargetActor->GetActorNameOrLabel());
			}
			
			for (const auto& It : HitResults)
			{
				AActor* HitActor = It.GetActor();
				
				if (HitActor == TargetActor)
				{
					if (It.Distance <= StopDistance)
					{
						MoveToAction.State = EActionState::Succeeded;
						return;
					}
				}
			}
		}
	}
	Timer+=DeltaTime;
	
	// Move
	const float Direction = FVector::DotProduct(TargetActor->GetActorLocation() - GetActorLocation(), GetActorForwardVector()) > 0 ? 1.0f : -1.0f;
	MoveForwardScaled(Direction);
}

bool ANpcFriendly::MoveToCondition() const
{
	return bCanMove;
}

void ANpcFriendly::MoveToReset()
{
	Timer = RaycastInterval;
	TargetActor = nullptr;
}



void ANpcFriendly::TargetPlayer(float DeltaTime)
{
	if (bPrintDebug_TargetPlayer) UE_LOG(LogTemp, Warning, TEXT("Targetting the player"));
	TargetActor = GetWorld()->GetFirstPlayerController()->GetPawn();
	
	if (TargetActor != nullptr) TargetPlayerAction.State = EActionState::Succeeded;
	else TargetPlayerAction.State = EActionState::Failed;
}



void ANpcFriendly::TargetNearestEnemy(float DeltaTime)
{
	TargetActor = NpcManager->FindNearestNpc(GetActorLocation(), ENpcSearchOption::AnyHostile, MainSide);
	
	if (bPrintDebug_TargetNearestEnemy) UE_LOG(LogTemp, Warning, TEXT("NearestEnemy = %s"), TargetActor? TEXT("Valid"): TEXT("Null"));
	
	if (TargetActor != nullptr) TargetNearestEnemyAction.State = EActionState::Succeeded;
	else TargetNearestEnemyAction.State = EActionState::Failed;
}


void ANpcFriendly::TargetFarthestTower(float DeltaTime)
{
	TargetActor = BuildingsManager->GetFarthestBuilding(EBuildingType::Tower, MainSide);
	
	if (bPrintDebug_TargetFurthestTower) UE_LOG(LogTemp, Warning, TEXT("Targeting %s"), *TargetActor->GetActorNameOrLabel());
	
	if (TargetActor != nullptr) TargetFarthestTowerAction.State = EActionState::Succeeded;
	else TargetFarthestTowerAction.State = EActionState::Failed;
}

bool ANpcFriendly::TargetFarthestTowerCondition() const
{
	return BuildingsManager->DoVacantTowersExist(MainSide); // problem
}


void ANpcFriendly::MeleeAttack(float DeltaTime)
{
	// get target
	if (TargetActor == nullptr)
	{
		MeleeAttackAction.State = EActionState::Failed;
		return;
	}
	
	// get target's stat component
	UStatsComponent* TargetStatComponent = TargetActor->GetComponentByClass<UStatsComponent>();
	if (TargetStatComponent == nullptr)
	{
		MeleeAttackAction.State = EActionState::Failed;
		return;		
	}
	
	const float Damage = Stats->GetMeleeDamage(BaseDamage_MeleeAttack);
	TargetStatComponent->ApplyDamagePatch(Damage);
	
	MeleeAttackAction.State = EActionState::Succeeded;
	Delay = Cooldown_MeleeAttack;
}

bool ANpcFriendly::MeleeAttackCondition() const
{
	return CharacterClass == ECharacterType::Fighter;
}




void ANpcFriendly::RangedAttack(float DeltaTime)
{
	// get target
	if (TargetActor == nullptr)
	{
		RangedAttackAction.State = EActionState::Failed;
		return;
	}
	
	//* 1. Get an arrow from the gamemode
	AArrow* Arrow = Cast<AArrow>(Cast<ADefaultGameMode>(GetWorld()->GetAuthGameMode())->GetArrow());
	if (!Arrow)
	{
		if (bPrintDebug_RangedAttack) UE_LOG(LogTemp, Warning, TEXT("Can't get an arrow"));
		RangedAttackAction.State = EActionState::Failed;
		return;
	}
	
	//* 2. Get my statsComponent and pass some information down to the arrow
	Arrow->Damage = Stats->GetRangedDamage(BaseDamage_MeleeAttack);
	
	//* 3. Call Launch At
	if (Arrow->LaunchAt(this, GetProjectileSpawnLocation(), TargetActor->GetActorLocation()))
	{
		RangedAttackAction.State = EActionState::Succeeded;
		Delay = Cooldown_RangedAttack;
	}
	else
	{
		RangedAttackAction.State = EActionState::Failed;
	}
}

bool ANpcFriendly::RangedAttackCondition() const
{
	return CharacterClass == ECharacterType::Archer;
}




void ANpcFriendly::OccupyTower(float DeltaTime)
{
	AArcherTower* TargetTower = Cast<AArcherTower>(TargetActor);
	
	if (TargetTower == nullptr || !TargetTower->HasRoom())
	{
		OccupyTowerAction.State = EActionState::Failed;
		return;
	}
	
	if (bCanMove)
	{
		TargetTower->AddOccupant(this);
		bCanMove = false;
		NpcManager->RemoveNpc(this, ENpcTag::Friendly, MainSide);
	}
	OccupyTowerAction.State = EActionState::Succeeded;
}

bool ANpcFriendly::OccupyTowerCondition() const
{
	return CharacterClass == ECharacterType::Archer;
}
