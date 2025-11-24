
#pragma once

#include "CoreMinimal.h"
#include "NpcManager.generated.h"


class ANpcBase;
class ANpcFriendly;

UCLASS()
class UNpcManager: public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	UNpcManager();
	
	static UNpcManager* Get(const UObject* WorldContextObject);

	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	void SetWorldOrigin(const FVector& NewWorldOrigin);	
	
	void AddFriendly();
	void AddHostile();
	
private:
	FVector WorldOrigin;
	
	UPROPERTY()
	TArray<ANpcFriendly*> Friendlies;
	
	UPROPERTY()
	TArray<ANpcBase*> Hostiles;
	
};
