// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC/NpcFriendly.h"
#include "Components/WidgetComponent.h"
#include "PaperSpriteComponent.h"
#include "GameModes/DefaultGameMode.h"
#include "HTN/HTNComponent.h"
#include "Managers/NpcDelegates.h"
#include "Movement/CircularPawnMovementComponent.h"

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
	
	//MovementComp = CreateDefaultSubobject<UCircularPawnMovementComponent>(TEXT("Movement"));
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
	
	UE_LOG(LogTemp, Log, TEXT("OnExternalWallChanged"));
	
	// force task goto wall
	ForceTask(GotoWallTask);
}

AWall* ANpcFriendly::GetGuardingWall() const
{
	return GuardingWall;
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
	
	FNpcDelegates::OnFurthestLeftWallChanged.AddUObject(this, &ANpcFriendly::OnExternalWallChanged);
	
}

void ANpcFriendly::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	FNpcDelegates::OnFurthestLeftWallChanged.RemoveAll(this);
	Super::EndPlay(EndPlayReason);
}

void ANpcFriendly::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	// Set tasks based on class
	const FClassInfo* MyClass = CharacterClass.GetRow<FClassInfo>(TEXT("Getting Class Tasks"));
	if (!MyClass)
	{
		UE_LOG(LogTemp, Error, TEXT("NpcBase.cpp(58): NPC has no class!"))
		return;
	}
	
	HtnDomain->SetTasks(MyClass->ClassTasks);

	// Set tool based on class
	if (!CharacterTool.IsNull())
	{
		ToolSpriteComp->SetSprite(GetCharacterToolInfo()->ToolSprite);
	}
	else if (!MyClass->BaseTool.IsNull())
	{
		// set tool to base tool
		ToolSpriteComp->SetSprite(GetToolInfo(MyClass->BaseTool)->ToolSprite);
	}
}
