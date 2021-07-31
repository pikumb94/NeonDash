// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "NeonDashGameState.generated.h"

/**
 * 
 */
UCLASS()
class NEONDASH_API ANeonDashGameState : public AGameStateBase
{
	GENERATED_BODY()
	
	/* Total number of Player Controllers that possess a Pawn  */
	UPROPERTY(Replicated)
	int32 CurrentIngamePlayers {0};

public:
	int32 GetIngamePlayers();
	void SetIngamePlayers(int32 CurrInPlay);
};
