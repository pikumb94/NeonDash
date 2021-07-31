// Fill out your copyright notice in the Description page of Project Settings.


#include "NeonDashGameState.h"
#include "Net/UnrealNetwork.h"

int32 ANeonDashGameState::GetIngamePlayers()
{
	return CurrentIngamePlayers;
}

void ANeonDashGameState::SetIngamePlayers(int32 CurrInPlay)
{
	CurrentIngamePlayers = CurrInPlay;
}


void ANeonDashGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANeonDashGameState, CurrentIngamePlayers);
}