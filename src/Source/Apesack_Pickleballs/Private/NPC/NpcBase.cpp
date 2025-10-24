// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC/NpcBase.h"
#include "PaperSpriteComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "HTN/HTNComponent.h"

// Sets default values
ANpcBase::ANpcBase()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Collider"));
	BoxCollider->SetupAttachment(RootComponent);

	SpriteComp = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Sprite"));
	SpriteComp->SetupAttachment(BoxCollider);
	HtnDomain = CreateDefaultSubobject<UHTNComponent>(TEXT("HTN"));
	MovementComp = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("Movement"));
}

void ANpcBase::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	TagContainer = CharacterTag.GetSingleTagContainer();
}

const FClassInfo* ANpcBase::GetClassInfo() const
{
	if (CharacterClass.IsNull()) return nullptr;
	return CharacterClass.GetRow<FClassInfo>(TEXT("Class Getter"));
}

const FToolInfo* ANpcBase::GetTool() const
{
	if (CharacterTool.IsNull()) return nullptr;
	return CharacterTool.GetRow<FToolInfo>(TEXT("Tool Getter"));
}

void ANpcBase::ForceTask(const TSoftObjectPtr<UTask> Task) const
{
	HtnDomain->CancelActivePlan();
	HtnDomain->RunTask(Task);
}

void ANpcBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	const FClassInfo* MyClass = CharacterClass.GetRow<FClassInfo>(TEXT("Getting Class Tasks"));
	HtnDomain->SetTasks(MyClass->ClassTasks);
}
