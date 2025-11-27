#include "Buildings/BuildingBase.h"

#include "GameModes/DefaultGameMode.h"
#include "Managers/NpcDelegates.h"

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
	
	Hp = 1;

	OnTakeAnyDamage.AddDynamic(this, &ABuildingBase::OnTakeDamage);

	// notify the gamemode that we exist
	GameMode = Cast<ADefaultGameMode>(GetWorld()->GetAuthGameMode());
	if (!GameMode)
	{
		UE_LOG(LogTemp, Error, TEXT("ABuildingBase(27): GameMode is null!"))
		return;
	}
	GameMode->NewBuilding(this, BuildingType);
	
	const float Angle = ADefaultGameMode::GetAngleBetweenVectors(GetActorLocation(), GameMode->WorldOriginNormal);
	if (Angle > 0)
	{
		UE_LOG(LogTemp, Error, TEXT("flip"));
		RootComponent->AddLocalRotation(FRotator(0, 0, 180), false, nullptr, ETeleportType::TeleportPhysics);
	}
}

void ABuildingBase::OnTakeDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType,
                             class AController* InstigatedBy, AActor* DamageCauser)
{
	// on any damage taken..
	Hp = FMath::Clamp(Hp - Damage, 0, MaxHp);
	//UE_LOG(LogTemp, Error, TEXT("hp = %f"), Hp)
	//UE_LOG(LogTemp, Warning, TEXT("On Damage Taken, HP = %f"), Hp);
	if (Damage > 0 && !bNotifiedBuildersOfDamage && FNpcDelegates::OnBuildingDamaged.IsBound())
	{
		bNotifiedBuildersOfDamage = true;
		FNpcDelegates::OnBuildingDamaged.Broadcast(this);
	}
	
	if (Hp == 0)
	{
		if (OnBuildingDestroyed.IsBound()) OnBuildingDestroyed.Broadcast();

		// Local OnDeath functionality
		GameMode->BuildingDestroyed(this, BuildingType);
		OnBuildingDestroyed.Clear();
		OnTakeAnyDamage.RemoveAll(this);
		Destroy();
	}
	if (Hp == MaxHp)
	{
		if (FNpcDelegates::OnBuildingRepaired.IsBound()) FNpcDelegates::OnBuildingRepaired.Broadcast(this);
	}
}
