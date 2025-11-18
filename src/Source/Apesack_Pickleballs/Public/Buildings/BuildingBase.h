#pragma once

#include "CoreMinimal.h"
#include "GameplayTagAssetInterface.h"
#include "GameFramework/Actor.h"
#include "BuildingBase.generated.h"

class ADefaultGameMode;

UENUM(BlueprintType)
enum EBuildingType : uint8
{
	Null,
	Wall,
	ArcherTower,
	Shop
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBuildingDestroyed);

UCLASS()
class APESACK_PICKLEBALLS_API ABuildingBase : public AActor, public IGameplayTagAssetInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABuildingBase();

	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;

	FOnBuildingDestroyed OnBuildingDestroyed;

	float DistanceFromOrigin;
	
	//float GetRepairPriority() const {return Priority;}
	bool IsDestroyed() const {return Hp == 0;}
protected:

	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Building")
	TEnumAsByte<EBuildingType> BuildingType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess=true), Category="Building")
	FGameplayTag BuildingTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Building")
	float MaxHp = 10.f;
	
	//float Priority;
	
private:
	float Hp;
	TObjectPtr<ADefaultGameMode> GameMode;
	
	bool bNotifiedBuildersOfDamage;
	
	UFUNCTION()
	void OnTakeDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

};
