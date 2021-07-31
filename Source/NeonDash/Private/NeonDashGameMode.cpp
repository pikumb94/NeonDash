// Copyright Epic Games, Inc. All Rights Reserved.

#include "NeonDashGameMode.h"
#include "NeonDashPawn.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraActor.h"
#include "GameFramework/PlayerStart.h"
#include "NeonDashPlayerController.h"
#include "InitPawn.h"
#include "NeonDashGameState.h"
#include "NeonDashPlayerState.h"

ANeonDashGameMode::ANeonDashGameMode()
{
	// set default pawn class to our character class
	DefaultPawnClass = AInitPawn::StaticClass();
	PlayerControllerClass = ANeonDashPlayerController::StaticClass();
	
	MaterialP1 = CreateDefaultSubobject<UMaterialInterface>(TEXT("P1Material"));
	MaterialP2 = CreateDefaultSubobject<UMaterialInterface>(TEXT("P2Material"));
	MaterialP3 = CreateDefaultSubobject<UMaterialInterface>(TEXT("P3Material"));
	MaterialP4 = CreateDefaultSubobject<UMaterialInterface>(TEXT("P4Material"));

	OnPlayerSpawn.AddDynamic(this, &ANeonDashGameMode::SpawnPlayer);
	OnActorKilled.AddDynamic(this, &ANeonDashGameMode::IngamePlayerKilled);
}

void ANeonDashGameMode::BeginPlay()
{
	TArray<AActor*> FoundClassActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), FoundClassActors);

	for (AActor* TActor : FoundClassActors)
	{
		APlayerStart* MyActor = Cast<APlayerStart>(TActor);

		if (MyActor != nullptr)
			SpawnPoints.Add(MyActor);
	}

	//One PC is always spawned by default
	int i = 1;

	while (i < MaxNumPlayers)
	{
		UGameplayStatics::CreatePlayer(GetWorld());

		i++;
	}

}


void ANeonDashGameMode::SpawnPlayer(APlayerController* PC) {

	APawn* ControlledPawn = PC->GetPawn();
	ANeonDashGameState* GS = GetWorld()->GetGameState<ANeonDashGameState>();

	if (ControlledPawn) {

		if (GS && !Cast<ANeonDashPawn>(ControlledPawn))
		{
			GS->SetIngamePlayers(GS->GetIngamePlayers() + 1);
			PlayerControllers.Add(PC);
		}

		ControlledPawn->Destroy();

	}

	int SpawnPointIndex = 0;
	PlayerControllers.Find(PC, SpawnPointIndex);
	/*if (GS)
	{
		SpawnPointIndex = FMath::Clamp(GS->GetIngamePlayers()-1,0, MaxNumPlayers-1);
	}*/

	APlayerStart* SpawnPoint = SpawnPoints[SpawnPointIndex];

	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.bNoFail = true;
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	LastSpawnedPawn = GetWorld()->SpawnActor<ANeonDashPawn>(NeonDashPawnClass, SpawnPoint->GetActorLocation(), SpawnPoint->GetActorRotation(), ActorSpawnParams);
	
	switch (SpawnPointIndex) {
		case 0:
			LastSpawnedPawn->InitShipMaterial(MaterialP1);
		break;
		case 1:
			LastSpawnedPawn->InitShipMaterial(MaterialP2);
		break;
		case 2:
			LastSpawnedPawn->InitShipMaterial(MaterialP3);
		break;
		case 3:
			LastSpawnedPawn->InitShipMaterial(MaterialP4);
		break;
	}
		
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, (PC->GetPawn()? PC->GetPawn()->GetName():FString("NO PAWN")));

	PC->Possess(LastSpawnedPawn);
	auto NDPlayerState = Cast<ANeonDashPlayerState>(LastSpawnedPawn->GetPlayerState());
	if (NDPlayerState)
		NDPlayerState->SetPlayerState(SpawnPointIndex + 1);
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, (PC->GetPawn() ? PC->GetPawn()->GetName() : FString("NO PAWN")));

	PC->SetViewTargetWithBlend(GameCamera);
	PC->bAutoManageActiveCameraTarget = false;
	
}

void ANeonDashGameMode::IngamePlayerKilled(AActor* VictimActor, AActor* KillerActor, AController* KillerController)
{
	//ANeonDashGameState* GS = GetWorld()->GetGameState<ANeonDashGameState>();
	//GS->SetIngamePlayers(GS->GetIngamePlayers() - 1);

	auto NDPawn = Cast<ANeonDashPawn>(VictimActor);
	NDPawn->ResetAllBarriers();
	VictimActor->Destroy();
	//@TODO: update scores and player total lives
}
