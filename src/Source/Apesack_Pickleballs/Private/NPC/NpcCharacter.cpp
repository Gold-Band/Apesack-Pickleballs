// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC/NpcCharacter.h"
#include "PaperSpriteComponent.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "HTN/HTNComponent.h"
#include "NPC/NpcName.h"
#include "UI/OptionsWidget.h"

// Sets default values
ANpcCharacter::ANpcCharacter()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(SceneComponent);
	
	BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Collider"));
	BoxCollider->SetupAttachment(SceneComponent);

	NameTag = CreateDefaultSubobject<UWidgetComponent>("Name");
	NameTag->SetupAttachment(SceneComponent);
	
	InteractionOptions = CreateDefaultSubobject<UWidgetComponent>("Options");
	InteractionOptions->SetupAttachment(SceneComponent);
	
	SpriteComp = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Sprite"));
	SpriteComp->SetupAttachment(BoxCollider);
	HtnDomain = CreateDefaultSubobject<UHTNComponent>(TEXT("HTN"));
	MovementComp = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("Movement"));
	
	if (CharacterName.IsEmpty())
	{
		// get random name
		static ConstructorHelpers::FObjectFinder<UDataTable> NamesDataTableFinder(TEXT("/Game/NPC/NpcNames.NpcNames"));
		if (NamesDataTableFinder.Succeeded())
		{
			UDataTable* NamesDataTable = NamesDataTableFinder.Object;
			TArray<FNpcName*> AllNames;
			NamesDataTable->GetAllRows(TEXT("GetRandomName"), AllNames);
			CharacterName = *AllNames[FMath::RandRange(0, AllNames.Num() - 1)]->SampleName;
		}
	}
	
}

bool ANpcCharacter::AreOptionsVisible() const
{
	return InteractionOptions->IsVisible();	
}

void ANpcCharacter::OpenInteractionDialogue()
{
	InteractionOptions->SetVisibility(true);
}

void ANpcCharacter::CloseInteractionDialogue()
{
	InteractionOptions->SetVisibility(false);
}

UOptionsWidget* ANpcCharacter::GetInteractionDialogue() const
{
	return Cast<UOptionsWidget>(InteractionOptions->GetUserWidgetObject());
}

// Called when the game starts or when spawned
void ANpcCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}
