// Fill out your copyright notice in the Description page of Project Settings.


#include "NeonDashPlayerState.h"
#include "ReplicatedStateComponent.h"


ANeonDashPlayerState::ANeonDashPlayerState()
{
	RepStateComp = CreateDefaultSubobject<UReplicatedStateComponent>(TEXT("RepStateComp"));
}

void ANeonDashPlayerState::AddScore(float ScoreDelta)
{
	SetScore(GetScore() + ScoreDelta);
}

float ANeonDashPlayerState::GetHealth() const
{
	
	return RepStateComp->GetHealth();
}

float ANeonDashPlayerState::GetPlayerTeamNumber() const
{
	return RepStateComp->TeamNum;
}

void ANeonDashPlayerState::SetHealth(const float NewHealth)
{
	RepStateComp->SetHealth(NewHealth);
}

void ANeonDashPlayerState::SetPlayerState(uint8 teamNum)
{
	RepStateComp->TeamNum = teamNum;
}

