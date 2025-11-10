// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorPool.h"
#include "GameFramework/GameModeBase.h"
#include "DefaultGameMode.generated.h"

class AProjectileBase;
class ABuildingBase;
class UNpcDelegates;
class UWorldClockSubsystem;
enum EShopType : uint8;
class ANpcShop;
struct FBuildingInfo;
struct FClassInfo;
class UTask;
/**
 * 
 */
UCLASS()
class APESACK_PICKLEBALLS_API ADefaultGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	
	bool BuildersUnlocked() const { return bBuildersUnlocked; }
	bool ArchersUnlocked() const { return bArchersUnlocked; }
	bool SoldiersUnlocked() const { return bSoldiersUnlocked; }

	const TSoftObjectPtr<UTask>& GetPromoteMeleeTask() const { return PromoteMeleeTask; }
	const TSoftObjectPtr<UTask>& GetPromoteRangedTask() const { return PromoteRangedTask; }
	const TSoftObjectPtr<UTask>& GetPromoteBuilderTask() const { return PromoteBuilderTask; }

	bool GetBuilderShopLocation(FVector& OutLocation) const;
	bool GetArcherShopLocation(FVector& OutLocation) const;
	bool GetBlacksmithShopLocation(FVector& OutLocation) const;
	
	FString GetRandomNpcName() const;
	TArray<FClassInfo*> GetAllClasses() const;
	TArray<FBuildingInfo*> GetAllBuildings() const;

	void RegisterShop(const ANpcShop* Shop, const EShopType ShopType);
	void NewBuilding(ABuildingBase* Building);

	UFUNCTION(BlueprintPure)
	AActor* GetArrow();
	
private:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess=true))
	bool bBuildersUnlocked;
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess=true))
	TSoftObjectPtr<UTask> PromoteBuilderTask;
	TWeakObjectPtr<const ANpcShop> BuilderShop; 
	
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess=true))
	bool bArchersUnlocked;
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess=true))
	TSoftObjectPtr<UTask> PromoteRangedTask;
	TWeakObjectPtr<const ANpcShop> ArcherShop; 

	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess=true))
	bool bSoldiersUnlocked;
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess=true))
	TSoftObjectPtr<UTask> PromoteMeleeTask;
	TWeakObjectPtr<const ANpcShop> BlacksmithShop; 
	
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess=true))
	TSoftObjectPtr<UDataTable> NpcClasses;
	
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess=true))
	TSoftObjectPtr<UDataTable> NpcTools;
	
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess=true))
	TSoftObjectPtr<UDataTable> Buildings;

	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess=true))
	TSoftObjectPtr<UDataTable> NpcNames;


	// cached manager instances
	
	UPROPERTY()
	TObjectPtr<UWorldClockSubsystem> WorldClock;

	UPROPERTY()
	FVector WorldOrigin;

	TActorPool<AActor> ArrowPool;
	
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess=true))
	TSubclassOf<AActor> ArrowClass;
};
