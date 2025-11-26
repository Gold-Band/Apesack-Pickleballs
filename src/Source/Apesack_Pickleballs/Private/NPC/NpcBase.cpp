// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC/NpcBase.h"
#include "PaperSpriteComponent.h"
#include "Components/BoxComponent.h"
#include "Movement/CircularPawnMovementComponent.h"

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

	MovementComp = CreateDefaultSubobject<UCircularPawnMovementComponent>(TEXT("Movement"));
	MovementComp->MaxSpeed = 200;
}

void ANpcBase::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	TagContainer = CharacterTag.GetSingleTagContainer();
}

void ANpcBase::Flip()
{
	AddActorLocalRotation(FRotator(0.f, 180.f, 0.f), false, nullptr, ETeleportType::ResetPhysics);
	// other stuff
}

FVector ANpcBase::GetForwardVector() const
{
	return bFwd? GetActorForwardVector() : -1 * GetActorForwardVector();
}


float ANpcBase::GetCharacterPreferredRadius() const
{
	return Radius;
}

float ANpcBase::GetDirectionToTown()
{
	return OriginDirection;
	//OriginDirection = ADefaultGameMode
}

void ANpcBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	OnTakeAnyDamage.AddDynamic(this, &ANpcBase::OnTakeDamage);


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
	//Hp = FMath::Max(Hp - Damage, 0);

	//UE_LOG(LogTemp, Warning, TEXT("On Damage Taken, HP = %f"), Hp);

	//if (Hp == 0)
	//{
	//	if (OnDeath.IsBound())
		//	OnDeath.Broadcast();

		// Local OnDeath functionality
	//	Destroy();
//	}
	//else
	//{
	//	return;   // <-- simply returns from the function
//	}
}
void ANpcBase::ApplyDamageListener(AActor* DamagedActor, float Damage, const class UDamageType* DamageType,
	class AController* InstigatedBy, AActor* DamageCauser)
{

}

