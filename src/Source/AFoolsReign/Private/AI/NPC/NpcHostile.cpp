// Fill out your copyright notice in the Description page of Project Settings.
#include "AI/NPC/NpcHostile.h"

#include "MathUtil.h"
#include "StatsComponent.h"
#include "AI/HTN/HTNComponent.h"
#include "AI/HTN/ListItemObject.h"
#include "AI/NPC/NpcFriendly.h"
#include "GameModes/DefaultGameMode.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Managers/NpcManager.h"
#include "Movement/CircularPawnMovementComponent.h"

// Sets default values
ANpcHostile::ANpcHostile()
{
	NpcType = ENpcTag::Hostile;
	CharacterName = "Hostile";
	
	GetSideInterval = 1;
}

TArray<UListItemObject*> ANpcHostile::GetInfo() const
{
	TArray<UListItemObject*> Info{};
	
	// hp
	UListItemObject* HpInfo = NewObject<UListItemObject>();
	HpInfo->DisplayText = FText::FromString(FString::Printf(TEXT("Hp: %i/%i"), FMath::RoundToInt(Stats->GetHealth()), FMath::RoundToInt(Stats->GetMaxHealth())));
	
	Info.Add(HpInfo);
	
	return Info;
}

void ANpcHostile::BeginPlay()
{
	const float DistanceFromOrigin = ADefaultGameMode::GetAngleToOrigin(GetActorLocation());
	MainSide = DistanceFromOrigin < 0? EOriginSide::Left : EOriginSide::Right;
	
	Super::BeginPlay();
	
	UNpcManager::OnMostVulnerableAssetChangedDelegate.AddUObject(this, &ThisClass::OnNearestAttackableChanged);
}

void ANpcHostile::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UNpcManager::OnMostVulnerableAssetChangedDelegate.RemoveAll(this);
	Super::EndPlay(EndPlayReason);
}

void ANpcHostile::CreateBehaviours()
{
	FAction WalkAction{FString("Walk")};
	WalkAction.Func = [&](const float DeltaTime){return Walk(DeltaTime);};
	
	FAction MoveToAction{FString("Move To")};
	MoveToAction.Func = [&](const float DeltaTime){return MoveTo(DeltaTime);};
	
	FAction TargetAttackableAction{FString("Target Attackable")};
	TargetAttackableAction.Func = [&](const float DeltaTime){return TargetAttackable(DeltaTime);};
	
	FAction MeleeAnimateAction{FString("Attack Part 1")};
	MeleeAnimateAction.Func = [&](const float DeltaTime){ return MeleeAnimation(DeltaTime);};
	
	FAction MeleeHitAction{FString("Attack Part 2")};
	MeleeHitAction.Func = [&](const float DeltaTime){ return CheckHit(DeltaTime);};
	
	FAction DelayAction{FString("Delay")};
	DelayAction.Func = [&] (const float DeltaTime) { return Delay(DeltaTime);};
	
	// Melee Attack
	MeleeAttackTask.Actions.Add(TargetAttackableAction);
	MeleeAttackTask.Actions.Add(MoveToAction);
	MeleeAttackTask.Actions.Add(MeleeAnimateAction);
	MeleeAttackTask.Actions.Add(DelayAction);
	MeleeAttackTask.Actions.Add(MeleeHitAction);
	MeleeAttackTask.bPrintDebug = bPrintDebug_MeleeAttack;
	MeleeAttackTask.Condition = [&]{ const bool cond = MeleeAttackCondition(); bCanMove = !cond; return cond;};
	MeleeAttackTask.OnStarted = [&] {Timer = 0; DelayTime = DamageDelay;};
	MeleeAttackTask.OnEnded = [&] { if (MeleeAttackTask.Failed()) bCanMove = true; MoveToTimer = RaycastInterval;};
	MeleeAttackTask.Cooldown = Cooldown_MeleeAttack;
	
	// Walk
	MoveForwardTask.Actions.Add(WalkAction);
	MoveForwardTask.Condition = [&] {return MoveToCondition();};
	MoveForwardTask.bPrintDebug = bPrintDebug_MoveTo;
	
	HtnDomain->AssignTask(&MeleeAttackTask);
	HtnDomain->AssignTask(&MoveForwardTask);
	
	// Wait
	Super::CreateBehaviours();
}

void ANpcHostile::OnNearestAttackableChanged(AActor* NewTarget, EOriginSide Side)
{
	if (Side != MainSide) return;
	
	MeleeAttackTask.Reset();
}

EActionState ANpcHostile::Walk(float DeltaTime)
{
	if (MainSide == EOriginSide::Left) MoveForwardScaled(1);
	else MoveForwardScaled(-1);
	
	return EActionState::Succeeded;
}

EActionState ANpcHostile::MoveTo(float DeltaTime)
{
	// Is destination still valid?
	if (bIsFirstTick)
	{
		bIsFirstTick = false;
		return EActionState::InProgress;
	}
	
	if (!TargetActor)
	{
		return EActionState::Failed;
	}
	
	// Are we there yet?
	const float DistanceSquared = FVector::DistSquaredXY(GetActorLocation(), TargetActor->GetActorLocation().GetClampedToSize2D(MovementComp->Radius,MovementComp->Radius)); // this crashes sometimes on first tick
	const bool bIsWall = NpcManager->IsMostVulnerableAWall();
	if (DistanceSquared <= StartRaycastingDistanceSquared && MoveToTimer >= RaycastInterval && bIsWall)
	{
		MoveToTimer = 0;
		
		UKismetSystemLibrary::LineTraceMulti(
			GetWorld(), // world
			GetActorLocation(), // start 
			TargetActor->GetActorLocation(), // end 
			UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel1), // channel
			false,
			TArray<AActor*>{}, // ignore 
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
	else if (!bIsWall)
	{
		if (DistanceSquared <= StartRaycastingDistanceSquared*10)
		{
			const float TargetRadius = TargetActor->GetActorLocation().Size2D();
			const float Dist = FMath::Abs( TargetRadius - MovementComp->Radius);
			constexpr float Speed = 3;
			const float Alpha = Speed/Dist;
			MovementComp->Radius = FMath::Lerp(MovementComp->Radius, TargetRadius, FMath::Clamp(Alpha, 0,1)); 
		}
		if (DistanceSquared <= FMath::Square(StopDistance))
		{
			return EActionState::Succeeded;
		}
	}
	
	MoveToTimer+=DeltaTime;
	
	MoveDirection = GetDirectionTo(TargetActor->GetActorLocation());
	MoveForwardScaled(MoveDirection);
	
	return EActionState::InProgress;
}

bool ANpcHostile::MoveToCondition() const
{
	return bCanMove;
}

void ANpcHostile::MoveToReset()
{
	MoveToTimer = RaycastInterval;
	//TargetActor = nullptr;
}

EActionState ANpcHostile::MeleeAnimation(float DeltaTime)
{
	bCanMove = false;
	
	// get target
	if (TargetActor == nullptr)
	{
		return EActionState::Failed;
	}
	OnMeleeAttack();
	return EActionState::Succeeded;
}

EActionState ANpcHostile::CheckHit(float DeltaTime)
{
	// get target's stat component
	UStatsComponent* TargetStatComponent = TargetActor->GetComponentByClass<UStatsComponent>();
	if (TargetStatComponent == nullptr)
	{
		return EActionState::Failed;
	}
	
	FDamagePatch DamagePatch = Stats->GetDamagePatch();

	// 2. OVERRIDE specific fields
	DamagePatch.NormalDamage = 5.f;
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
		DamagePatch.DebuffDuration,
		DamagePatch.BleedDamage
	);
	
	return EActionState::Succeeded;
}

bool ANpcHostile::MeleeAttackCondition() const
{
	if (MainSide == EOriginSide::Left) return UNpcManager::LeftMostVulnerableAsset != nullptr;
	return UNpcManager::RightMostVulnerableAsset != nullptr;
}

EActionState ANpcHostile::TargetAttackable(float DeltaTime)
{
	TargetActor = NpcManager->GetAttackable(EOriginSide::Any, GetActorLocation());
	
#if WITH_EDITOR
	if (bPrintDebug_TargetNearestAny)
	{
		const float Angle = ADefaultGameMode::GetAngleBetweenVectors(GetActorLocation(), TargetActor->GetActorLocation());
		UE_LOG(LogTemp, Warning, TEXT("%s::NearestAttackable=%s (Angle=%f)"), *GetActorNameOrLabel(),TargetActor? *TargetActor->GetActorNameOrLabel(): TEXT("Null"), Angle);
	}
#endif

	if (TargetActor != nullptr) return EActionState::Succeeded;
	return EActionState::Failed;
}

EActionState ANpcHostile::Delay(float DeltaTime)
{
	if ((Timer+=DeltaTime) >= DelayTime)
	{
		Timer = 0;
		return EActionState::Succeeded;
	}
	return EActionState::InProgress;
}
