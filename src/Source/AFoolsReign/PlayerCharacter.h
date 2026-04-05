#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputMappingContext.h"
#include "PlayerCharacter.generated.h"

class FCTweenInstanceVector;
class ADefaultGameMode;
enum class EOriginSide : uint8;
class UStatsComponent;
class UCameraComponent;
class UCircularPawnMovementComponent;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnMovedSignature, float, float);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEnterBattleFormationSignature, EOriginSide, Side);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FExitBattleFormationSignature);

UCLASS()
class AFOOLSREIGN_API APlayerCharacter : public APawn {
	GENERATED_BODY()

public:
	APlayerCharacter();
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintPure)
	int GetMoveDirection() const { return MoveDirection; }
	
	UFUNCTION(BlueprintPure)
	bool GetIsSprinting() const { return bIsSprinting; }
	
	UPROPERTY(BlueprintCallable)
	FEnterBattleFormationSignature EnterBattleFormationDelegate;
	
	UPROPERTY(BlueprintCallable)
	FExitBattleFormationSignature ExitBattleFormationDelegate;
	
	FOnMovedSignature OnMovedDelegate;

	TArray<bool> PartyOrder{false, false, false, false};
	int PartySize = 0;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void FlipDirection(bool Direction);

	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void EnableDisableSprintAnim(bool Toggle);

	UPROPERTY()
	bool bLastDirection = false;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Character Properties")
	bool bInWorldBoundary = false;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Character Properties")
	bool bRecalculateSide = false;

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
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	EOriginSide GetActorSide(AActor* Actor) const;
	
	UPROPERTY(VisibleAnywhere, Category = "Character Properties")
	EOriginSide MainSide;
	
	UPROPERTY(BlueprintReadWrite)
	bool bSensesHostiles = false;
	
	UPROPERTY(BlueprintReadWrite)
	bool bCanMove = true;

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
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UCircularPawnMovementComponent> MovementComp;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float SprintMultiplier = 0.5f;
    
	void StartSprinting(const struct FInputActionInstance& Instance);
	void StopSprinting(const struct FInputActionInstance& Instance);

	float DefaultSpeed = 0.1f;
	bool bIsSprinting = false;
	int MoveDirection = 0;

	UFUNCTION()
	void HandleMove(const FInputActionInstance& Instance);
	
	UFUNCTION()
	void LazyMove(const FInputActionInstance& Instance);
	
	UFUNCTION()
	void OnStoppedMoving(const FInputActionInstance& Instance);
	
	void Move(const FVector& Direction);
	
	UFUNCTION()
	void OnDeath();
	
	UFUNCTION()
	void OnDamaged(float DamageRecieved, float UpdatedHealth, int DamageType, AActor* InstigatorActor);
	
	FCTweenInstanceVector* KnockbackTween;
	FCTweenInstanceVector* JumpTween;
	
	bool bWasHit = false;
};