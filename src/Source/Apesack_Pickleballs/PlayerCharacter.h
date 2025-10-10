#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputMappingContext.h"
#include "Interactable.h"
#include "PlayerCharacter.generated.h"

class UCameraComponent;
class UFloatingPawnMovement;

UCLASS()
class APESACK_PICKLEBALLS_API APlayerCharacter : public APawn {
	GENERATED_BODY()

public:
	APlayerCharacter();
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	int Coins;

	UFUNCTION(BlueprintCallable)
	void IncrementCoins();

	UFUNCTION(BlueprintCallable)
	bool SpendCoins(int requestedCoins);
	
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool LoggingEnabled;
	
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
	
private:
	FVector CharacterLastPosition;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UInputMappingContext> Gameplay_IMC;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TSoftObjectPtr<UInputAction> SprintAction;

	UPROPERTY(VisibleAnywhere, meta=(AllowPrivateAccess=true))
	TObjectPtr<UFloatingPawnMovement> MovementComp;

	UPROPERTY(EditAnywhere, Category = "Movement")
    float SprintMultiplier = 2.0f;
    
// Sprint handlers
	void StartSprinting(const struct FInputActionInstance& Instance);
void StopSprinting(const struct FInputActionInstance& Instance);

	float DefaultSpeed = 2.0f;
    private: bool bIsSprinting = false;

	UFUNCTION()
	void HandleMove(const FInputActionInstance& Instance);
	
	void PrintCoins() const;
};