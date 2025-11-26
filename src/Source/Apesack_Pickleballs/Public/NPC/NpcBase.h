// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagAssetInterface.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Pawn.h"
#include "Engine/Texture.h"
#include "Engine/DataTable.h"
#include "NpcBase.generated.h"

class UPaperSprite;
class UBoxComponent;
class UPaperSpriteComponent;
class UCircularPawnMovementComponent;

USTRUCT(BlueprintType)
struct FToolInfo : public FTableRowBase // row name is tool name
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag ToolTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDataTableRowHandle NextTool;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UTexture2D> ToolIcon;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UPaperSprite> ToolSprite;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int PurchaseCost = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int ToolDamage = 1;
};


USTRUCT(BlueprintType)
struct FClassInfo : public FTableRowBase // row name is class name
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSoftObjectPtr<class UTask>> ClassTasks;

	// not used
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag ClassTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDataTableRowHandle BaseTool;
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterDied);

UCLASS()
class APESACK_PICKLEBALLS_API ANpcBase : public APawn, public IGameplayTagAssetInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ANpcBase();
	
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;

	UFUNCTION(BlueprintCallable)
	void Flip();
	
	FVector GetForwardVector() const;
	
	UPROPERTY(BlueprintAssignable, Category="NPC")
	FOnCharacterDied OnDeath;
	
	UFUNCTION(BlueprintPure)
	float GetCharacterPreferredRadius() const;
	
	UFUNCTION(BlueprintPure)
	float GetDirectionToTown();
	
protected:
	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default")
	float MaxHp = 300.f;
	
private:
	float Hp;
	float Radius;
	
	// direction to the player's town (left or right)
	float OriginDirection = 0;
	bool bFwd = true;
	
	UFUNCTION()
	void OnTakeDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"), Category="Default")
	FGameplayTag CharacterTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	TObjectPtr<UBoxComponent> BoxCollider;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	TObjectPtr<UCircularPawnMovementComponent> MovementComp;

	UFUNCTION()
	void ApplyDamageListener(AActor* DamagedActor, float Damage, const class UDamageType* DamageType,
		class AController* InstigatedBy, AActor* DamageCauser);

protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	TObjectPtr<UPaperSpriteComponent> SpriteComp;
};
