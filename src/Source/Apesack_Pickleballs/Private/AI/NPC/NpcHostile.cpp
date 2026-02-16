// Fill out your copyright notice in the Description page of Project Settings.
#include "AI/NPC/NpcHostile.h"
#include "StatsComponent.h"
#include "AI/HTN/HTNComponent.h"
#include "AI/HTN/ListItemObject.h"
#include "GameModes/DefaultGameMode.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Managers/NpcManager.h"

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
	const float DistanceFromOrigin = ADefaultGameMode::GetDistanceToOrigin(GetActorLocation());
	MainSide = DistanceFromOrigin < 0? EOriginSide::Left : EOriginSide::Right;
	
	Super::BeginPlay();
	
	UNpcManager::OnMostVulnerableAssetChangedDelegate.AddUObject(this, &ThisClass::OnNearestAttackableChanged);
	
	IgnoreActors = NpcManager->GetNpcs(ENpcSearchOption::AnyHostile, EOriginSide::Any);	
	IgnoreActors.Add(this);
}

void ANpcHostile::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UNpcManager::OnMostVulnerableAssetChangedDelegate.RemoveAll(this);
	Super::EndPlay(EndPlayReason);
}

bool ANpcHostile::GetSideCheckCondition()
{
	return true;
}

void ANpcHostile::BindActions()
{
	WalkAction.ExecutionDelegate.BindUObject(this, &ThisClass::Walk);
	
	MoveToAction.ConditionDelegate.BindUObject(this, &ThisClass::MoveToCondition);
	MoveToAction.ExecutionDelegate.BindUObject(this, &ThisClass::MoveTo);
	MoveToAction.ResetDelegate.BindUObject(this, &ThisClass::MoveToReset);
	
	MeleeAttackAction.ConditionDelegate.BindUObject(this, &ThisClass::MeleeAttackCondition);
	MeleeAttackAction.ExecutionDelegate.BindUObject(this, &ThisClass::MeleeAttack);
	
	CooldownAction.ExecutionDelegate.BindUObject(this, &ThisClass::Cooldown);
	CooldownAction.ResetDelegate.BindUObject(this, &ThisClass::CooldownReset);
	
	TargetAttackableAction.ConditionDelegate.BindUObject(this, &ThisClass::TargetAttackableCondition);
	TargetAttackableAction.ExecutionDelegate.BindUObject(this, &ThisClass::TargetAttackable);
}

void ANpcHostile::CreateBehaviours()
{
	// Melee Attack
	MeleeAttackTask.Actions.Add(&TargetAttackableAction);
	MeleeAttackTask.Actions.Add(&MoveToAction);
	MeleeAttackTask.Actions.Add(&MeleeAttackAction);
	MeleeAttackTask.Actions.Add(&CooldownAction);
	MeleeAttackTask.bPrintDebug = bPrintDebug_MeleeAttack;
	HtnDomain->AssignTask(&MeleeAttackTask);
	 
	// Walk
	MoveForwardTask.Actions.Add(&WalkAction);
	MoveForwardTask.bPrintDebug = bPrintDebug_MoveTo;
	HtnDomain->AssignTask(&MoveForwardTask);
}

void ANpcHostile::OnNearestAttackableChanged(AActor* NewTarget, EOriginSide Side)
{
	if (Side != MainSide) return;
	
	MeleeAttackTask.Reset();
}

float ANpcHostile::GetAngleBetweenVectors(const FVector& A, const FVector& B)
{
	if (A.IsNearlyZero() || B.IsNearlyZero()) return 0.f;
	
	const float Dot = FVector::DotProduct(A, B);
	const float CrossDot = FVector::CrossProduct(A, B).Dot(FVector::UpVector);
	return FMath::RadiansToDegrees(FMath::Atan2(CrossDot, Dot));
}

void ANpcHostile::Walk(float DeltaTime)
{
	if (MainSide == EOriginSide::Left) MoveForwardScaled(1);
	else MoveForwardScaled(-1);
}


void ANpcHostile::Cooldown(float DeltaTime)
{
	Timer += DeltaTime;
	if (Timer >= Delay) CooldownAction.State = EActionState::Succeeded;
}


void ANpcHostile::CooldownReset()
{
	Timer = 0.0f;
}

void ANpcHostile::MoveTo(float DeltaTime)
{
	// Is destination still valid?
	if (bIsFirstTick)
	{
		bIsFirstTick = false;
		return;
	}
	
	if (!TargetActor)
	{
		MoveToAction.State = EActionState::Failed;
		return;
	}
	
	// Are we there yet?
	const float DistanceSquared = FVector::DistSquaredXY(GetActorLocation(), TargetActor->GetActorLocation()); // this crashes sometimes on first tick
	if (DistanceSquared <= StartRaycastingDistanceSquared && MoveToTimer >= RaycastInterval)
	{
		MoveToTimer = 0;
		UKismetSystemLibrary::LineTraceMulti(
			GetWorld(), // world
			GetActorLocation(), // start 
			TargetActor->GetActorLocation(), // end 
			UEngineTypes::ConvertToTraceType(ECC_Visibility), // channel
			false,
			IgnoreActors, // ignore 
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
				//if (bPrintDebug_MoveTo) UE_LOG(LogTemp, Warning, TEXT("Distance = %f"), It.Distance);
				if (It.Distance <= StopDistance)
				{
					MoveToAction.State = EActionState::Succeeded;
					return;
				}
			}
		}
	}
	MoveToTimer+=DeltaTime;
	
	const float Direction = FVector::DotProduct(TargetActor->GetActorLocation() - GetActorLocation(), GetActorForwardVector()) > 0 ? 1.0f : -1.0f;
	MoveForwardScaled(Direction);
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

void ANpcHostile::MeleeAttack(float DeltaTime)
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
	
		OnMeleeAttack();
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
	    DamagePatch.DebuffDuration
	);
	MeleeAttackAction.State = EActionState::Succeeded;
	Delay = Cooldown_MeleeAttack;
}

bool ANpcHostile::MeleeAttackCondition() const
{
	return NpcManager->FindNearestNpc(GetActorLocation(), ENpcSearchOption::AnyFriendly, MainSide) != nullptr;
}

void ANpcHostile::TargetAttackable(float DeltaTime)
{
	float Angle = 0;
	if (MainSide == EOriginSide::Left)
	{
		//Angle = GetAngleBetweenVectors(GetActorLocation(), UNpcManager::LeftMostVulnerableAsset->GetActorLocation());
		//if (Angle < 0) TargetActor = UNpcManager::LeftMostVulnerableAsset;
		TargetActor = UNpcManager::LeftMostVulnerableAsset;
	}
	else
	{
		//Angle = GetAngleBetweenVectors(GetActorLocation(), UNpcManager::LeftMostVulnerableAsset->GetActorLocation());
		//if (Angle > 0) TargetActor = UNpcManager::RightMostVulnerableAsset;
		TargetActor = UNpcManager::RightMostVulnerableAsset;
	}
	
	if (bPrintDebug_TargetNearestAny) UE_LOG(LogTemp, Warning, TEXT("%s::NearestAttackable=%s (Angle=%f)"), *GetActorNameOrLabel(),TargetActor? *TargetActor->GetActorNameOrLabel(): TEXT("Null"), Angle);
	
	if (TargetActor != nullptr) TargetAttackableAction.State = EActionState::Succeeded;
	else TargetAttackableAction.State = EActionState::Failed;
}

bool ANpcHostile::TargetAttackableCondition() const
{
	if (MainSide == EOriginSide::Left) return UNpcManager::LeftMostVulnerableAsset != nullptr;
	return UNpcManager::RightMostVulnerableAsset != nullptr;
}
