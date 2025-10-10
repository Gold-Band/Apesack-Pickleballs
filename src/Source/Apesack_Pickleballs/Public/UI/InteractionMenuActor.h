
#pragma once

#include "CoreMinimal.h"
#include "InteractionMenuActor.generated.h"


class UGridNode;
class UNpcManager;
class UOptionsWidget;
class ANpcCharacter;
struct FClassInfo;
class UWidgetComponent;

UENUM(BlueprintType)
enum class EInteractionContext : uint8
{
	NpcCharacter,
	Building
};

UCLASS()
class AInteractionMenuActor: public AActor
{
	GENERATED_BODY()

public:
	AInteractionMenuActor();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	
	void SetFollowActor(AActor* Actor);

	template <typename T>
	void OpenInteractionDialog(T* Actor)
	{
		UE_LOG(LogTemp, Error, TEXT("AInteractionMenuActor::OpenInteractionDialog - Unknown interactable type!"));
	}

	template <>
	void OpenInteractionDialog<ANpcCharacter>(ANpcCharacter* Actor);

	void CloseInteractionDialog();

	UGridNode* GetMostRelevantNode();
	
private:
	
	void SetInteractionContext(EInteractionContext Context);

	UPROPERTY()
	TWeakObjectPtr<UNpcManager> NpcManager;

	UPROPERTY(EditAnywhere)
	FVector FollowOffset = FVector(0, 50, 130);
	
	UPROPERTY()
	AActor* FollowActor;
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess=true))
	TObjectPtr<UWidgetComponent> WidgetComponent;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> NpcInteractionWidgetClass;
	
	UPROPERTY()
	TObjectPtr<UOptionsWidget> NpcInteractionWidget;
};
