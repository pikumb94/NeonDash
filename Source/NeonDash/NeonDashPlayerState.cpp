// Fill out your copyright notice in the Description page of Project Settings.


#include "NeonDashPlayerState.h"
#include "HealthComponent.h"
void ANeonDashPlayerState::AddScore(float ScoreDelta)
{
	SetScore(GetScore() + ScoreDelta);
}

float ANeonDashPlayerState::GetHealth() const
{
	return GetNetOwningPlayer()->GetPlayerController()->GetPawn()->GetComponentByClass<HealthComponent>()->Health;
}

void ANeonDashPlayerState::SetHealth(const float NewHealth)
{
}
