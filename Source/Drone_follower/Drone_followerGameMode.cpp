// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "Drone_followerGameMode.h"
#include "Drone_followerPawn.h"

ADrone_followerGameMode::ADrone_followerGameMode()
{
	// set default pawn class to our flying pawn
	DefaultPawnClass = ADrone_followerPawn::StaticClass();
}
