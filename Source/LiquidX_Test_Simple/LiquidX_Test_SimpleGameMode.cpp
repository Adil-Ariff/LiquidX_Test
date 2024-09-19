// Copyright Epic Games, Inc. All Rights Reserved.

#include "LiquidX_Test_SimpleGameMode.h"
#include "LiquidX_Test_SimpleCharacter.h"
#include "UObject/ConstructorHelpers.h"

ALiquidX_Test_SimpleGameMode::ALiquidX_Test_SimpleGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

}
