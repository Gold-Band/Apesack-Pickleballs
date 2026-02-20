#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputMappingContext.h"
#include "PlayerCharacter.generated.h"

enum class EOriginSide : uint8;
class UStatsComponent;
class UCameraComponent;
class UCircularPawnMovementComponent;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnMovedSignature, float, float);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEnterBattleFormationSignature, EOriginSide, Side);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FExitBattleFormationSignature);

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

	UPROPERTY(BlueprintCallable)
	FEnterBattleFormationSignature EnterBattleFormationDelegate;
	
	UPROPERTY(BlueprintCallable)
	FExitBattleFormationSignature ExitBattleFormationDelegate;
	
	FOnMovedSignature OnMovedDelegate;
	TArray<bool> PartyOrder{false, false, false, false};
	int PartySize = 0;
	
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool LoggingEnabled;
	
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float KnockbackDistance = 150;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float KnockbackHeight = 30;
	
	UPROPERTY()
	TObjectPtr<UStatsComponent> Stats;
	
private:
	FVector CharacterLastPosition;
	
	UPROPERTY(EditAnywhere)
	float Radius = 100;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UInputMappingContext> Gameplay_IMC;

	UPROPERTY(EditAnywhere, Category = "Input")
	TSoftObjectPtr<UInputAction> MoveAction;
	
	UPROPERTY(EditAnywhere, Category = "Input")
	TSoftObjectPtr<UInputAction> LazyMoveAction;
	
	UPROPERTY(EditAnywhere, Category = "Input")
	TSoftObjectPtr<UInputAction> SprintAction;
	
	UPROPERTY(VisibleAnywhere, meta=(AllowPrivateAccess=true))
	TObjectPtr<UCircularPawnMovementComponent> MovementComp;

	UPROPERTY(EditAnywhere, Category = "Movement")
    float SprintMultiplier = 0.5f;
    
// Sprint handlers
	void StartSprinting(const struct FInputActionInstance& Instance);
void StopSprinting(const struct FInputActionInstance& Instance);

	float DefaultSpeed = 0.1f;
    private: bool bIsSprinting = false;

	UFUNCTION()
	void HandleMove(const FInputActionInstance& Instance);
	
	UFUNCTION()
	void LazyMove(const FInputActionInstance& Instance);
	
	void Move(const FVector& Direction);
	
	void PrintCoins() const;
	
	UFUNCTION()
	void OnDeath();
	
	UFUNCTION()
	void OnDamaged(float DamageRecieved, float UpdatedHealth, int DamageType, AActor* InstigatorActor);
	
	bool bWasHit = false;
};