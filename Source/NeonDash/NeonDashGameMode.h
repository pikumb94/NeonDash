// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "NeonDashGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerSpawnSignature, APlayerController*, PC);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnActorKilled, AActor*, VictimActor, AActor*, KillerActor, AController*, KillerController);

class ACameraActor;
class APlayerStart;

UCLASS(MinimalAPI)
class ANeonDashGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	//virtual void PostLogin(APlayerController* NewPlayer) override; IMPLEMENT THIS TO DELETE A PC ON SERVER WHEN A CLIENT LOGS-IN THE GAME => ONLINE W/ LOCAL TOGHETER!!!

public:
	ANeonDashGameMode();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	ACameraActor* GameCamera;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnPlayerSpawnSignature OnPlayerSpawn;

	UPROPERTY(BlueprintAssignable, Category = "GameMode")
	FOnActorKilled OnActorKilled;

	UFUNCTION(BlueprintImplementableEvent, Category = "GameMode")
	void SpawnPlayer(APlayerController* PC);

	UPROPERTY(BlueprintReadOnly, Category = "SpawnLocations")
	TArray<APlayerStart*> SpawnPoints;
private:
	

	int CurrNumPlayers = 0;//TO MOVE FROM HERE!
	int MaxNumPlayers = 4;

	APawn* LastSpawnedPawn;
};



