// Fill out your copyright notice in the Description page of Project Settings.


#include "NeonDashPlayerState.h"
#include "HealthComponent.h"


ANeonDashPlayerState::ANeonDashPlayerState()
{
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComp"));
}

void ANeonDashPlayerState::AddScore(float ScoreDelta)
{
	SetScore(GetScore() + ScoreDelta);
}

float ANeonDashPlayerState::GetHealth() const
{
	//return GetNetOwningPlayer()->GetPlayerController()->GetPawn()->GetComponentByClass<HealthComponent>()->Health;
	return .0f;
}

void ANeonDashPlayerState::SetHealth(const float NewHealth)
{
}
