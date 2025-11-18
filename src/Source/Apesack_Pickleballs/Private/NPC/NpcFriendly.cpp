// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC/NpcFriendly.h"
#include "Components/WidgetComponent.h"
#include "PaperSpriteComponent.h"
#include "Buildings/Zone.h"
#include "GameModes/DefaultGameMode.h"
#include "HTN/HTNComponent.h"
#include "Managers/NpcDelegates.h"
#include "WorldClock/WorldClockSubsystem.h"

// Sets default values
ANpcFriendly::ANpcFriendly()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	NameTag = CreateDefaultSubobject<UWidgetComponent>("Name");
	NameTag->SetupAttachment(RootComponent);
	
	
	ToolSpriteComp = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Tool"));
	ToolSpriteComp->SetupAttachment(SpriteComp);
	
	HtnDomain = CreateDefaultSubobject<UHTNComponent>(TEXT("HTN"));
}


const FClassInfo* ANpcFriendly::GetClassInfo() const
{
	if (CharacterClass.IsNull()) return nullptr;
	return CharacterClass.GetRow<FClassInfo>(TEXT("Class Getter"));
}

const FToolInfo* ANpcFriendly::GetCharacterToolInfo() const
{
	return GetToolInfo(CharacterTool);
}

const FToolInfo* ANpcFriendly::GetToolInfo(const FDataTableRowHandle& ToolHandle)
{
	if (ToolHandle.IsNull()) return nullptr;
	return ToolHandle.GetRow<FToolInfo>(TEXT("Tool Getter"));
}

void ANpcFriendly::ForceTask(const TSoftObjectPtr<UTask> Task) const
{
	HtnDomain->CancelActivePlan();
	HtnDomain->RunTask(Task);
}

void ANpcFriendly::OnExternalWallChanged(AWall* Wall)
{
	// go defend it
	GuardingWall = Wall;
	
	//UE_LOG(LogTemp, Log, TEXT("OnExternalWallChanged"));
	
	// force task goto wall
	GotoWallIfFighter();
	
}

AWall* ANpcFriendly::GetGuardingWall() const
{
	return GuardingWall;
}

bool ANpcFriendly::IsInProperZone() const
{
	if (Zone == Zone_Archer && MyClassType == Class_Ranger) return true;
	if (Zone == Zone_Melee && MyClassType == Class_Melee) return true;
	return false;
}

TEnumAsByte<EClassType> ANpcFriendly::GetClassType() const
{
	return MyClassType;
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
	
	UWorldClockSubsystem::Get(this)->OnNightStartedDelegate.AddDynamic(this, &ANpcFriendly::OnNightStarted);
	UWorldClockSubsystem::Get(this)->OnNightEndedDelegate.AddDynamic(this, &ANpcFriendly::OnNightEnded);
	
}

void ANpcFriendly::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (DefendSide == Right) FNpcDelegates::OnFurthestRightWallChanged.RemoveAll(this);
	else if (DefendSide == Left) FNpcDelegates::OnFurthestLeftWallChanged.RemoveAll(this);
	
	Super::EndPlay(EndPlayReason);
}

void ANpcFriendly::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	// Set tasks based on class
	ApplyClass();
}

void ANpcFriendly::ApplyClass()
{
	const FClassInfo* MyClass = CharacterClass.GetRow<FClassInfo>(TEXT("Getting Class Tasks"));
	if (!MyClass)
	{
		UE_LOG(LogTemp, Error, TEXT("NpcBase.cpp(58): NPC has no class!"))
		return;
	}
	
	HtnDomain->SetTasks(MyClass->ClassTasks);

	bool bHasTool = false;
	
	// Set tool based on class
	if (!CharacterTool.IsNull())
	{
		ToolSpriteComp->SetSprite(GetCharacterToolInfo()->ToolSprite);
		bHasTool = true;
	}
	else if (!MyClass->BaseTool.IsNull())
	{
		// set tool to base tool
		ToolSpriteComp->SetSprite(GetToolInfo(MyClass->BaseTool)->ToolSprite);
		CharacterTool = MyClass->BaseTool;
		bHasTool = true;
	}
	
	if (bHasTool)
	{
		// Assign guarding role
		const FName ToolTag = GetCharacterToolInfo()->ToolTag.GetTagName();
		if (ToolTag.IsEqual(FName("Tool.Ranged")))
		{
			SetFighter();
			MyClassType = Class_Ranger;
		}
		else if (ToolTag.IsEqual(FName("Tool.Melee")))
		{
			SetFighter();
			MyClassType = Class_Melee;
		}
		else
		{
			// is builder
			MyClassType = Class_Builder;
		}
	}
	else
	{
		// set non fighter
		MyClassType = Class_Peasant;
	}
}

void ANpcFriendly::SetFighter()
{
	if (DefendSide == None) DefendSide = static_cast<TEnumAsByte<EDefendSide>>(FMath::RandRange(0, MAX-1));
	
	if (DefendSide == Right) FNpcDelegates::OnFurthestRightWallChanged.AddUObject(this, &ANpcFriendly::OnExternalWallChanged);
	else if (DefendSide == Left) FNpcDelegates::OnFurthestLeftWallChanged.AddUObject(this, &ANpcFriendly::OnExternalWallChanged);
}

void ANpcFriendly::GotoWallIfFighter()
{
	switch (MyClassType)
	{
	case Class_Peasant:
		break;
	case Class_Builder:
		break;
	case Class_Melee:
		ForceTask(GotoWallTaskMelee);
		break;
	case Class_Ranger:
		ForceTask(GotoWallTaskArcher);
		break;
	default: ;
	}
}

void ANpcFriendly::OnNightStarted()
{
	HtnDomain->UpdateWorldState(FString("IsDaytime"), false);
	if (GuardingWall) GotoWallIfFighter();
}

void ANpcFriendly::OnNightEnded()
{
	HtnDomain->UpdateWorldState(FString("IsDaytime"), true);
}
