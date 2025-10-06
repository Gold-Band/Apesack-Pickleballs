// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NpcInteractionComponent.generated.h"

class UOptionsWidget;
class ANpcCharacter;
/**
 *  Add this to the player. Tells the player how to interact with an NPC
 */
UCLASS(classGroup = (Custom), meta = (BlueprintSpawnableComponent))
class APESACK_PICKLEBALLS_API UNpcInteractionComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UNpcInteractionComponent();

	UFUNCTION(BlueprintPure)
	bool HasValidNpc() const {return SelectedNpc != nullptr;}

	UFUNCTION(BlueprintPure)
	bool IsInteracting() const;
	
	UFUNCTION(BlueprintCallable)
	void StartInteraction();

	UFUNCTION(BlueprintCallable)
	void EndInteraction();

	UFUNCTION(BlueprintCallable)
	void SelectNpc(ANpcCharacter* OtherNpc);

	UFUNCTION(BlueprintCallable)
	void UnSelectNpc();

	UFUNCTION(BlueprintCallable)
	void SelectNextOption();

	UFUNCTION(BlueprintCallable)
	void ConfirmSelection();
	
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TObjectPtr<ANpcCharacter> SelectedNpc;

	UPROPERTY()
	TObjectPtr<UOptionsWidget> SelectedNpcOptions;
	
};
