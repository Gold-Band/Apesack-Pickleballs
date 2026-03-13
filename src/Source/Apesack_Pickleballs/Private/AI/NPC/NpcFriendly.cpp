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
	BuildingsManager->OnNewWallBuiltDelegate.AddUObject(this, &ThisClass::OnWallBuilt);
	BuildingsManager->OnNewArcherTowerBuiltDelegate.AddUObject(this, &ThisClass::OnTowerBuilt);
}

void ANpcFriendly::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// unbind from delegates
	UWorldClockSubsystem* ClockSubsystem = UWorldClockSubsystem::Get(GetWorld());
	ClockSubsystem->OnNightStartedDelegate.RemoveAll(this);
	ClockSubsystem->OnNightEndedDelegate.RemoveAll(this);

	UNpcManager::OnRaidDetectedDelegate.RemoveAll(this);
	BuildingsManager->OnNewWallBuiltDelegate.RemoveAll(this);
	BuildingsManager->OnNewArcherTowerBuiltDelegate.RemoveAll(this);
	
	Super::EndPlay(EndPlayReason);
}

void ANpcFriendly::CreateBehaviours()
{
	// designing actions //
	FAction MoveTimedAction{FString("Move Timed")};
	MoveTimedAction.Func = [&](const float DeltaTime){return MoveTimed(DeltaTime);};
	
	FAction MoveToAction{FString("Move To")};
	MoveToAction.Func = [&](const float DeltaTime){ return MoveTo(DeltaTime);};
	
	FAction MoveToVectorAction{FString("Move To Vector")};
	MoveToVectorAction.Func = [&](const float DeltaTime){ return MoveToVector(DeltaTime);};
	
	FAction MoveToOffsetAction{FString("Move To Offset")};
	MoveToOffsetAction.Func = [&](const float DeltaTime){ return MoveToOffset(DeltaTime);};
	
	FAction TargetNearestEnemyAction{FString("Target Enemy")};
	TargetNearestEnemyAction.Func = [&](const float DeltaTime){ return TargetNearestEnemy(DeltaTime);};
	
	FAction TargetFarthestTowerAction{FString("Target Tower")};
	TargetFarthestTowerAction.Func = [&](const float DeltaTime){ return TargetFarthestTower(DeltaTime);};
	
	FAction TargetNearestBuildingAction{FString("Target Building")};
	TargetNearestBuildingAction.Func= [&](const float DeltaTime){ return TargetNearestBuilding(DeltaTime);};
	
	FAction MeleeAttackAction{FString("Attack")};
	MeleeAttackAction.Func = [&](const float DeltaTime){ return MeleeAttack(DeltaTime);};
	
	FAction RangedAttackAction{FString("Shoot")};
	RangedAttackAction.Func = [&](const float DeltaTime){ return RangedAttack(DeltaTime);};
	
	FAction OccupyTowerAction{FString("Occupy Tower")};
	OccupyTowerAction.Func = [&](const float DeltaTime){ return OccupyTower(DeltaTime);};
	
	FAction GetDefensePositionAction{FString("Get Defense Position")};
	GetDefensePositionAction.Func =[&](const float DeltaTime){ return GetDefensePosition(DeltaTime);};
	
	
	
	
	// designing tasks //
	
	
	// goto
	GotoTask.Actions.Add(MoveToVectorAction);
	GotoTask.OnEnded = [&]{OnGotoCompleted();};
	GotoTask.Condition = [&]{return GotoCondition();};
	GotoTask.Cooldown = 0;
	
	// defend wall
	DefendWallTask.Actions.Add(GetDefensePositionAction);
	DefendWallTask.Actions.Add(MoveToVectorAction);
	DefendWallTask.OnEnded = [&]{OnGotoCompleted();};
	DefendWallTask.Condition = [&]{return GetDefensePositionCondition() && MoveCondition();};
	DefendWallTask.bPrintDebug = bPrintDebug_DefendWall;
	DefendWallTask.Cooldown = 3.f;
	
	// Occupy Tower
	OccupyTowerTask.Actions.Add(TargetFarthestTowerAction);
	OccupyTowerTask.Actions.Add(MoveToVectorAction);
	OccupyTowerTask.Actions.Add(OccupyTowerAction);
	OccupyTowerTask.Condition = [&]{return OccupyTowerCondition() && MoveCondition();};
	OccupyTowerTask.bPrintDebug = bPrintDebug_TargetFurthestTower;
	OccupyTowerTask.Cooldown = 3.f;
	
	// Build
	BuildTask.Actions.Add(TargetNearestBuildingAction);
	BuildTask.Actions.Add(MoveToVectorAction);
	BuildTask.Actions.Add(MeleeAttackAction);
	BuildTask.Condition = [&]{return MoveCondition() && TargetBuildingCondition();};
	BuildTask.Cooldown = Cooldown_MeleeAttack;
	BuildTask.OnEnded = [&]{MoveToReset();};
	
	// Melee Attack
	MeleeAttackTask.Actions.Add(TargetNearestEnemyAction);
	MeleeAttackTask.Actions.Add(MeleeAttackAction);
	MeleeAttackTask.OnStarted = [&]{SetMeleeParams();};
	MeleeAttackTask.Condition = [&]{return MeleeAttackCondition() && TargetNearestEnemyCondition();};
	MeleeAttackTask.Cooldown = Cooldown_MeleeAttack;
	
	// Ranged Attack
	RangedAttackTask.Actions.Add(TargetNearestEnemyAction);
	RangedAttackTask.Actions.Add(RangedAttackAction);
	RangedAttackTask.OnStarted = [&]{SetRangedParams();};
	RangedAttackTask.Condition = [&]{return RangedAttackCondition() && TargetNearestEnemyCondition();};
	RangedAttackTask.bPrintDebug = bPrintDebug_RangedAttack;
	RangedAttackTask.Cooldown = Cooldown_RangedAttack;

	// Follow 
	FollowTask.Actions.Add(MoveToOffsetAction);
	FollowTask.Condition = [&]{return bEnabled_FollowPlayer && MoveCondition();};
	FollowTask.Cooldown = 0;
	
	// Wander
	WanderTask.Actions.Add(MoveTimedAction);
	WanderTask.Condition = [&]{return MoveTimedCondition();};
	WanderTask.OnStarted = [&]{SetMoveTime();};
	WanderTask.OnEnded = [&]{MoveTimedReset();};
	WanderTask.Cooldown = Cooldown_Wander;
	
	// priority order
	HtnDomain->AssignTask(&OccupyTowerTask);
	HtnDomain->AssignTask(&DefendWallTask);
	HtnDomain->AssignTask(&GotoTask);
	HtnDomain->AssignTask(&RangedAttackTask);
	HtnDomain->AssignTask(&MeleeAttackTask);
	HtnDomain->AssignTask(&BuildTask);
	HtnDomain->AssignTask(&FollowTask);
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
	
	/*if (CharacterClass != ECharacterType::Peasant)
	{ // set peasant
		UListItemObject* Action = NewObject<UListItemObject>();
		Action->DisplayText = FText::FromString(TEXT("Set Peasant"));
		Action->ContextActor = this;
		const TFunction<void()> Func = [&]()
		{
			
			CharacterClass = ECharacterType::Peasant;
			SetInitialWeaponType(0);
		
		};
		Action->OnActionCalledFunction = Func;
		Action->Cost = 0;
		Actions.Add(Action);
		
	}
	if (CharacterClass != ECharacterType::Archer) 
	{ // set archer
		UListItemObject* Action = NewObject<UListItemObject>();
		Action->DisplayText = FText::FromString(TEXT("Set Archer"));
		Action->ContextActor = this;
		const TFunction<void()> Func = [&]()
		{
			CharacterClass = ECharacterType::Archer;
			SetInitialWeaponType(0);
		};
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
			SetInitialWeaponType(1);
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
		const TFunction<void()> Func = [&]()

		{
			CharacterClass = ECharacterType::Builder; 
			SetInitialWeaponType(2);
		
		};
		Action->OnActionCalledFunction = Func;
		Action->bDisable = false;
		Action->Cost = 2;
		Actions.Add(Action);
	
	}*/
	
	if (!bIsPartyMember)
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
	TargetLocation = PlayerCharacter->GetActorLocation().RotateAngleAxis(OffsetAngle, FVector::UpVector);//.GetClampedToSize2D(NewRadius,NewRadius);
	
	
	bCanMove = true;
	bGotoLocation = true; // flag to run GotoTask
	bIsClicked = true;
	
	GotoTask.Reset(); // should it auto reset? for some reason it doesnt..
}

void ANpcFriendly::OnClickedAway()
{
	if (bIsPartyMember) return;
	
	NewRadiusTween();
	GotoTask.Reset();
	//TargetLocation = GetActorLocation();
	
	bCanMove = true;
	bGotoLocation = false;
	bIsClicked = false;
	bAssumedPosition = false;
}

void ANpcFriendly::OnWallBuilt(AWall* Wall, EOriginSide OriginSide)
{
	if (OriginSide == MainSide && IsCombatant() && !bIsPartyMember && !bIsClicked)
	{
		bAssumedPosition = false;
		GotoTask.Reset();
		DefendWallTask.DoCooldown(10); // skip the cooldown
	}
}

void ANpcFriendly::OnTowerBuilt(AArcherTower* ArcherTower, EOriginSide OriginSide)
{
	OccupyTowerTask.DoCooldown(10); // skip the cooldown
}

void ANpcFriendly::OnNightStarted()
{
	bIsNighttime = true;	
	if (!bIsPartyMember) bAssumedPosition = false;
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
	EFCEase::OutQuad)->SetOnComplete([&]()
	{
		if (!this || !bGotoLocation) return;
		TargetLocation = TargetLocation.GetClampedToSize2D(MovementComp->Radius,MovementComp->Radius);
	})->SetAutoDestroy(true);
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
	
	// skip cooldowns on duties
	OccupyTowerTask.DoCooldown(10);
	DefendWallTask.DoCooldown(10);
	
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


EActionState ANpcFriendly::MoveTimed(float DeltaTime)
{
	Timer += DeltaTime;
	if (Timer >= MoveTime)
	{
		return EActionState::Succeeded;
	}
	
	MoveForwardScaled(MoveDirection * WanderSpeed);
	return EActionState::InProgress;
}

bool ANpcFriendly::MoveTimedCondition() const
{
	return MoveCondition() && !bIsPartyMember && !bIsNighttime && !TargetActor;	
}

void ANpcFriendly::MoveTimedReset()
{
	Timer = 0.0f;
	MoveTime = 0.0f;
}

void ANpcFriendly::SetMoveTime()
{
	MoveTime = FMath::RandRange(WanderTimeMin, WanderTimeMax);
	MoveDirection = FMath::RandRange(-1, 1);
#if WITH_EDITOR
	if (bPrintDebug_MoveTimed) UE_LOG(LogTemp, Warning, TEXT("set move time to %fs  |  set direction to %i"), MoveTime, MoveDirection);
#endif
}

EActionState ANpcFriendly::MoveTo(float DeltaTime)
{
	// Is destination still valid?
	if (TargetActor == nullptr)
	{
#if WITH_EDITOR
		if (bPrintDebug_MoveTo) UE_LOG(LogTemp, Warning, TEXT("mans null"));
#endif
		return EActionState::Failed;
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
		
#if WITH_EDITOR
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
#endif
		
		for (const auto& It : HitResults)
		{
			AActor* HitActor = It.GetActor();
			
			if (HitActor == TargetActor)
			{
				if (It.Distance <= StopDistance)
				{
					return EActionState::Succeeded;
				}
			}
		}
	}
	Timer+=DeltaTime;
	
	// Move
	MoveDirection = GetDirectionTo(TargetLocation);
	MoveForwardScaled(MoveDirection);
	
	return EActionState::InProgress;
}

EActionState ANpcFriendly::MoveToVector(float DeltaTime)
{
	// Are we there yet?
	const float DistanceSquared = FVector::DistSquaredXY(GetActorLocation(), TargetLocation);
	if (DistanceSquared <= StopDistance)
	{
		return EActionState::Succeeded;
	}
	
	// Move
	MoveDirection = GetDirectionTo(TargetLocation);
	MoveForwardScaled(MoveDirection);
	
	return EActionState::InProgress;
}

EActionState ANpcFriendly::MoveToOffset(float DeltaTime)
{
	const APlayerCharacter* PlayerCharacter = NpcManager->GetPlayer();
	
	if (PlayerCharacter == nullptr)
	{
		bIsLerping = false;
		return EActionState::Failed;
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
	
		bIsLerping = false;
		
		// start copying player movement
		bCopyMovement = true;
		
		//TotalDistance = 0;
		bAssumedPosition = true;
		
		return EActionState::Succeeded;
	}
		
	constexpr float Speed = 4;
	const float Alpha = Speed/Distance;
	
	// lerp to position with constant speed
	SetActorLocation(FMath::Lerp(GetActorLocation(),TargetLocation, FMath::Clamp(Alpha, 0,1)));
	
	MoveDirection = GetDirectionTo(TargetLocation);
	bIsLerping = true;
	
	return EActionState::InProgress;
}

bool ANpcFriendly::MoveCondition() const
{
	return bCanMove && !bIsOccupyingTower;
}

bool ANpcFriendly::GotoCondition() const
{
	return MoveCondition() && bGotoLocation;
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
	if (bIsClicked) bCanMove = false;
	else bAssumedPosition = true;
	
	//GotoTask.Reset();
}

bool ANpcFriendly::TargetPlayerCondition() const
{
	return MoveCondition() && bIsPartyMember == true && bEnabled_FollowPlayer == true && !bAssumedPosition;
}

void ANpcFriendly::CopyPlayerMovement(float Direction, float Speed)
{
	if (!bCopyMovement) return;
	
	check(MovementComp);
	MovementComp->MaxSpeed = Speed;
	MoveDirection = Direction;
	
	AddMovementInput(GetActorForwardVector(), Direction);
}

EActionState ANpcFriendly::TargetNearestEnemy(float DeltaTime)
{
	TargetActor = NpcManager->FindNearestNpc(GetActorLocation(), ENpcSearchOption::AnyHostile, MainSide, FMath::Square(TargetingDistance));
	
#if WITH_EDITOR
	if (bPrintDebug_TargetNearestEnemy)
	{
		if (TargetActor) DrawDebugLine(GetWorld(), GetActorLocation(), TargetActor->GetActorLocation(), FColor::Yellow, false, 0.05f);
		UE_LOG(LogTemp, Warning, TEXT("NearestEnemy = %s"), TargetActor? TEXT("Valid"): TEXT("Null"));
	}
#endif
	
	if (TargetActor != nullptr) return EActionState::Succeeded;
	return EActionState::Failed;
}

bool ANpcFriendly::TargetNearestEnemyCondition() const
{
	return IsCombatant();
}

EActionState ANpcFriendly::TargetFarthestTower(float DeltaTime)
{
	TargetActor = BuildingsManager->GetFarthestBuilding(EBuildingType::Tower, MainSide);
	
#if WITH_EDITOR
	if (bPrintDebug_TargetFurthestTower) UE_LOG(LogTemp, Warning, TEXT("Targeting %s"), *TargetActor->GetActorNameOrLabel());
#endif
	
	if (TargetActor != nullptr)
	{
		TargetLocation = TargetActor->GetActorLocation().GetClampedToSize2D(MovementComp->Radius, MovementComp->Radius);
		return EActionState::Succeeded;
	}
	return EActionState::Failed;
}

bool ANpcFriendly::TargetFarthestTowerCondition() const
{
	return BuildingsManager->DoVacantTowersExist(MainSide); // problem
}

EActionState ANpcFriendly::TargetNearestBuilding(float DeltaTime)
{
	TargetActor = BuildingsManager->GetNearestBuilding(GetActorLocation(), EBuildingType::Wall, MainSide, true);
	
	if (!TargetActor) return EActionState::Failed;
	
	const float Radius = MovementComp->Radius;
	const float Distance = 0.2f;
	const FVector RotateAxis = MainSide == EOriginSide::Left? FVector::DownVector : FVector::UpVector;
	TargetLocation = TargetActor->GetActorLocation().RotateAngleAxis(Distance, RotateAxis).GetClampedToSize2D(Radius, Radius);
	
	
	return EActionState::Succeeded;
}

bool ANpcFriendly::TargetBuildingCondition() const
{
	return bEnabled_TargetBuilding && !bIsNighttime && !IsCombatant();
}

EActionState ANpcFriendly::MeleeAttack(float DeltaTime)
{


	if (!TargetActor || !Stats)
	{
		return EActionState::Failed;
	}

	UStatsComponent* TargetStatComponent =
		TargetActor->GetComponentByClass<UStatsComponent>();
OnMeleeAttack();
	if (!TargetStatComponent)
	{
		return EActionState::Failed;
	}
	
	if (CharacterClass == ECharacterType::Builder)
	{
		const ABuilding* Building = Cast<ABuilding>(TargetActor);
		if (Building && Building->IsDamaged())
		{
			TargetStatComponent->HealPatch(20);
			
			return EActionState::Succeeded;
		}
		TargetActor = nullptr;
		return EActionState::Failed;
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
	
	
#if WITH_EDITOR
	if (bPrintDebug_MeleeAttack) UE_LOG(LogTemp, Warning, TEXT("Melee attack"))
#endif
	
	return EActionState::Succeeded;
}

bool ANpcFriendly::MeleeAttackCondition() const
{
	return CharacterClass == ECharacterType::Fighter || CharacterClass == ECharacterType::Builder;
}

void ANpcFriendly::SetMeleeParams()
{
	TargetActor = nullptr;
	TargetingDistance = TargetingDistance_Melee;
}

EActionState ANpcFriendly::RangedAttack(float DeltaTime)
{
	// get target
	if (!TargetActor || !Stats)
	{
		OnBowAttack(false);
		return EActionState::Failed;
	}
	
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

		return EActionState::Failed;
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
Arrow->ShooterActor = this;
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
		
		FVector ToTarget = TargetActor->GetActorLocation() - GetActorLocation();
		ToTarget.Z = 0.f;
		const FVector LocalToTarget = GetActorTransform().InverseTransformVectorNoScale(ToTarget);
		const bool bIsFacingTarget = LocalToTarget.X > 0.f;
		OnBowAttack(bIsFacingTarget);
		
		return EActionState::Succeeded;
	}
	
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
	return EActionState::Failed;
}

bool ANpcFriendly::RangedAttackCondition() const
{
	return CharacterClass == ECharacterType::Archer;
}

void ANpcFriendly::SetRangedParams()
{
	TargetActor = nullptr;
	TargetingDistance = TargetingDistance_Ranged;
}

EActionState ANpcFriendly::OccupyTower(float DeltaTime)
{
	AArcherTower* TargetTower = Cast<AArcherTower>(TargetActor);
	
	if (TargetTower == nullptr || !TargetTower->HasRoom())
	{
		return EActionState::Failed;
	}
	
	if (bCanMove)
	{
		MovementComp->Velocity = FVector::ZeroVector;
		MovementComp->SetComponentTickEnabled(false);
		TargetTower->AddOccupant(this);
		bIsOccupyingTower = true;
		NpcManager->RemoveNpc(this, NpcType, MainSide); // causes freeze if an enemy targets this
	}
	RangedAttackTask.Reset();
	
	return EActionState::Succeeded;
}

bool ANpcFriendly::OccupyTowerCondition() const
{
	return CharacterClass == ECharacterType::Archer && bCanMove && !bIsPartyMember;
}

EActionState ANpcFriendly::GetDefensePosition(float DeltaTime)
{
	const AActor* WallToDefend = BuildingsManager->GetFarthestBuilding(EBuildingType::Wall, MainSide);
	if (WallToDefend == nullptr)
	{
		return EActionState::Failed;
	}
	
	// get the wall's position vector and rotate it a random amount and make it the npc's radius
	
	const FVector RotateAxis = MainSide == EOriginSide::Left? FVector::DownVector : FVector::UpVector;

	const float MaxDistance = 0.5f + ExtraDistancePerPerson * (NpcManager->GetNpcs(ENpcSearchOption::AnyFriendly, MainSide).Num()/2); 
	const float Radius = MovementComp->Radius;
	TargetLocation = WallToDefend->GetActorLocation().RotateAngleAxis(FMath::RandRange(MinDistance, MaxDistance), RotateAxis).GetClampedToSize2D(Radius, Radius);
	
#if WITH_EDITOR
	if (bPrintDebug_DefendWall)
	{
		const FVector Min = WallToDefend->GetActorLocation().RotateAngleAxis(MinDistance, RotateAxis);
		const FVector Max = WallToDefend->GetActorLocation().RotateAngleAxis(MaxDistance, RotateAxis);
		DrawDebugLine(GetWorld(), Min, Min + FVector::UpVector * 100.0f, FColor::Yellow, false, 5.f);
		DrawDebugLine(GetWorld(), Max, Max + FVector::UpVector * 100.0f, FColor::Yellow, false, 5.f);
	}
#endif
	
	bGotoLocation = true;
	bCanMove = true;
	return EActionState::Succeeded;
}	

bool ANpcFriendly::GetDefensePositionCondition() const
{
	//if (bPrintDebug_DefendWall) UE_LOG(LogTemp, Warning, TEXT("defend conditions: %s, %s, %s, %s, %s"), IsCombatant()?TEXT("t"):TEXT("f"),
	//	!bIsPartyMember?TEXT("t"):TEXT("f"), !bAssumedPosition?TEXT("t"):TEXT("f"), (bIsNighttime||bRaid)?TEXT("t"):TEXT("f"), !bIsClicked?TEXT("t"):TEXT("f"));
	return IsCombatant() && !bIsPartyMember && !bAssumedPosition && (bIsNighttime||bRaid) && !bIsClicked;
}

void ANpcFriendly::LogBool(const FString& Name, const bool Value, const bool Simple) const
{
#if WITH_EDITOR
	UE_LOG(LogTemp,Warning,TEXT("%s: %s"), *Name, *BoolToString(MoveCondition(),Simple), *BoolToString(bGotoLocation, Simple))
#endif
}
