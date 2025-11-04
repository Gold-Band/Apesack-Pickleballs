#include "PlayerCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "GameFramework/FloatingPawnMovement.h"
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
	}
}

void APlayerCharacter::HandleMove(const FInputActionInstance& Instance){
	FVector Value = Instance.GetValue().Get<FVector>();

	if (MovementComp) {
		if (bIsSprinting) {
			MovementComp->MaxSpeed = DefaultSpeed * SprintMultiplier;

			
		} else {
			MovementComp->MaxSpeed = DefaultSpeed;
		}
	}
	
	AddMovementInput(Value.X * GetActorForwardVector());
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


void APlayerCharacter::StartSprinting(const FInputActionInstance& Instance) {
	bIsSprinting = true;
}

void APlayerCharacter::StopSprinting(const FInputActionInstance& Instance) {
	bIsSprinting = false;
}


