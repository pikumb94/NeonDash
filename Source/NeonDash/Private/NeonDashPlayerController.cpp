// Fill out your copyright notice in the Description page of Project Settings.


#include "NeonDashPlayerController.h"
#include "NeonDashGameMode.h"
#include "Components/InputComponent.h"

void ANeonDashPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindKey(EKeys::Gamepad_Special_Left, EInputEvent::IE_Pressed, this, &ANeonDashPlayerController::OnSelectKey);
	InputComponent->BindKey(EKeys::Gamepad_Special_Right, EInputEvent::IE_Pressed, this, &ANeonDashPlayerController::OnStartKey);

	InputComponent->BindKey(EKeys::One, EInputEvent::IE_Pressed, this, &ANeonDashPlayerController::OnStartKey);
	InputComponent->BindKey(EKeys::Two, EInputEvent::IE_Pressed, this, &ANeonDashPlayerController::OnSelectKey);

	//InputComponent->BindAction("SpawnKey", EInputEvent::IE_Pressed, this, &ANeonDashPlayerController::OnStartKey);
}

void ANeonDashPlayerController::OnStartKey()
{
	ANeonDashGameMode* N_GM = Cast<ANeonDashGameMode>(GetWorld()->GetAuthGameMode());
	if (N_GM)
		N_GM->OnPlayerSpawn.Broadcast(this);
}

void ANeonDashPlayerController::OnSelectKey()
{

}
