// Shoot Them Up Game. All rights reserved.


#include "Menu/UI/STUMenuWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "STUGameInstance.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Menu/STUMenuGameModeBase.h"
void USTUMenuWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (StartGameButton && QuitGameButton)
	{
		StartGameButton->OnClicked.AddDynamic(this, &USTUMenuWidget::OnStartGame);
		QuitGameButton->OnClicked.AddDynamic(this, &USTUMenuWidget::OnQuitGame);
		
	}
}

void USTUMenuWidget::OnStartGame()
{
	if (!GetWorld()) return;
	const auto GameMode = Cast<ASTUMenuGameModeBase>(GetWorld()->GetAuthGameMode());	
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &USTUMenuWidget::StartGame, 3, false);
	GameMode->SetMenuLoading();
}

void USTUMenuWidget::StartGame()
{
	if (!GetWorld()) return;

	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);

	const auto STUGameInstance = GetWorld()->GetGameInstance<USTUGameInstance>();
	if (!STUGameInstance) return;

	if (STUGameInstance->GetStartupLevelName().IsNone())
	{
		return;
	}
	UGameplayStatics::OpenLevel(this, STUGameInstance->GetStartupLevelName());
}

void USTUMenuWidget::OnQuitGame()
{
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, true);
}