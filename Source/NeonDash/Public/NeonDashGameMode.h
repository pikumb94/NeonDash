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
	/* Player1 Material */
	UPROPERTY(Category = Material, EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UMaterialInterface* MaterialP1;
	/* Player2 Material */
	UPROPERTY(Category = Material, EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UMaterialInterface* MaterialP2;
	/* Player3 Material */
	UPROPERTY(Category = Material, EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UMaterialInterface* MaterialP3;
	/* Player4 Material */
	UPROPERTY(Category = Material, EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UMaterialInterface* MaterialP4;

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

	UFUNCTION()
	void SpawnPlayer(APlayerController* PC);

	UFUNCTION()
	void IngamePlayerKilled(AActor* VictimActor, AActor* KillerActor, AController* KillerController);

	UPROPERTY(BlueprintReadOnly, Category = "SpawnLocations")
	TArray<APlayerStart*> SpawnPoints;

	/** Pawn Class to spawn when player is dashing */
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class ANeonDashPawn> NeonDashPawnClass;
private:

	TArray<APlayerController*> PlayerControllers;
	int MaxNumPlayers = 4;
	class ANeonDashPawn* LastSpawnedPawn;
};



