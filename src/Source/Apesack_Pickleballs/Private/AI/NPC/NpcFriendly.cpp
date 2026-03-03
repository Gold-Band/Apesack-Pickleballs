// Fill out your copyright notice in the Description page of Project Settings.
#include "AI/NPC/NpcFriendly.h"

#include "FCTween.h"
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
	
	// Target building cooldown
	if (!bEnabled_TargetBuilding && 
		(CooldownTimer_TargetBuilding += DeltaSeconds) >= Cooldown_TargetBuilding)
	{
		bEnabled_TargetBuilding = true;
	}
	
	// Target tower cooldown
	if (!bEnabled_TargetTower && 
		(CooldownTimer_TargetTower += DeltaSeconds) >= Cooldown_TargetTower)
	{
		bEnabled_TargetTower = true;
	}
	
	
	// Follow Player Cooldown
	/*if (bIsPartyMember && 
		!bEnabled_FollowPlayer &&
		(CooldownTimer_FollowPlayer += DeltaSeconds) >= Cooldown_FollowPlayer)
	{
		bEnabled_FollowPlayer = true;
		bAssumedPosition = false;
	}*/
}

void ANpcFriendly::BeginPlay()
{
	Super::BeginPlay();

	if (CharacterName.IsEmpty())
	{
		const ADefaultGameMode* GameMode = Cast<ADefaultGameMode>(GetWorld()->GetAuthGameMode());
		if (!GameMode)
		{
#if WITH_EDITOR
			UE_LOG(LogTemp, Error, TEXT("ANpcFriendly::ANpcFriendly - Failed to get the game mode!"))		
#endif
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
	
	// Targetting - player
	//TargetPlayerAction.ExecutionDelegate.BindUObject(this, &ThisClass::TargetPlayer);
	//TargetPlayerAction.ConditionDelegate.BindUObject(this, &ThisClass::TargetPlayerCondition);
	
	// Move to
	MoveToAction.ConditionDelegate.BindUObject(this, &ThisClass::MoveToCondition);
	MoveToAction.ExecutionDelegate.BindUObject(this, &ThisClass::MoveTo);
	MoveToAction.ResetDelegate.BindUObject(this, &ThisClass::MoveToReset);
	
	MoveToVectorAction.ConditionDelegate.BindUObject(this, &ThisClass::MoveToVectorCondition);
	MoveToVectorAction.ExecutionDelegate.BindUObject(this, &ThisClass::MoveToVector);
	
	MoveToOffsetAction.ConditionDelegate.BindUObject(this, &ThisClass::TargetPlayerCondition);
	MoveToOffsetAction.ExecutionDelegate.BindUObject(this, &ThisClass::MoveToOffset);
	
	// Targetting - Enemy
	TargetNearestEnemyAction.ExecutionDelegate.BindUObject(this, &ThisClass::TargetNearestEnemy);
	TargetNearestEnemyAction.ConditionDelegate.BindUObject(this, &ThisClass::TargetNearestEnemyCondition);
	
	// Melee Attack
	MeleeAttackAction.ConditionDelegate.BindUObject(this, &ThisClass::MeleeAttackCondition);
	MeleeAttackAction.ExecutionDelegate.BindUObject(this, &ThisClass::MeleeAttack);
	
	// Cooldown
	CooldownAction.ExecutionDelegate.BindUObject(this, &ThisClass::Cooldown);
	CooldownAction.ResetDelegate.BindUObject(this, &ThisClass::CooldownReset);
	
	// Targetting - Tower
	TargetFarthestTowerAction.ConditionDelegate.BindUObject(this, &ThisClass::TargetFarthestTowerCondition);
	TargetFarthestTowerAction.ExecutionDelegate.BindUObject(this, &ThisClass::TargetFarthestTower);
	
	// Targeting - building
	TargetNearestBuildingAction.ConditionDelegate.BindUObject(this, &ThisClass::TargetBuildingCondition);
	TargetNearestBuildingAction.ExecutionDelegate.BindUObject(this, &ThisClass::TargetNearestBuilding);
	
	// Occupy Tower
	OccupyTowerAction.ConditionDelegate.BindUObject(this, &ThisClass::OccupyTowerCondition);
	OccupyTowerAction.ExecutionDelegate.BindUObject(this, &ThisClass::OccupyTower);
	
	// Ranged Attack
	RangedAttackAction.ConditionDelegate.BindUObject(this, &ThisClass::RangedAttackCondition);
	RangedAttackAction.ExecutionDelegate.BindUObject(this, &ThisClass::RangedAttack);
	
	// Move timed (aka wander)
	MoveTimedAction.ConditionDelegate.BindUObject(this, &ThisClass::MoveTimedCondition);
	MoveTimedAction.ExecutionDelegate.BindUObject(this,&ThisClass::MoveTimed);
	MoveTimedAction.ResetDelegate.BindUObject(this, &ThisClass::MoveTimedReset);
	
	// Get defense position
	GetDefensePositionAction.ConditionDelegate.BindUObject(this, &ThisClass::GetDefensePositionCondition);
	GetDefensePositionAction.ExecutionDelegate.BindUObject(this, &ThisClass::GetDefensePosition);
	
	OnAssumedDefensePositionAction.ExecutionDelegate.BindUObject(this, &ThisClass::OnAssumedDefensePosition);
	
	// Party
	//OnJoinedPlayerAction.ExecutionDelegate.BindUObject(this, &ThisClass::OnJoinedPlayer);
	
	Super::BindActions();
}

void ANpcFriendly::CreateBehaviours()
{
	// goto
	GotoTask.Actions.Add(&MoveToVectorAction);
	GotoTask.OnEndedDelegate.BindUObject(this, &ThisClass::OnGotoCompleted);
	HtnDomain->AssignTask(&GotoTask);
	
	// defend wall
	DefendWallTask.Actions.Add(&GetDefensePositionAction);
	DefendWallTask.Actions.Add(&MoveToVectorAction);
	DefendWallTask.Actions.Add(&OnAssumedDefensePositionAction);
	HtnDomain->AssignTask(&DefendWallTask);
	
	// Build
	BuildTask.Actions.Add(&TargetNearestBuildingAction);
	BuildTask.Actions.Add(&MoveToAction);
	BuildTask.Actions.Add(&MeleeAttackAction);
	HtnDomain->AssignTask(&BuildTask);
	
	// Melee Attack
	MeleeAttackTask.OnStartedDelegate.BindUObject(this, &ThisClass::SetMeleeParams);
	MeleeAttackTask.Actions.Add(&TargetNearestEnemyAction);
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
	RangedAttackTask.OnStartedDelegate.BindUObject(this, &ThisClass::SetRangedParams);
	RangedAttackTask.Actions.Add(&TargetNearestEnemyAction);
	RangedAttackTask.Actions.Add(&RangedAttackAction);
	RangedAttackTask.bPrintDebug = bPrintDebug_RangedAttack;
	HtnDomain->AssignTask(&RangedAttackTask);

	// Follow 
	//FollowTask.Actions.Add(&TargetPlayerAction);
	FollowTask.Actions.Add(&MoveToOffsetAction);
	//FollowTask.Actions.Add(&OnJoinedPlayerAction);
	HtnDomain->AssignTask(&FollowTask);
	
	// Wander
	WanderTask.Actions.Add(&MoveTimedAction);
	HtnDomain->AssignTask(&WanderTask);
	
	// adding the default Wait task
	Super::CreateBehaviours();
}

void ANpcFriendly::OnDamaged(float DamageRecieved, float UpdatedHealth, int DamageType, AActor* InstigatorActor)
{
	Super::OnDamaged(DamageRecieved, UpdatedHealth, DamageType, InstigatorActor);
	bAssumedPosition = false;
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
	
	// sum
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
		Action->Cost = 3;
		Actions.Add(Action);
	}
	if (CharacterClass != ECharacterType::Fighter)
	{ // set fighter
		UListItemObject* Action = NewObject<UListItemObject>();
		Action->DisplayText = FText::FromString(TEXT("Set Fighter"));
		Action->ContextActor = this;
		const TFunction<void()> Func = [&]()
		{
			CharacterClass = ECharacterType::Fighter;
			MainSide = NpcManager->SuggestOccupySide();
		};
		Action->OnActionCalledFunction = Func;
		Action->bDisable = false;
		Action->Cost = 3;
		Actions.Add(Action);
	}
	if (CharacterClass != ECharacterType::Builder)
	{ // set builder
		UListItemObject* Action = NewObject<UListItemObject>();
		Action->DisplayText = FText::FromString(TEXT("Set Builder"));
		Action->ContextActor = this;
		const TFunction<void()> Func = [&](){CharacterClass = ECharacterType::Builder;};
		Action->OnActionCalledFunction = Func;
		Action->bDisable = false;
		Action->Cost = 2;
		Actions.Add(Action);
	}
	
	if ((CharacterClass != ECharacterType::Peasant || CharacterClass != ECharacterType::Builder) && !bIsPartyMember)
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

void ANpcFriendly::OnClicked()
{
	Super::OnClicked();

	if (bIsPartyMember) return;

	constexpr float NewRadius = 19050;
	NewRadiusTween(NewRadius);
	
	const AActor* PlayerCharacter = NpcManager->GetPlayer();
	
	const EOriginSide Side = ADefaultGameMode::GetActorSideFrom(PlayerCharacter, this);
	if (Side == EOriginSide::Left) OffsetAngle = 0.3f;
	else if (Side == EOriginSide::Right) OffsetAngle = -0.3f;
	TargetLocation = PlayerCharacter->GetActorLocation().RotateAngleAxis(OffsetAngle, FVector::UpVector).GetClampedToSize2D(NewRadius,NewRadius);
	
	bCanMove = true;
	bGotoLocation = true; // flag to run GotoTask
}

void ANpcFriendly::OnClickedAway()
{
	if (bIsPartyMember) return;
	
	NewRadiusTween();
	GotoTask.Reset();
	TargetLocation = GetActorLocation();
	
	bCanMove = true;
	bGotoLocation = false;
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

void ANpcFriendly::NewRadiusTween(float NewRadius)
{
	FCTween::Play(
	MovementComp->Radius, NewRadius,
	[&](const float& r)
	{
		if (!this) return;
		MovementComp->Radius = r;
	},
	0.3f,
	EFCEase::OutQuad);
}

void ANpcFriendly::OnDeath_Implementation()
{
	Super::OnDeath_Implementation();
	if (bIsPartyMember) LeaveParty();
}

bool ANpcFriendly::GetSideCheckCondition()
{
	return !bAssumedPosition && bCanMove && !bIsPartyMember;
}

void ANpcFriendly::JoinParty()
{
	if (bIsPartyMember) return;
	
	// enables Movement actions to run
	bCanMove = true;
	
	// enables party related actions to run
	bIsPartyMember = true;
	
	// dont allow copying of player movement yet
	bCopyMovement = false;
	
	// if we were using GotoLocation, reset it
	if (bGotoLocation)
	{
		GotoTask.Reset();
		bGotoLocation = false;
	}
	
	// if we were in defense position, reset that task
	DefendWallTask.Reset();
	
	// makes MoveToOffset run
	bEnabled_FollowPlayer = true;
	
	MainSide = EOriginSide::Any;
	
	NpcManager->GetPlayer()->PartySize++;
	
	TArray<bool>* Arr = &NpcManager->GetPlayer()->PartyOrder;
	for (PartyIndex = 0; (*Arr)[PartyIndex] == true && PartyIndex < Arr->Num() ; PartyIndex++);
	NpcManager->GetPlayer()->PartyOrder[PartyIndex] = true;
	
	ExitFormation();
	
#if WITH_EDITOR
	if (bPrintDebug_TargetPlayer) UE_LOG(LogTemp, Warning, TEXT("Joined Party"));
#endif
	
	NpcManager->GetPlayer()->OnMovedDelegate.AddUObject(this, &ThisClass::CopyPlayerMovement);
	NpcManager->GetPlayer()->EnterBattleFormationDelegate.AddUniqueDynamic(this, &ThisClass::EnterFormation);
	NpcManager->GetPlayer()->ExitBattleFormationDelegate.AddUniqueDynamic(this, &ThisClass::ExitFormation);
}

void ANpcFriendly::LeaveParty()
{
	if (!bIsPartyMember) return;
	
#if WITH_EDITOR
	if (bPrintDebug_TargetPlayer) UE_LOG(LogTemp, Warning, TEXT("Leave party"));
#endif
	
	bIsPartyMember = false;
	//NpcType = ENpcTag::Friendly;
	bEnabled_FollowPlayer = false;
	bCopyMovement = false;
	
	if (MovementComp) MovementComp->MaxSpeed = MoveSpeed;
	
	NpcManager->GetPlayer()->OnMovedDelegate.RemoveAll(this);
	NpcManager->GetPlayer()->EnterBattleFormationDelegate.RemoveAll(this);
	NpcManager->GetPlayer()->ExitBattleFormationDelegate.RemoveAll(this);
	
	NpcManager->GetPlayer()->PartySize--;

	NpcManager->GetPlayer()->PartyOrder[PartyIndex]= false;
	
	FollowTask.Reset();
	
	NewRadiusTween();
}

void ANpcFriendly::EnterFormation(EOriginSide Side)
{
	if (!IsCombatant()) return;
	
	bAssumedPosition = false;
	float NewRadius = MovementComp->Radius;
	
	// Set radius
	if (PartyIndex == 0) NewRadius = 19000;
	else if (PartyIndex == 1) NewRadius = 19050;
	else if (PartyIndex == 2) NewRadius = 18950;
	else if (PartyIndex == 3) NewRadius = 18900;
	
	if (CharacterClass == ECharacterType::Archer)
	{
		// Set offset angle
		if (Side == EOriginSide::Left) OffsetAngle = 0.2f;
		else if (Side == EOriginSide::Right) OffsetAngle = -0.2f;
	}
	else if (CharacterClass == ECharacterType::Fighter)
	{
		// Set offset angle
		if (Side == EOriginSide::Left) OffsetAngle = 0.4f;
		else if (Side == EOriginSide::Right) OffsetAngle = -0.4f;
	}
	
	// Lerp Radius to NewRadius
	NewRadiusTween(NewRadius);
}

void ANpcFriendly::ExitFormation()
{
	bAssumedPosition = false;
	
	// reset radius
	float NewRadius = 18999 - PartyIndex;
	
	// reset offset angle
	if (PartyIndex == 0) OffsetAngle = 0.25f;
	else if (PartyIndex == 1) OffsetAngle = -0.25f;
	else if (PartyIndex == 2) OffsetAngle = 0.45f;
	else if (PartyIndex == 3) OffsetAngle = -0.45f;
	
	// Lerp Radius to NewRadius
	NewRadiusTween(NewRadius);
}

bool ANpcFriendly::IsCombatant() const
{
	return CharacterClass == ECharacterType::Fighter || CharacterClass == ECharacterType::Archer;
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
#if WITH_EDITOR
		if (bPrintDebug_MoveTimed) UE_LOG(LogTemp, Warning, TEXT("set move time to %fs"), MoveTime);
#endif
	}
	
	MoveForwardScaled(MoveDirection * WanderSpeed);
	
	Timer += DeltaTime;
	if (Timer >= MoveTime)
	{
		MoveTimedAction.State = EActionState::Succeeded;
	}
}

bool ANpcFriendly::MoveTimedCondition() const
{
	return bCanMove && !bIsPartyMember && !bIsNighttime/* && !bAssumedPosition*/ && !TargetActor;	
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
#if WITH_EDITOR
		if (bPrintDebug_MoveTo) UE_LOG(LogTemp, Warning, TEXT("mans null"));
#endif
		MoveToAction.State = EActionState::Failed;
		return;
	}
	
	// Are we there yet?
	const float DistanceSquared = FVector::DistSquaredXY(GetActorLocation(), TargetActor->GetActorLocation().GetClampedToMaxSize2D(MovementComp->Radius));
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
#if WITH_EDITOR
			UE_LOG(LogTemp, Warning, TEXT("Num actors in sight = %i%s"), HitResults.Num(), *HitActors);
			UE_LOG(LogTemp, Warning, TEXT("Target = %s"), *TargetActor->GetActorNameOrLabel());
#endif
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
	Timer+=DeltaTime;
	
	// Move
	MoveDirection = GetDirectionTo(TargetLocation);
	MoveForwardScaled(MoveDirection);
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
	MoveDirection = GetDirectionTo(TargetLocation);
	MoveForwardScaled(MoveDirection);
}

void ANpcFriendly::MoveToOffset(float DeltaTime)
{
	const APlayerCharacter* PlayerCharacter = NpcManager->GetPlayer();
	
	if (PlayerCharacter == nullptr)
	{
		MoveToOffsetAction.State = EActionState::Failed;
		bIsLerping = false;
		return;
	}
	
	TargetLocation = PlayerCharacter->GetActorLocation().RotateAngleAxis(OffsetAngle, FVector::UpVector).GetClampedToSize2D(MovementComp->Radius,MovementComp->Radius);
	
	// Are we there yet?
	const float Distance = FVector::Dist2D(GetActorLocation(), TargetLocation);
	//TotalDistance = FMath::Max(TotalDistance, Distance) + 10;
	
#if WITH_EDITOR
	if (bPrintDebug_MoveTo) UE_LOG(LogTemp, Warning, TEXT("MoveOffset Distance = %f"), Distance);
#endif
	
	if (Distance < 5)
	{
		MoveToOffsetAction.State = EActionState::Succeeded;
	
		bIsLerping = false;
		
		// start copying player movement
		bCopyMovement = true;
		
		//TotalDistance = 0;
		bAssumedPosition = true;
		
		return;
	}
		
	constexpr float Speed = 4;
	const float Alpha = Speed/Distance;
	
	// lerp to position with constant speed
	SetActorLocation(FMath::Lerp(GetActorLocation(),TargetLocation, FMath::Clamp(Alpha, 0,1)));
	
	MoveDirection = GetDirectionTo(TargetLocation);
	bIsLerping = true;
}

bool ANpcFriendly::MoveToCondition() const
{
	return bCanMove == true;
}

bool ANpcFriendly::MoveToVectorCondition() const
{
	return MoveToCondition() && bGotoLocation;
}

void ANpcFriendly::MoveToReset()
{
	Timer = RaycastInterval;
	TargetActor = nullptr;
}

void ANpcFriendly::OnGotoCompleted()
{
#if WITH_EDITOR
	UE_LOG(LogTemp,Warning,TEXT("Goto completed"))
#endif
	bGotoLocation = false;
	bCanMove = false;
}

bool ANpcFriendly::TargetPlayerCondition() const
{
	return MoveToCondition() && bIsPartyMember == true && bEnabled_FollowPlayer == true && !bAssumedPosition;
}

void ANpcFriendly::CopyPlayerMovement(float Direction, float Speed)
{
	if (!bCopyMovement) return;
	
	check(MovementComp);
	MovementComp->MaxSpeed = Speed;
	MoveDirection = Direction;
	AddMovementInput(GetActorForwardVector(), Direction);
}

void ANpcFriendly::TargetNearestEnemy(float DeltaTime)
{
	TargetActor = NpcManager->FindNearestNpc(GetActorLocation(), ENpcSearchOption::AnyHostile, MainSide, FMath::Square(TargetingDistance));
	
#if WITH_EDITOR
	if (bPrintDebug_TargetNearestEnemy)
	{
		if (TargetActor) DrawDebugLine(GetWorld(), GetActorLocation(), TargetActor->GetActorLocation(), FColor::Yellow, false, 0.05f);
		UE_LOG(LogTemp, Warning, TEXT("NearestEnemy = %s"), TargetActor? TEXT("Valid"): TEXT("Null"));
	}
#endif
	
	if (TargetActor != nullptr) TargetNearestEnemyAction.State = EActionState::Succeeded;
	else TargetNearestEnemyAction.State = EActionState::Failed;
}

bool ANpcFriendly::TargetNearestEnemyCondition() const
{
	return (bEnabled_MeleeAttack || bEnabled_RangedAttack) && IsCombatant();
}

void ANpcFriendly::TargetFarthestTower(float DeltaTime)
{
	TargetActor = BuildingsManager->GetFarthestBuilding(EBuildingType::Tower, MainSide);
	
#if WITH_EDITOR
	if (bPrintDebug_TargetFurthestTower) UE_LOG(LogTemp, Warning, TEXT("Targeting %s"), *TargetActor->GetActorNameOrLabel());
#endif
	
	if (TargetActor != nullptr) TargetFarthestTowerAction.State = EActionState::Succeeded;
	else TargetFarthestTowerAction.State = EActionState::Failed;
	
	// start cooldown
	bEnabled_TargetTower = false;
	CooldownTimer_TargetTower = 0;
}

bool ANpcFriendly::TargetFarthestTowerCondition() const
{
	return BuildingsManager->DoVacantTowersExist(MainSide); // problem
}

void ANpcFriendly::TargetNearestBuilding(float DeltaTime)
{
	TargetActor = BuildingsManager->GetNearestBuilding(GetActorLocation(), EBuildingType::Wall, MainSide, true);
	
	if (!TargetActor) TargetNearestBuildingAction.State = EActionState::Failed;
	else TargetNearestBuildingAction.State = EActionState::Succeeded;
	
	// start cooldown
	bEnabled_TargetBuilding = false; 
	CooldownTimer_TargetBuilding = 0;
}

bool ANpcFriendly::TargetBuildingCondition() const
{
	return bEnabled_TargetBuilding && !bIsNighttime && !IsCombatant();
}

void ANpcFriendly::MeleeAttack(float DeltaTime)
{
	if (!TargetActor || !Stats)
	{
		MeleeAttackAction.State = EActionState::Failed;
		CooldownTimer_MeleeAttack = 0;
		bEnabled_MeleeAttack = false;
		return;
	}

	UStatsComponent* TargetStatComponent =
		TargetActor->GetComponentByClass<UStatsComponent>();

	if (!TargetStatComponent)
	{
		MeleeAttackAction.State = EActionState::Failed;
		CooldownTimer_MeleeAttack = 0;
		bEnabled_MeleeAttack = false;
		return;		
	}
	
	if (CharacterClass == ECharacterType::Builder)
	{
		const ABuilding* Building = Cast<ABuilding>(TargetActor);
		if (Building && Building->IsDamaged())
		{
			TargetStatComponent->HealPatch(20);
			
			MeleeAttackAction.State = EActionState::InProgress;
			
			// start the cooldown
			CooldownTimer_MeleeAttack = 0;
			bEnabled_MeleeAttack = false;
			return;
		}
		TargetActor = nullptr;
		MeleeAttackAction.State = EActionState::Succeeded;
		// start the cooldown
		CooldownTimer_MeleeAttack = 0;
		bEnabled_MeleeAttack = false;
		return;
	}
	
	// 1. GET stats as struct
	FDamagePatch DamagePatch = Stats->GetDamagePatch();

	// 2. OVERRIDE specific fields
	DamagePatch.NormalDamage = 5.0f;
	DamagePatch.ProficiencyDamageType = 0.f;

	// 3. APPLY to target by unpacking struct fields
	TargetStatComponent->ApplyDamagePatch(
		this,
	    DamagePatch.NormalDamage,
	    DamagePatch.SelfLifeStealPercent,
	    DamagePatch.BaseCritChance,
	    DamagePatch.CritMultiplier,
	    DamagePatch.TotalDamageScale,
	    DamagePatch.ProficiencyDamageType,
	    DamagePatch.RangedDamageScale,
	    DamagePatch.MeleeDamageScale,
	    DamagePatch.FireDamageScale,
	    DamagePatch.PoisonDamageScale,
	    DamagePatch.MagicDamageScale,
	    DamagePatch.FireDamage,
	    DamagePatch.PoisonDamage,
	    DamagePatch.MagicDamage,
	    DamagePatch.DebuffDuration
	);
	
	MeleeAttackAction.State = EActionState::Succeeded;
	
	// start the cooldown
	CooldownTimer_MeleeAttack = 0;
	bEnabled_MeleeAttack = false;
	
#if WITH_EDITOR
	if (bPrintDebug_MeleeAttack) UE_LOG(LogTemp, Warning, TEXT("Melee attack"))
#endif
}

bool ANpcFriendly::MeleeAttackCondition() const
{
	return bEnabled_MeleeAttack && (CharacterClass == ECharacterType::Fighter || CharacterClass == ECharacterType::Builder);
}

void ANpcFriendly::SetMeleeParams()
{
	TargetingDistance = TargetingDistance_Melee;
}

void ANpcFriendly::RangedAttack(float DeltaTime)
{
	// get target
	if (TargetActor == nullptr)
	{
		RangedAttackAction.State = EActionState::Failed;
		bEnabled_RangedAttack = false;
		CooldownTimer_RangedAttack = 0;
		OnBowAttack(false);
		return;
	}

	FVector ToTarget = TargetActor->GetActorLocation() - GetActorLocation();
	ToTarget.Z = 0.f;
	const FVector LocalToTarget = GetActorTransform().InverseTransformVectorNoScale(ToTarget);
	const bool bIsFacingTarget = LocalToTarget.X > 0.f;

	OnBowAttack(bIsFacingTarget);

	//* 1. Get an arrow from the gamemode
	AArrow* Arrow = Cast<AArrow>(
		Cast<ADefaultGameMode>(GetWorld()->GetAuthGameMode())->GetArrow()
	);

	if (!Arrow)
	{
#if WITH_EDITOR
		if (bPrintDebug_RangedAttack)
			UE_LOG(LogTemp, Warning, TEXT("Can't get an arrow"));
#endif

		RangedAttackAction.State = EActionState::Failed;
		bEnabled_RangedAttack = false;
		CooldownTimer_RangedAttack = 0;
		return;
	}

	//* 2. Pass stats to arrow

	// --------------------------------
	// 1. Declare variables
	// --------------------------------
	// --------------------------------
	// 2. GET stats FROM THIS ACTOR
	// --------------------------------
	const FDamagePatch DamagePatch = Stats->GetDamagePatch();

	// --------------------------------
	// 3. APPLY stats TO ARROW
	// --------------------------------

	// Base damage (ranged-scaled)
	Arrow->Damage = 5.f;

	// Core scaling
	Arrow->TotalDamageScale = DamagePatch.TotalDamageScale;

	// Crit
	Arrow->BaseCritChance = DamagePatch.BaseCritChance;
	Arrow->CritMultiplier = DamagePatch.CritMultiplier;

	// Sustain
	Arrow->SelfLifeStealPercent = DamagePatch.SelfLifeStealPercent;

	// Damage type
	Arrow->ProficiencyDamageType = 1;

	// Damage scaling
	Arrow->RangedDamageScale = DamagePatch.RangedDamageScale;
	Arrow->MeleeDamageScale = DamagePatch.MeleeDamageScale;
	Arrow->FireDamageScale = DamagePatch.FireDamageScale;
	Arrow->PoisonDamageScale = DamagePatch.PoisonDamageScale;
	Arrow->MagicDamageScale = DamagePatch.MagicDamageScale;

	// Flat elemental damage
	Arrow->FireDamage = DamagePatch.FireDamage;
	Arrow->PoisonDamage = DamagePatch.PoisonDamage;
	Arrow->MagicDamage = DamagePatch.MagicDamage;

	// Effects
	Arrow->DebuffDuration = DamagePatch.DebuffDuration;

	Arrow->bDrawPathDebug = bPrintDebug_RangedAttack;

	//* 3. Launch
	
	if (Arrow->LaunchAt(GetProjectileSpawnLocation(),TargetActor->GetActorLocation()))
	{
		RangedAttackAction.State = EActionState::Succeeded;
	}
	else
	{
		RangedAttackAction.State = EActionState::Failed;
		Arrow->Disable();
		
#if WITH_EDITOR
		if (bPrintDebug_RangedAttack)
		{
			UE_LOG(
				LogTemp,
				Warning,
				TEXT("no solution to hit %s"),
				*TargetActor->GetActorNameOrLabel()
			);
		}
#endif
	}
	CooldownTimer_RangedAttack = 0;
	bEnabled_RangedAttack = false;
}

bool ANpcFriendly::RangedAttackCondition() const
{
	return CharacterClass == ECharacterType::Archer && bEnabled_RangedAttack;
}

void ANpcFriendly::SetRangedParams()
{
	TargetingDistance = TargetingDistance_Ranged;
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
	return CharacterClass == ECharacterType::Archer && bCanMove && !bIsPartyMember;
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
	TargetLocation = WallToDefend->GetActorLocation().RotateAngleAxis(FMath::RandRange(MinDistance, MaxDistance), RotateAxis).GetUnsafeNormal2D() * MovementComp->Radius;
	if (bPrintDebug_DefendWall)
	{
		const FVector Min = WallToDefend->GetActorLocation().RotateAngleAxis(MinDistance, RotateAxis);
		const FVector Max = WallToDefend->GetActorLocation().RotateAngleAxis(MaxDistance, RotateAxis);
		DrawDebugLine(GetWorld(), Min, Min + FVector::UpVector * 100.0f, FColor::Yellow, false, 5.f);
		DrawDebugLine(GetWorld(), Max, Max + FVector::UpVector * 100.0f, FColor::Yellow, false, 5.f);
	}
	
	bGotoLocation = true;
	GetDefensePositionAction.State = EActionState::Succeeded;
}	

bool ANpcFriendly::GetDefensePositionCondition() const
{
	return IsCombatant() && !bIsPartyMember && !bAssumedPosition && (bIsNighttime||bRaid);
}

void ANpcFriendly::OnAssumedDefensePosition(float DeltaTime)
{
	bGotoLocation = false;
	bAssumedPosition = true;
	OnAssumedDefensePositionAction.State = EActionState::Succeeded;
}