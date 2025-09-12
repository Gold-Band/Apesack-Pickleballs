#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputMappingContext.h"
#include "Dude.generated.h"

class UCameraComponent;
class UFloatingPawnMovement;

UCLASS()
class APESACK_PICKLEBALLS_API ADude : public APawn {
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ADude();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
public:    
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	FVector CharacterLastPosition;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UInputMappingContext> Gameplay_IMC;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UInputAction> MoveAction;

	UPROPERTY(VisibleAnywhere, meta=(AllowPrivateAccess=true))
	TObjectPtr<UFloatingPawnMovement> MovementComp;
    
	UFUNCTION()
	void HandleMove(const FInputActionInstance& Instance);
};
