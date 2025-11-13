#include "Buildings/BuildingBase.h"

#include "GameModes/DefaultGameMode.h"

ABuildingBase::ABuildingBase()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(FName("RootComponent"));
	SetRootComponent(RootComponent);
	RootComponent->Mobility = EComponentMobility::Static;
}

void ABuildingBase::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	TagContainer = BuildingTag.GetSingleTagContainer();
}

void ABuildingBase::BeginPlay()
{
	Super::BeginPlay();

	OnTakeAnyDamage.AddDynamic(this, &ABuildingBase::OnTakeDamage);

	// notify the gamemode that we exist
	ADefaultGameMode* GameMode = Cast<ADefaultGameMode>(GetWorld()->GetAuthGameMode());
	if (!GameMode)
	{
		UE_LOG(LogTemp, Error, TEXT("ABuildingBase(27): GameMode is null!"))
		return;
	}

	GameMode->NewBuilding(this);
}

void ABuildingBase::OnTakeDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType,
                             class AController* InstigatedBy, AActor* DamageCauser)
{
	// on any damage taken..
	Hp = FMath::Clamp(Hp - Damage, 0, MaxHp);

	//UE_LOG(LogTemp, Warning, TEXT("On Damage Taken, HP = %f"), Hp);
	
	if (Hp == 0)
	{
		if (OnBuildingDestroyed.IsBound()) OnBuildingDestroyed.Broadcast();

		// Local OnDeath functionality
		Destroy();
	}
}
