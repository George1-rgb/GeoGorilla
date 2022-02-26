// Shoot Them Up Game. All rights reserved.


#include "Menu/STUMenuGameModeBase.h"
#include "Menu/STUMenuPlayerController.h"
#include "Menu/UI/STUMenuHUD.h"

ASTUMenuGameModeBase::ASTUMenuGameModeBase()
{
	PlayerControllerClass = ASTUMenuPlayerController::StaticClass();
	HUDClass = ASTUMenuHUD::StaticClass();
}

void ASTUMenuGameModeBase::StartPlay()
{
	Super::StartPlay();
	SetMenuState(ESTUMenuState::MainMenu);
}

void ASTUMenuGameModeBase::SetMenuLoading()
{
	SetMenuState(ESTUMenuState::Loading);
}

void ASTUMenuGameModeBase::SetMenuState(ESTUMenuState State)
{
	if (MatchState == State) return;
	MatchState = State;
	OnMenuStateChanged.Broadcast(MatchState);
}