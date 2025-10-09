// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NpcInteractionComponent.generated.h"

class ANpcCharacter;
class UNpcManager;
class UTask;
class UOptionsWidget;
/**
 *  Add this to the player. Tells an NPC what to do through the NpcManager
 */
UCLASS(classGroup = (Custom), meta = (BlueprintSpawnableComponent))
class APESACK_PICKLEBALLS_API UNpcInteractionComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UNpcInteractionComponent();

	UPROPERTY(BlueprintReadWrite)
	TWeakObjectPtr<ANpcCharacter> SelectedNpc;
};
