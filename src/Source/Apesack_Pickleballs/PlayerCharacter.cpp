#include "PlayerCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "FCTween.h"
#include "InputActionValue.h"
#include "StatsComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameModes/DefaultGameMode.h"
#include "Movement/CircularPawnMovementComponent.h"

// Sets default values
APlayerCharacter::APlayerCharacter() {
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MovementComp = CreateDefaultSubobject<UCircularPawnMovementComponent>(TEXT("Movement"));
	DefaultSpeed = MovementComp->MaxSpeed; // Capture initial speed

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
	
	Stats = Cast<UStatsComponent>(GetComponentByClass<UStatsComponent>());
	if (Stats)
	{
		Stats->OnDeathDelegate.AddUniqueDynamic(this, &ThisClass::OnDeath);
		Stats->OnDamagedDelegate.AddUniqueDynamic(this, &ThisClass::OnDamaged);
	}
}

void APlayerCharacter::BeginDestroy() {
	Super::BeginDestroy();
}

void APlayerCharacter::IncrementCoins() {
	Coins++;
	PrintCoins();
}

bool APlayerCharacter::SpendCoins(int requestedCoins) {
	if(Coins < requestedCoins) return false;
	Coins -= requestedCoins;
	PrintCoins();
	return true;
}

void APlayerCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
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

void APlayerCharacter::Move(const FVector& Direction)
{
	if (MovementComp) {
		if (bIsSprinting) {
			MovementComp->MaxSpeed = DefaultSpeed * SprintMultiplier;

			
		} else {
			MovementComp->MaxSpeed = DefaultSpeed;
		}
	}
	
	AddMovementInput(Direction.X * GetActorForwardVector());
	if (OnMovedDelegate.IsBound()) OnMovedDelegate.Broadcast(Direction.X, MovementComp->MaxSpeed);
}


void APlayerCharacter::PrintCoins() const {
	if(!LoggingEnabled || !GEngine) return;
	GEngine->AddOnScreenDebugMessage(
		-1,
		1,
		FColor::Emerald,
		FString::Printf(TEXT("Coins: %i"), Coins)
	);
}

void APlayerCharacter::OnDeath()
{
}

void APlayerCharacter::OnDamaged(float DamageRecieved, float UpdatedHealth, int DamageType, AActor* InstigatorActor)
{
	if (bWasHit) return; // once at a time
	bWasHit = true;
	
	int InstigatorDirection = 1;
	if (InstigatorActor)
	{
		float Angle = ADefaultGameMode::GetAngleBetweenVectors(GetActorLocation(), InstigatorActor->GetActorLocation());
		if (Angle > 0) InstigatorDirection = -1;
	}
	
	const FVector Start = GetActorLocation();
	const FVector End = Start - GetActorForwardVector() * 150 * InstigatorDirection;
	const float Duration = 0.3f; 
	
	// move back
	FCTween::Play(
	Start,
	End,
	[&](const FVector& t)
	{
		SetActorLocation(t);
	},
	Duration,
	EFCEase::OutQuad)->SetOnComplete([&]()
	{
		bWasHit = false;
	});
	
	// jump
	FCTween::Play(
	Start,
	Start + FVector::UpVector * 30,
	[&](const FVector& t)
	{
		if (!this) return;
		const FVector Location = GetActorLocation();
		SetActorLocation(FVector{Location.X, Location.Y, t.Z});
	},
	Duration/4,
	EFCEase::OutQuad)->SetYoyo(true);
}


void APlayerCharacter::StartSprinting(const FInputActionInstance& Instance) {
	bIsSprinting = true;
}

void APlayerCharacter::StopSprinting(const FInputActionInstance& Instance) {
	bIsSprinting = false;
}


