#pragma once

#include "CoreMinimal.h"
#include "GameplayTagAssetInterface.h"
#include "GameFramework/Actor.h"
#include "BuildingBase.generated.h"

class UPaperSpriteComponent;
class UBoxComponent;

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
	
protected:

	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess=true))
	FGameplayTag BuildingTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxHp = 10.f;
	
private:
	float Hp = MaxHp;

	UFUNCTION()
	void OnTakeDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

};
