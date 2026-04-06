
#pragma once

#include "CoreMinimal.h"
#include "BuildingsManager.h"
#include "NpcManager.generated.h"


class ANpcCultist;
class APlayerCharacter;
class ANpc;
class ANpcFriendly;


UENUM(BlueprintType)
enum class ENpcSearchOption : uint8
{
	Any,
	AnyHostile,
	AnyFriendly
};

UENUM(BlueprintType)
enum class ENpcTag : uint8
{
	None = 0,
	Hostile = 1,
	Friendly = 2,
	Party = 3
};


// delegate for when an npc or building is the newly most exposed to the enemy
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnMostVulnerableAssetChangedSignature, AActor*, EOriginSide);

// delegate for when enemies are spotted near a wall
DECLARE_MULTICAST_DELEGATE_OneParam(FOnRaidDetectedSignature, EOriginSide);



UCLASS()
class UNpcManager: public UTickableWorldSubsystem
{
	GENERATED_BODY()
	
public:
	UNpcManager();
	
	
	//* 
	//* Npc Delegates
	//*
	static FOnMostVulnerableAssetChangedSignature OnMostVulnerableAssetChangedDelegate;
	static AActor* LeftMostVulnerableAsset;
	static AActor* RightMostVulnerableAsset;
	static FOnRaidDetectedSignature OnRaidDetectedDelegate;

	virtual ETickableTickType GetTickableTickType() const override;
	
	virtual TStatId GetStatId() const override;
	
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION(BlueprintPure, BlueprintCallable)
	static UNpcManager* Get(const UObject* WorldContextObject);

	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	void AddNpc(AActor* Npc, ENpcTag Tag, EOriginSide Side);
	void RemoveNpc(AActor* Npc, ENpcTag Tag);
	AActor* FindNearestNpc(const FVector& FromLocation, const ENpcSearchOption SearchFilter, const EOriginSide Side = EOriginSide::Any, const float CheckRadiusSquared = UE_MAX_FLT);
	AActor* FindNearestNpcOrPlayer(const FVector& FromLocation, const ENpcSearchOption SearchFilter, const EOriginSide Side = EOriginSide::Any, const float CheckRadiusSquared = UE_MAX_FLT);
	
	AActor* GetFarthestFriendlyNpc(EOriginSide Side);
	
	void SortByOriginAngle(TArray<AActor*>* SortArray);
	
	TArray<AActor*> GetNpcs(ENpcSearchOption SearchFilter, EOriginSide Side) const;
	
	EOriginSide SuggestOccupySide() const;
	
	float RaidDetectionDistance = 10000;
	
	APlayerCharacter* GetPlayer() const;
	
	float GetMaxSafeAngle(const EOriginSide Side) const;
	
	UFUNCTION(BlueprintCallable)
	void OnCultistDied(const EOriginSide Side);
	
	UFUNCTION()
	void OnDayTicked(uint8 Day);
	
	AActor* GetAttackable(const EOriginSide Side, const FVector& RefLocation);

	bool IsMostVulnerableAWall() const {return bMostVulnerableIsAWall;};
	
private:
	// Returns CheckActor if it passes the inspection. Else, it returns null.
	bool IsActorValidNearest(const AActor* CheckActor, const EOriginSide CheckSide, const float CheckDist, const float CheckRadiusSquared) const;
	bool IsCorrectSide(const EOriginSide Side, const FVector& WorldLocation) const;
	
	TArray<AActor*>* GetArray(ENpcSearchOption SearchFilter);
	
	float GetMostVulnerableAssetAndDistance(const EOriginSide Side, AActor*& OutActor);
	
	void RefreshNearbyVulnerables(const EOriginSide Side);
	
	void RecalculateSafeZone(const EOriginSide Side, const float FarthestWallAngle);
	
	void ReDrawSafeZoneBounds() const;
	
	bool bMostVulnerableIsAWall;
	//bool bMostVulnerableIsPlayer;
	
	UPROPERTY()
	AActor* PreviousLeftMostVulnerableAsset;
	
	UPROPERTY()
	AActor* PreviousRightMostVulnerableAsset; 
	
	UPROPERTY()
	TArray<AActor*> LeftVulnerables;
	
	UPROPERTY()
	TArray<AActor*> RightVulnerables;
	
	UPROPERTY()
	UBuildingsManager* BuildingsManager;
	
	//FVector WorldOrigin;
	
	UPROPERTY()
	TArray<AActor*> AllHostiles;
	
	UPROPERTY()
	TArray<AActor*> AllFriendlies;
	
	// tick interval
	float TickInterval = 0.2f;
	float Timer;
	
	// x = left side, y = right side
	FVector2D MaxSafeAngles {1.5f,1.5f}; // min value
	float SafeZoneWallPaddingAngle = 2.0f;
	
	UPROPERTY()
	APlayerCharacter* PlayerRef = nullptr;
	
	UPROPERTY()
	TSubclassOf<ANpcCultist> CultistClass;
	
	bool bDrawDebug_MostVulerable = false;
	bool bDrawDebug_VulnerableGroup = false;
	bool bDrawDebug_SafeZoneBounds = false;
};
