// Fill out your copyright notice in the Description page of Project Settings.
#include "AI/NPC/NpcFriendly.h"
#include "StatsComponent.h"
#include "AI/Actions/Action.h"
#include "AI/HTN/HTNComponent.h"
#include "AI/HTN/ListItemObject.h"
#include "Apesack_Pickleballs/PlayerCharacter.h"
#include "Buildings/ArcherTower.h"
#include "GameModes/DefaultGameMode.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Managers/NpcManager.h"
#include "Movement/CircularPawnMovementComponent.h"
#include "Projectiles/Arrow.h"

// Sets default values
ANpcFriendly::ANpcFriendly()
{
	NpcType = ENpcTag::Friendly;
	
	GetSideInterval = 1;
}

void ANpcFriendly::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Melee cooldown
	if (!bEnabled_MeleeAttack &&
		(CooldownTimer_MeleeAttack += DeltaSeconds) >= Cooldown_MeleeAttack)
	{
		bEnabled_MeleeAttack = true;
	}
	
	// Ranged Cooldown
	if (!bEnabled_RangedAttack &&
		(CooldownTimer_RangedAttack += DeltaSeconds) >= Cooldown_RangedAttack)
	{
		bEnabled_RangedAttack = true;
	}
	
	// Follow Player Cooldown
	if (bIsPartyMember && 
		!bEnabled_FollowPlayer &&
		(CooldownTimer_FollowPlayer += DeltaSeconds) >= Cooldown_FollowPlayer)
	{
		bEnabled_FollowPlayer = true;
	}
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
	
	WaitAction.ExecutionDelegate.BindUObject(this, &ThisClass::Wait);
	
	TargetPlayerAction.ExecutionDelegate.BindUObject(this, &ThisClass::TargetPlayer);
	TargetPlayerAction.ConditionDelegate.BindUObject(this, &ThisClass::TargetPlayerCondition);
	
	MoveToAction.ConditionDelegate.BindUObject(this, &ThisClass::MoveToCondition);
	MoveToAction.ExecutionDelegate.BindUObject(this, &ThisClass::MoveTo);
	MoveToAction.ResetDelegate.BindUObject(this, &ThisClass::MoveToReset);
	
	MoveToVectorAction.ConditionDelegate.BindUObject(this, &ThisClass::MoveToCondition);
	MoveToVectorAction.ExecutionDelegate.BindUObject(this, &ThisClass::MoveToVector);
	
	MoveToOffsetAction.ConditionDelegate.BindUObject(this, &ThisClass::MoveToCondition);
	MoveToOffsetAction.ExecutionDelegate.BindUObject(this, &ThisClass::MoveToOffset);
	
	TargetNearestEnemyAction.ExecutionDelegate.BindUObject(this, &ThisClass::TargetNearestEnemy);
	
	// Melee Attack
	MeleeAttackAction.ConditionDelegate.BindUObject(this, &ThisClass::MeleeAttackCondition);
	MeleeAttackAction.ExecutionDelegate.BindUObject(this, &ThisClass::MeleeAttack);
	
	SetMeleeParamsAction.ExecutionDelegate.BindUObject(this, &ThisClass::SetMeleeParams);
	
	// Cooldown
	CooldownAction.ExecutionDelegate.BindUObject(this, &ThisClass::Cooldown);
	CooldownAction.ResetDelegate.BindUObject(this, &ThisClass::CooldownReset);
	
	// Targetting - Tower
	TargetFarthestTowerAction.ConditionDelegate.BindUObject(this, &ThisClass::TargetFarthestTowerCondition);
	TargetFarthestTowerAction.ExecutionDelegate.BindUObject(this, &ThisClass::TargetFarthestTower);
	
	// Occupy Tower
	OccupyTowerAction.ConditionDelegate.BindUObject(this, &ThisClass::OccupyTowerCondition);
	OccupyTowerAction.ExecutionDelegate.BindUObject(this, &ThisClass::OccupyTower);
	
	// Ranged Attack
	RangedAttackAction.ConditionDelegate.BindUObject(this, &ThisClass::RangedAttackCondition);
	RangedAttackAction.ExecutionDelegate.BindUObject(this, &ThisClass::RangedAttack);
	
	SetRangedParamsAction.ExecutionDelegate.BindUObject(this, &ThisClass::SetRangedParams);
	
	
	MoveTimedAction.ConditionDelegate.BindUObject(this, &ThisClass::MoveTimedCondition);
	MoveTimedAction.ExecutionDelegate.BindUObject(this,&ThisClass::MoveTimed);
	MoveTimedAction.ResetDelegate.BindUObject(this, &ThisClass::MoveTimedReset);
	
	GetDefensePositionAction.ConditionDelegate.BindUObject(this, &ThisClass::GetDefensePositionCondition);
	GetDefensePositionAction.ExecutionDelegate.BindUObject(this, &ThisClass::GetDefensePosition);
	
	OnAssumedDefensePositionAction.ExecutionDelegate.BindUObject(this, &ThisClass::OnAssumedDefensePosition);
	
	OnJoinedPlayerAction.ExecutionDelegate.BindUObject(this, &ThisClass::OnJoinedPlayer);
}

void ANpcFriendly::CreateBehaviours()
{
	Super::CreateBehaviours();
	
	// goto safe zone
	
	
	// defend wall
	DefendWallTask.Actions.Add(&GetDefensePositionAction);
	DefendWallTask.Actions.Add(&MoveToVectorAction);
	DefendWallTask.Actions.Add(&OnAssumedDefensePositionAction);
	HtnDomain->AssignTask(&DefendWallTask);
	
	
	// Follow 
	FollowTask.Actions.Add(&TargetPlayerAction);
	FollowTask.Actions.Add(&MoveToOffsetAction);
	FollowTask.Actions.Add(&OnJoinedPlayerAction);
	HtnDomain->AssignTask(&FollowTask);
	
	
	// Melee Attack
	MeleeAttackTask.Actions.Add(&SetMeleeParamsAction);
	MeleeAttackTask.Actions.Add(&TargetNearestEnemyAction);
	//MeleeAttackTask.Actions.Add(&MoveToAction);
	MeleeAttackTask.Actions.Add(&MeleeAttackAction);
	HtnDomain->AssignTask(&MeleeAttackTask);
	
	
	// Occupy Tower
	OccupyTowerTask.Actions.Add(&TargetFarthestTowerAction);
	OccupyTowerTask.Actions.Add(&MoveToAction);
	OccupyTowerTask.Actions.Add(&OccupyTowerAction);
	OccupyTowerTask.bPrintDebug = bPrintDebug_TargetFurthestTower;
	OccupyTowerTask.bResetOnFail = true;
	HtnDomain->AssignTask(&OccupyTowerTask);
	
	// Ranged Attack
	RangedAttackTask.Actions.Add(&SetRangedParamsAction);
	RangedAttackTask.Actions.Add(&TargetNearestEnemyAction);
	RangedAttackTask.Actions.Add(&RangedAttackAction);
	RangedAttackTask.bPrintDebug = bPrintDebug_RangedAttack;
	HtnDomain->AssignTask(&RangedAttackTask);
	
	// Wander
	WanderTask.Actions.Add(&MoveTimedAction);
	HtnDomain->AssignTask(&WanderTask);
	
	// Wait
	WaitTask.Actions.Add(&WaitAction);
	HtnDomain->AssignTask(&WaitTask);
}

TArray<UListItemObject*> ANpcFriendly::GetInfo() const
{
	TArray<UListItemObject*> Info{};
	
	// hp
	UListItemObject* HpInfo = NewObject<UListItemObject>();
	HpInfo->DisplayText = FText::FromString(FString::Printf(TEXT("Hp: %i/%i"), FMath::RoundToInt(Stats->GetHealth()), FMath::RoundToInt(Stats->GetMaxHealth())));
	
	// class
	FString ClassTxt = "?";
	switch (CharacterClass)
	{
	case ECharacterType::Peasant:
		ClassTxt = TEXT("Peasant");
		break;
	case ECharacterType::Fighter:
		ClassTxt = TEXT("Fighter");
		break;
	case ECharacterType::Archer:
		ClassTxt = TEXT("Archer");
		break;
	case ECharacterType::Builder:
		ClassTxt = TEXT("Builder");
		break;
	}
	
	UListItemObject* ClassInfo = NewObject<UListItemObject>();
	ClassInfo->DisplayText = FText::FromString(FString::Printf(TEXT("Class: %s"), *ClassTxt));
	
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

TArray<UListItemObject*> ANpcFriendly::GetActions()
{
	TArray<UListItemObject*> Actions{};
	
	
	if (CharacterClass != ECharacterType::Peasant)
	{ // set peasant
		UListItemObject* Action = NewObject<UListItemObject>();
		Action->DisplayText = FText::FromString(TEXT("Set Peasant"));
		Action->ContextActor = this;
		const TFunction<void()> Func = [&](){CharacterClass = ECharacterType::Peasant;};
		Action->OnActionCalledFunction = Func;
		Action->Cost = 0;
		Actions.Add(Action);
	}
	if (CharacterClass != ECharacterType::Archer) 
	{ // set archer
		UListItemObject* Action = NewObject<UListItemObject>();
		Action->DisplayText = FText::FromString(TEXT("Set Archer"));
		Action->ContextActor = this;
		const TFunction<void()> Func = [&](){CharacterClass = ECharacterType::Archer;};
		Action->OnActionCalledFunction = Func;
		Action->Cost = 0;
		Actions.Add(Action);
	}
	if (CharacterClass != ECharacterType::Fighter)
	{ // set fighter
		UListItemObject* Action = NewObject<UListItemObject>();
		Action->DisplayText = FText::FromString(TEXT("Set Fighter (Coming Soon!)"));
		Action->ContextActor = this;
		const TFunction<void()> Func = [&]()
		{
			CharacterClass = ECharacterType::Fighter;
			MainSide = NpcManager->SuggestOccupySide();
		};
		Action->OnActionCalledFunction = Func;
		Action->bDisable = true;
		Action->Cost = 0;
		Actions.Add(Action);
	}
	if (CharacterClass != ECharacterType::Builder)
	{ // set builder
		UListItemObject* Action = NewObject<UListItemObject>();
		Action->DisplayText = FText::FromString(TEXT("Set Builder (Coming Soon!)"));
		Action->ContextActor = this;
		const TFunction<void()> Func = [&](){CharacterClass = ECharacterType::Builder;};
		Action->OnActionCalledFunction = Func;
		Action->bDisable = true;
		Action->Cost = 0;
		Actions.Add(Action);
	}
	
	if (CharacterClass != ECharacterType::Peasant && !bIsPartyMember)
	{ // join party
		UListItemObject* Action = NewObject<UListItemObject>();
		const int Size =  NpcManager->GetPlayer()->PartySize;
		const int MaxSize = NpcManager->GetPlayer()->PartyOrder.Num();
		if (Size >= MaxSize) Action->bDisable = true;
		Action->DisplayText = FText::FromString(FString::Printf(TEXT("Join Party (%i/%i)"), Size, MaxSize));
		Action->ContextActor = this;
		const TFunction<void()> Func = [&](){JoinParty();};
		Action->OnActionCalledFunction = Func;
		Actions.Add(Action);
	}
	else if (bIsPartyMember) 
	{ // leave party
		UListItemObject* Action = NewObject<UListItemObject>();
		Action->DisplayText = FText::FromString(TEXT("Leave Party"));
		Action->ContextActor = this;
		const TFunction<void()> Func = [&](){LeaveParty();};
		Action->OnActionCalledFunction = Func;
		Actions.Add(Action);
	}
	
	return Actions;
}

void ANpcFriendly::OnNightStarted()
{
	bIsNighttime = true;	
	bAssumedPosition = false;
}

void ANpcFriendly::OnNightEnded()
{
	bIsNighttime = false;	
	// = false;
}

void ANpcFriendly::OnRaidDetected(EOriginSide Side)
{
	if (Side != MainSide) return;
	bRaid = true;
	// code
}


bool ANpcFriendly::GetSideCheckCondition()
{
	return !bAssumedPosition && bCanMove && !bIsPartyMember;
}

void ANpcFriendly::JoinParty()
{
	bIsPartyMember = true;
	
	bAssumedPosition = false;
	DefendWallTask.Reset();
	
	bEnabled_FollowPlayer = true;
	MainSide = EOriginSide::Any;
	
	NpcManager->GetPlayer()->PartySize++;
	
	TArray<bool>* Arr = &NpcManager->GetPlayer()->PartyOrder;
	for (PartyIndex = 0; (*Arr)[PartyIndex] == true && PartyIndex < Arr->Num() ; PartyIndex++);
	NpcManager->GetPlayer()->PartyOrder[PartyIndex] = true;
	
	if (PartyIndex == 0)
	{
		OffsetAngle = 0.2f;
	}
	else if (PartyIndex == 1)
	{
		OffsetAngle = -0.2f;
	}
	else if (PartyIndex == 2)
	{
		OffsetAngle = 0.4f;
	}
	else if (PartyIndex == 3)
	{
		OffsetAngle = -0.4f;
	}
	
	if (bPrintDebug_TargetPlayer) UE_LOG(LogTemp, Warning, TEXT("Joined Party"));
}

void ANpcFriendly::LeaveParty()
{
	if (bPrintDebug_TargetPlayer) UE_LOG(LogTemp, Warning, TEXT("Leave party"));
	bIsPartyMember = false;
	//NpcType = ENpcTag::Friendly;
	bEnabled_FollowPlayer = false;
	
	if (MovementComp) MovementComp->MaxSpeed = MoveSpeed;
	
	NpcManager->GetPlayer()->OnMovedDelegate.RemoveAll(this);
	
	NpcManager->GetPlayer()->PartySize--;

	NpcManager->GetPlayer()->PartyOrder[PartyIndex]= false;
	
	FollowTask.Reset();
}


//*
//* ACTIONS
//*


void ANpcFriendly::Wait(float DeltaTime)
{
	WaitAction.State = EActionState::Succeeded;
}

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
	return bCanMove && !bIsPartyMember && !bIsNighttime/* && !bAssumedPosition*/;	
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
		if (bPrintDebug_MoveTo) UE_LOG(LogTemp, Warning, TEXT("mans null"));
		MoveToAction.State = EActionState::Failed;
		return;
	}
	
	// Are we there yet?
	const float DistanceSquared = FVector::DistSquaredXY(GetActorLocation(), TargetActor->GetActorLocation());
	if (Timer >= RaycastInterval && DistanceSquared <= StartRaycastingDistanceSquared)
	{
		Timer = 0;
		
		UKismetSystemLibrary::LineTraceMulti(
			GetWorld(), // world
			GetActorLocation(), // start 
			TargetActor->GetActorLocation(), // end 
			UEngineTypes::ConvertToTraceType(ECC_Visibility), // channel
			false,
			TArray<AActor*>{this}, // ignore 
			bPrintDebug_MoveTo? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None, // debug
			HitResults,
			true);
		
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
					if (bIsPartyMember)
					{
						CooldownTimer_FollowPlayer = 0;
					}
					return;
				}
			}
		}
	}
	Timer+=DeltaTime;
	
	// Move
	const float Direction = FVector::DotProduct(TargetActor->GetActorLocation() - GetActorLocation(), GetActorForwardVector()) > 0 ? 1.0f : -1.0f;
	MoveForwardScaled(Direction);
}

void ANpcFriendly::MoveToVector(float DeltaTime)
{
	// Are we there yet?
	const float DistanceSquared = FVector::DistSquaredXY(GetActorLocation(), TargetLocation);
	if (DistanceSquared <= StopDistance)
	{
		MoveToVectorAction.State = EActionState::Succeeded;
		return;
	}
	
	// Move
	const float Direction = FVector::DotProduct(TargetLocation - GetActorLocation(), GetActorForwardVector()) > 0 ? 1.0f : -1.0f;
	MoveForwardScaled(Direction);
}

void ANpcFriendly::MoveToOffset(float DeltaTime)
{
	if (TargetActor == nullptr)
	{
		if (bPrintDebug_MoveTo) UE_LOG(LogTemp, Warning, TEXT("mans null"));
		MoveToOffsetAction.State = EActionState::Failed;
		return;
	}
	
	TargetLocation = NpcManager->GetPlayer()->GetActorLocation().RotateAngleAxis(OffsetAngle, FVector::UpVector).GetUnsafeNormal2D() * Radius;
	
	// Are we there yet?
	const float DistanceSquared = FVector::DistSquaredXY(GetActorLocation(), TargetLocation);
	if (DistanceSquared <= StopDistance)
	{
		MoveToOffsetAction.State = EActionState::Succeeded;
		return;
	}
	
	// Move
	const float Direction = FVector::DotProduct(TargetLocation - GetActorLocation(), GetActorForwardVector()) > 0 ? 1.0f : -1.0f;
	MoveForwardScaled(Direction);
}

bool ANpcFriendly::MoveToCondition() const
{
	return bCanMove == true;
}

void ANpcFriendly::MoveToReset()
{
	Timer = RaycastInterval;
	TargetActor = nullptr;
}



void ANpcFriendly::TargetPlayer(float DeltaTime)
{
	if (bPrintDebug_TargetPlayer) UE_LOG(LogTemp, Warning, TEXT("Targetting the player"));
	TargetActor = NpcManager->GetPlayer();
	
	if (TargetActor != nullptr) TargetPlayerAction.State = EActionState::Succeeded;
	else TargetPlayerAction.State = EActionState::Failed;
}

bool ANpcFriendly::TargetPlayerCondition() const
{
	return bIsPartyMember == true && bEnabled_FollowPlayer == true;
}

void ANpcFriendly::OnJoinedPlayer(float DeltaTime)
{
	CooldownTimer_FollowPlayer = 0;
	bEnabled_FollowPlayer = false;
	// copy player's movement
	if (bPrintDebug_TargetPlayer) UE_LOG(LogTemp, Warning, TEXT("Joined player"));
	NpcManager->GetPlayer()->OnMovedDelegate.AddUObject(this, &ThisClass::CopyPlayerMovement);
	OnJoinedPlayerAction.State = EActionState::Succeeded;
}

void ANpcFriendly::CopyPlayerMovement(float Direction, float Speed)
{
	if (MovementComp) MovementComp->MaxSpeed = Speed;
	else UE_LOG(LogTemp, Warning, TEXT("my very own movement component is null ;-;"))
	AddMovementInput(GetActorForwardVector(), Direction);
}


void ANpcFriendly::TargetNearestEnemy(float DeltaTime)
{
	TargetActor = NpcManager->FindNearestNpc(GetActorLocation(), ENpcSearchOption::AnyHostile, MainSide, FMath::Square(TargetingDistance));
	
	if (bPrintDebug_TargetNearestEnemy)
	{
		if (TargetActor) DrawDebugLine(GetWorld(), GetActorLocation(), TargetActor->GetActorLocation(), FColor::Yellow, false, 0.05f);
		UE_LOG(LogTemp, Warning, TEXT("NearestEnemy = %s"), TargetActor? TEXT("Valid"): TEXT("Null"));
	}
	
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
		CooldownTimer_MeleeAttack = 0;
		bEnabled_MeleeAttack = false;
		return;
	}
	
	// get target's stat component
	UStatsComponent* TargetStatComponent = TargetActor->GetComponentByClass<UStatsComponent>();
	if (TargetStatComponent == nullptr)
	{
		MeleeAttackAction.State = EActionState::Failed;
		CooldownTimer_MeleeAttack = 0;
		bEnabled_MeleeAttack = false;
		return;		
	}
	
	const float Damage = Stats->GetMeleeDamage(BaseDamage_MeleeAttack);
	TargetStatComponent->ApplyDamagePatch(Damage);
	
	MeleeAttackAction.State = EActionState::Succeeded;
	CooldownTimer_MeleeAttack = 0;
	bEnabled_MeleeAttack = false;
	//Delay = Cooldown_MeleeAttack;
}

bool ANpcFriendly::MeleeAttackCondition() const
{
	return CharacterClass == ECharacterType::Fighter && bEnabled_MeleeAttack;
}

void ANpcFriendly::SetMeleeParams(float DeltaTime)
{
	TargetingDistance = TargetingDistance_Melee;
	SetMeleeParamsAction.State = EActionState::Succeeded;
}


void ANpcFriendly::RangedAttack(float DeltaTime)
{
	// get target
	if (TargetActor == nullptr)
	{
		RangedAttackAction.State = EActionState::Failed;
		bEnabled_RangedAttack = false;
		CooldownTimer_RangedAttack = 0;
		return;
	}
	
	//* 1. Get an arrow from the gamemode
	AArrow* Arrow = Cast<AArrow>(Cast<ADefaultGameMode>(GetWorld()->GetAuthGameMode())->GetArrow());
	if (!Arrow)
	{
		if (bPrintDebug_RangedAttack) UE_LOG(LogTemp, Warning, TEXT("Can't get an arrow"));
		RangedAttackAction.State = EActionState::Failed;
		bEnabled_RangedAttack = false;
		CooldownTimer_RangedAttack = 0;
		return;
	}
	
	//* 2. Get my statsComponent and pass some information down to the arrow
	Arrow->Damage = Stats->GetRangedDamage(BaseDamage_MeleeAttack);
	Arrow->bDrawPathDebug = bPrintDebug_RangedAttack;
	
	//* 3. Call Launch At
	TArray<AActor*> IgnoreActors = NpcManager->GetNpcs(ENpcSearchOption::AnyFriendly, MainSide);
	IgnoreActors.Add(this);
	IgnoreActors.Add(NpcManager->GetPlayer());
	if (Arrow->LaunchAt(IgnoreActors, GetProjectileSpawnLocation(), TargetActor->GetActorLocation()))
	{
		RangedAttackAction.State = EActionState::Succeeded;
	}
	else
	{
		RangedAttackAction.State = EActionState::Failed;
		if (bPrintDebug_RangedAttack) UE_LOG(LogTemp, Warning, TEXT("no solution to hit %s"), *TargetActor->GetActorNameOrLabel());
		Arrow->Disable();
	}
	CooldownTimer_RangedAttack = 0;
	bEnabled_RangedAttack = false;
}

bool ANpcFriendly::RangedAttackCondition() const
{
	return CharacterClass == ECharacterType::Archer && bEnabled_RangedAttack;
}

void ANpcFriendly::SetRangedParams(float DeltaTime)
{
	TargetingDistance = TargetingDistance_Ranged;
	SetRangedParamsAction.State = EActionState::Succeeded;
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
		NpcManager->RemoveNpc(this, NpcType, MainSide); // causes freeze if an enemy targets this
	}
	OccupyTowerAction.State = EActionState::Succeeded;
	RangedAttackTask.Reset();
}

bool ANpcFriendly::OccupyTowerCondition() const
{
	return CharacterClass == ECharacterType::Archer && bCanMove;
}

void ANpcFriendly::GetDefensePosition(float DeltaTime)
{
	const AActor* WallToDefend = BuildingsManager->GetFarthestBuilding(EBuildingType::Wall, MainSide);
	if (WallToDefend == nullptr)
	{
		GetDefensePositionAction.State = EActionState::Failed;
		return;
	}
	
	// get the wall's position vector and rotate it a random amount and make it the npc's radius
	
	FVector RotateAxis = FVector::UpVector;
	if (MainSide == EOriginSide::Left) RotateAxis = FVector::DownVector;

	const float MaxDistance = 0.5f + ExtraDistancePerPerson * (NpcManager->GetNpcs(ENpcSearchOption::AnyFriendly, MainSide).Num()/2); 
	TargetLocation = WallToDefend->GetActorLocation().RotateAngleAxis(FMath::RandRange(MinDistance, MaxDistance), RotateAxis).GetUnsafeNormal2D() * Radius;
	
	if (bPrintDebug_DefendWall)
	{
		const FVector Min = WallToDefend->GetActorLocation().RotateAngleAxis(MinDistance, RotateAxis);
		const FVector Max = WallToDefend->GetActorLocation().RotateAngleAxis(MaxDistance, RotateAxis);
		DrawDebugLine(GetWorld(), Min, Min + FVector::UpVector * 100.0f, FColor::Yellow, false, 5.f);
		DrawDebugLine(GetWorld(), Max, Max + FVector::UpVector * 100.0f, FColor::Yellow, false, 5.f);
	}
	
	GetDefensePositionAction.State = EActionState::Succeeded;
}	

bool ANpcFriendly::GetDefensePositionCondition() const
{
	return (CharacterClass == ECharacterType::Fighter || CharacterClass == ECharacterType::Archer) 
			&& !bIsPartyMember && !bAssumedPosition && bIsNighttime;
}

void ANpcFriendly::OnAssumedDefensePosition(float DeltaTime)
{
	bAssumedPosition = true;
	OnAssumedDefensePositionAction.State = EActionState::Succeeded;
}