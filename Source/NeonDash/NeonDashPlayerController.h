// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NeonDashPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class NEONDASH_API ANeonDashPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void SetupInputComponent() override;

private:
	void OnStartKey();
	void OnSelectKey();
};
