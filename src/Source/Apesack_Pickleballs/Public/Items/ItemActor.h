// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagAssetInterface.h"
#include "GameFramework/Actor.h"
#include "ItemActor.generated.h"

class UPaperSpriteComponent;
class UBoxComponent;

UCLASS()
class APESACK_PICKLEBALLS_API AItemActor : public AActor, public IGameplayTagAssetInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemActor();

	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;

	UFUNCTION(BlueprintCallable)
	virtual void PickedUp(AActor* ActorPickingUp);

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void OnPickedUp(AActor* ActorPickingUp);
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	FGameplayTagContainer ItemTags;

	UPROPERTY(Category=Sprite, VisibleAnywhere, BlueprintReadOnly, meta=(ExposeFunctionCategories="Sprite,Rendering,Physics,Components|Sprite", AllowPrivateAccess="true"))
	TObjectPtr<UPaperSpriteComponent> RenderComponent;

};
