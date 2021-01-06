// Copyright Epic Games, Inc. All Rights Reserved.

#include "NeonDashGameMode.h"
#include "NeonDashPawn.h"

ANeonDashGameMode::ANeonDashGameMode()
{
	// set default pawn class to our character class
	DefaultPawnClass = ANeonDashPawn::StaticClass();
}

