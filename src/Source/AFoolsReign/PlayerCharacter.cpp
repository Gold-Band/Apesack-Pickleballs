#include "PlayerCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "FCTween.h"
#include "InputActionValue.h"
#include "StatsComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameModes/DefaultGameMode.h"
#include "Managers/BuildingsManager.h"
#include "Movement/CircularPawnMovementComponent.h"

// Sets default values
APlayerCharacter::APlayerCharacter() {
	PrimaryActorTick.bCanEverTick = true;

	MovementComp = CreateDefaultSubobject<UCircularPawnMovementComponent>(TEXT("Movement"));
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay() {
	Super::BeginPlay();

	if(const APlayerController* PC = Cast<APlayerController>(GetController())) {
		if(const ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(PC->GetLocalPlayer())) {
			if(UEnhancedInputLocalPlayerSubsystem* InputSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>()) {
				if(!Gameplay_IMC.IsNull()) {
					InputSystem->AddMappingContext(Gameplay_IMC.LoadSynchronous(), 1);
				}
			}
		}
	}
	
	Radius = GetActorLocation().Size2D();
	DefaultSpeed = MovementComp->MaxSpeed;
	
	Stats = Cast<UStatsComponent>(GetComponentByClass<UStatsComponent>());
	if (Stats)
	{
		Stats->OnDeathDelegate.AddUniqueDynamic(this, &ThisClass::OnDeath);
		Stats->OnDamagedDelegate.AddUniqueDynamic(this, &ThisClass::OnDamaged);
	}
}

void APlayerCharacter::BeginDestroy() {
	FCTween::ClearActiveTweens();
	Super::BeginDestroy();
}

EOriginSide APlayerCharacter::GetActorSide(AActor* Actor) const
{
	if (!Actor) return EOriginSide::Any;
	const float Angle = ADefaultGameMode::GetAngleBetweenVectors(GetActorLocation(), Actor->GetActorLocation());
	return Angle > 0? EOriginSide::Left : EOriginSide::Right;
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if(UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if(!MoveAction.IsNull())
		{
			Input->BindAction(MoveAction.LoadSynchronous(), ETriggerEvent::Triggered, this, &APlayerCharacter::HandleMove);
			Input->BindAction(MoveAction.LoadSynchronous(), ETriggerEvent::Completed, this, &APlayerCharacter::OnStoppedMoving);
		}

		if (!SprintAction.IsNull())
		{
			UInputAction* Sprint = SprintAction.LoadSynchronous();
			Input->BindAction(Sprint, ETriggerEvent::Started, this, &APlayerCharacter::StartSprinting);
			Input->BindAction(Sprint, ETriggerEvent::Completed, this, &APlayerCharacter::StopSprinting);
			Input->BindAction(Sprint, ETriggerEvent::Canceled, this, &APlayerCharacter::StopSprinting);
		}

		if (!LazyMoveAction.IsNull())
		{
			Input->BindAction(LazyMoveAction.LoadSynchronous(), ETriggerEvent::Triggered, this, &APlayerCharacter::LazyMove);
			Input->BindAction(LazyMoveAction.LoadSynchronous(), ETriggerEvent::Completed, this, &APlayerCharacter::OnStoppedMoving);
		}
	}
}

void APlayerCharacter::HandleMove(const FInputActionInstance& Instance){
	FVector Value = Instance.GetValue().Get<FVector>();
	Move(Value);
}

void APlayerCharacter::LazyMove(const FInputActionInstance& Instance)
{
	float x, y;
	
	if (Cast<APlayerController>(GetController())->GetMousePosition(x,y))
	{
		FVector2D Size;
		GetWorld()->GetGameViewport()->GetViewportSize(Size);

		if (x > Size.X*0.95 || x < Size.X*0.05) StartSprinting(Instance);
		else StopSprinting(Instance);

		x = x > Size.X/2? 1: -1;
		const FVector Value {x,0,0};	
		
		Move(Value);
	}
}

void APlayerCharacter::OnStoppedMoving(const FInputActionInstance& Instance)
{
	MoveDirection = 0;

	if (ExitBattleFormationDelegate.IsBound() && !bSensesHostiles)
		ExitBattleFormationDelegate.Broadcast();
}

void APlayerCharacter::Move(const FVector& Direction)
{
	if (!MovementComp) return;
	
	
	if (bIsSprinting) MovementComp->MaxSpeed = DefaultSpeed * SprintMultiplier;
	else MovementComp->MaxSpeed = DefaultSpeed;

	if (Direction.X != 0)
	{
		const bool bMovingLeft = Direction.X < 0;

		if (bMovingLeft != bLastDirection)
		{
			bLastDirection = bMovingLeft;
			FlipDirection(bMovingLeft);
		}
	}

	// Boundary check
	if (bInWorldBoundary)
	{
		if (bRecalculateSide)
		{
			bRecalculateSide = false;
			MainSide = ADefaultGameMode::GetActorSideFromOrigin(this);
		}
		
		if (Direction.X > 0 && MainSide == EOriginSide::Right)
		{
			MovementComp->Velocity = FVector::Zero();
			return;
		}
		if (Direction.X < 0 && MainSide == EOriginSide::Left)
		{
			MovementComp->Velocity = FVector::Zero();
			return;
		}
	}

	MoveDirection = Direction.X;
	
	AddMovementInput(Direction.X * GetActorForwardVector());

	if (OnMovedDelegate.IsBound())
		OnMovedDelegate.Broadcast(Direction.X, MovementComp->MaxSpeed);
}

void APlayerCharacter::OnDeath()
{
}

void APlayerCharacter::OnDamaged(float DamageRecieved, float UpdatedHealth, int DamageType, AActor* InstigatorActor)
{
	if (bWasHit) return;
	bWasHit = true;
	
	int InstigatorDirection = 1;

	if (InstigatorActor)
	{
		float Angle = ADefaultGameMode::GetAngleBetweenVectors(GetActorLocation(), InstigatorActor->GetActorLocation());
		if (Angle > 0) InstigatorDirection = -1;
	}
	
	const FVector Start = GetActorLocation();
	const FVector End = Start - GetActorForwardVector() * KnockbackDistance * InstigatorDirection;
	const float Duration = 0.3f; 
	
	FCTween::Play(
	Start,
	End,
	[&](const FVector& t)
	{
		if (!this) return;
		SetActorLocation(t);
	},
	Duration,
	EFCEase::OutQuad)->SetOnComplete([&]()
	{
		bWasHit = false;
	})->SetAutoDestroy(true);
	
	FCTween::Play(
	Start,
	Start + FVector::UpVector * KnockbackHeight,
	[&](const FVector& t)
	{
		if (!this) return;
		const FVector Location = GetActorLocation();
		SetActorLocation(FVector{Location.X, Location.Y, t.Z});
	},
	Duration/4,
	EFCEase::OutQuad)->SetYoyo(true)->SetAutoDestroy(true);
}

void APlayerCharacter::StartSprinting(const FInputActionInstance& Instance) {
	bIsSprinting = true;
}

void APlayerCharacter::StopSprinting(const FInputActionInstance& Instance) {
	bIsSprinting = false;
}