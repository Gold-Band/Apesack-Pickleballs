#include "Managers/NpcManager.h"
#include "Managers/NpcDelegates.h"

UNpcManager::UNpcManager()
{
	
}

UNpcManager* UNpcManager::Get(const UObject* WorldContextObject)
{
	if (GEngine)
	{
		const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
		if (World)
		{
			return World->GetSubsystem<UNpcManager>();
		}
	}
	return nullptr;
}

void UNpcManager::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	
	// cache all buildings
	
}

void UNpcManager::SetWorldOrigin(const FVector& NewWorldOrigin)
{
	WorldOrigin = NewWorldOrigin;
}
