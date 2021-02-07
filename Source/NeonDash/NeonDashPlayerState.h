// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "NeonDashPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class NEONDASH_API ANeonDashPlayerState : public APlayerState
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	void AddScore(float ScoreDelta);

public:

	/** Gets the literal value of health. */
	float GetHealth() const;

	/** Sets the value of Health without causing other side effects to this instance. */
	void SetHealth(const float NewHealth);
};
