// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Npc.generated.h"

class UGoapAgent;

UCLASS()
class APESACK_PICKLEBALLS_API ANpc : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ANpc();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	template<typename T>
	struct TCallback {
		UPROPERTY(BlueprintCallable)
		static void Run(T Value) {
			UE_LOG(LogTemp, Warning, TEXT("Value: %d"), Value);
		}
	};

private:
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UGoapAgent> GoapAgent;

	
};
