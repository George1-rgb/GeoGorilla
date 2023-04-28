// Shoot Them Up Game. All rights reserved.


#include "Menu/STUMenuPlayerController.h"
#include "STUGameInstance.h"
void ASTUMenuPlayerController::BeginPlay()
{
	Super::BeginPlay();

	SetInputMode(FInputModeUIOnly());
	bShowMouseCursor = true;

}