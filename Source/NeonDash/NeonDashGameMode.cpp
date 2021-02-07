// Copyright Epic Games, Inc. All Rights Reserved.

#include "NeonDashGameMode.h"
#include "NeonDashPawn.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraActor.h"
#include "GameFramework/PlayerStart.h"
#include "NeonDashPlayerController.h"
#include "InitPawn.h"

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

	int i = 0;

	while(i<MaxNumPlayers)
	{
		UGameplayStatics::CreatePlayer(GetWorld());
		
		i++;
	}
	
}

ANeonDashGameMode::ANeonDashGameMode()
{
	// set default pawn class to our character class
	DefaultPawnClass = AInitPawn::StaticClass();
	PlayerControllerClass = ANeonDashPlayerController::StaticClass();
	
	OnPlayerSpawn.AddDynamic(this, &ANeonDashGameMode::SpawnPlayer);
}
/*
void ANeonDashGameMode::SpawnPlayer(APlayerController* PC) {

	APawn* ControlledPawn = PC->GetPawn();

	if (ControlledPawn) {
		ControlledPawn->Destroy();
	}

	APlayerStart* SpawnPoint = SpawnPoints[CurrNumPlayers % SpawnPoints.Num()];

	FActorSpawnParameters ActorSpawnParams;
	LastSpawnedPawn = GetWorld()->SpawnActor<ANeonDashPawn>(ANeonDashPawn::StaticClass(), SpawnPoint->GetActorLocation(), SpawnPoint->GetActorRotation(), ActorSpawnParams);
	
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, (PC->GetPawn()? PC->GetPawn()->GetName():FString("NO PAWN")));

	PC->Possess(LastSpawnedPawn);
	
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, (PC->GetPawn() ? PC->GetPawn()->GetName() : FString("NO PAWN")));

	PC->SetViewTargetWithBlend(GameCamera);

	CurrNumPlayers++;
	
}*/