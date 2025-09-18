#include "PlayerCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

#include "GameFramework/FloatingPawnMovement.h"

// Sets default values
APlayerCharacter::APlayerCharacter() {
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MovementComp = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("Movement"));
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
}

void APlayerCharacter::BeginDestroy() {
	Super::BeginDestroy();
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		if (!MoveAction.IsNull()) {
			Input->BindAction(MoveAction.LoadSynchronous(), ETriggerEvent::Triggered, this, &APlayerCharacter::HandleMove);
		}
	}
}

void APlayerCharacter::HandleMove(const FInputActionInstance& Instance) {
	const FVector Value = Instance.GetValue().Get<FVector>();
	AddMovementInput(Value);
}
