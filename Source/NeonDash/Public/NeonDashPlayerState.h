// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "NeonDashPlayerState.generated.h"

class UReplicatedStateComponent;

UCLASS()
class NEONDASH_API ANeonDashPlayerState : public APlayerState
{
	GENERATED_BODY()

	ANeonDashPlayerState();

public:
	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	void AddScore(float ScoreDelta);

	/** Gets the literal value of health. */
	float GetHealth() const;

	/** Gets the literal value of health. */
	float GetPlayerTeamNumber() const;

	/** Sets the value of Health without causing other side effects to this instance. */
	void SetHealth(const float NewHealth);

	/** Sets the state of the Player in particular its team number and health. */
	void SetPlayerState(uint8 teamNum);

private:
	UPROPERTY(VisibleAnywhere, Category = "RepStateComp")
	UReplicatedStateComponent* RepStateComp;
};
