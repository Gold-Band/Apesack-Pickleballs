// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC/NpcBase.h"
#include "PaperSpriteComponent.h"
#include "Components/BoxComponent.h"
#include "Movement/CircularPawnMovementComponent.h"
#include "HTN/HTNComponent.h"

// Sets default values
ANpcBase::ANpcBase()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	RootComponent = CreateDefaultSubobject<USceneComponent>(FName("RootComponent"));
	SetRootComponent(RootComponent);
	RootComponent->Mobility = EComponentMobility::Movable;
	
	BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Collider"));
	BoxCollider->SetupAttachment(RootComponent);

	SpriteComp = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Sprite"));
	SpriteComp->SetupAttachment(BoxCollider);

	ToolSpriteComp = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Tool"));
	ToolSpriteComp->SetupAttachment(SpriteComp);
	
	HtnDomain = CreateDefaultSubobject<UHTNComponent>(TEXT("HTN"));
	MovementComp = CreateDefaultSubobject<UCircularPawnMovementComponent>(TEXT("Movement"));
	

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

const FToolInfo* ANpcBase::GetCharacterToolInfo() const
{
	return GetToolInfo(CharacterTool);
}

const FToolInfo* ANpcBase::GetToolInfo(const FDataTableRowHandle& ToolHandle)
{
	if (ToolHandle.IsNull()) return nullptr;
	return ToolHandle.GetRow<FToolInfo>(TEXT("Tool Getter"));
}

void ANpcBase::ForceTask(UPrimitiveTask* PrimitiveTask)
{
	HtnDomain->CancelActivePlan();
	HtnDomain->RunPrimitiveTask(PrimitiveTask);
}

void ANpcBase::ForceTask(const TSoftObjectPtr<UTask> Task) const
{
	HtnDomain->CancelActivePlan();
	HtnDomain->RunTask(Task);
}

void ANpcBase::Flip()
{
	AddActorLocalRotation(FRotator(0.f, 180.f, 0.f), false, nullptr, ETeleportType::ResetPhysics);

	// other stuff
}


float ANpcBase::GetCharacterPreferredRadius() const
{
	return Radius;
}

void ANpcBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	OnTakeAnyDamage.AddDynamic(this, &ANpcBase::OnTakeDamage);
	
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

void ANpcBase::BeginPlay()
{
	Super::BeginPlay();
	
	Hp = MaxHp;
	Radius = GetActorLocation().Size2D();
	
}

void ANpcBase::OnTakeDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType,
                            class AController* InstigatedBy, AActor* DamageCauser)
{
	// on any damage taken..
	Hp = FMath::Max(Hp - Damage, 0);

	//UE_LOG(LogTemp, Warning, TEXT("On Damage Taken, HP = %f"), Hp);
	
	if (Hp == 0)
	{
		if (OnDeath.IsBound()) OnDeath.Broadcast();

		// Local OnDeath functionality
		Destroy();
	}
}
