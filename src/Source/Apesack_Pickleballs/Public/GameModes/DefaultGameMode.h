// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorPool.h"
#include "GameFramework/GameModeBase.h"
#include "WorldClock/WorldClockSubsystem.h"
#include "DefaultGameMode.generated.h"

struct FTimestamp;
class UNpcManager;
class UBuildingsManager;
enum EBuildingType : uint8;
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
	void NewBuilding(ABuildingBase* Building, const EBuildingType BuildingType);
	void BuildingDestroyed(ABuildingBase* Building, const EBuildingType BuildingType);
	
	static float GetAngleBetweenVectors(const FVector& A, const FVector& B);
	
	const FVector WorldOriginNormal = FVector(0.0f, 1.0f, 0.0f);
	
	UFUNCTION(BlueprintPure)
	AActor* GetArrow();
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bEnableClock = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTimestamp GameStartTime = FTimestamp(0,8,0,0);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float GameTimeScale = 1000.f;
	
private:
	virtual void BeginPlay() override;
	void InitializeLocalBuildingsManagerReference();
	void InitializeLocalNpcManagerReference();
	
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
	TObjectPtr<UBuildingsManager> BuildingsManager;
	
	UPROPERTY()
	TObjectPtr<UNpcManager> NpcManager;
	
	UPROPERTY()
	TObjectPtr<UWorldClockSubsystem> WorldClock;

	TActorPool<AActor> ArrowPool;
	
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess=true))
	TSubclassOf<AActor> ArrowClass;
};
