// Shoot Them Up Game. All rights reserved.


#include "UI/STUPauseWidget.h"
#include "GameFramework/GameModeBase.h"
#include "Components/Button.h"
#include "STUGameInstance.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY_STATIC(LogSTUPauseWidget, All, All);

void USTUPauseWidget::NativeOnInitialized()
{
Super::NativeOnInitialized();

	if (ClearPauseButton && MainMenuButton)
	{
		ClearPauseButton->OnClicked.AddDynamic(this, &USTUPauseWidget::OnClearPause);
		MainMenuButton->OnClicked.AddDynamic(this, &USTUPauseWidget::OnMainMenu);
	}
}

void USTUPauseWidget::OnClearPause()
{
	if (!GetWorld() || !GetWorld()->GetAuthGameMode()) return;

	GetWorld()->GetAuthGameMode()->ClearPause();

}

void USTUPauseWidget::OnMainMenu()
{
	if (!GetWorld()) return;

	const auto STUGameInstance = GetWorld()->GetGameInstance<USTUGameInstance>();
	if (!STUGameInstance) return;

	if (STUGameInstance->GetMainMenuLevelName().IsNone())
	{
		UE_LOG(LogSTUPauseWidget, Display, TEXT("Level name is NONE"));
		return;
	}
	UGameplayStatics::OpenLevel(this, STUGameInstance->GetMainMenuLevelName());
}