// Shoot Them Up Game. All rights reserved.


#include "Menu/UI/STUMenuHUD.h"
#include "UI/STUBaseWidget.h"
#include "Menu/STUMenuGameModeBase.h"
void ASTUMenuHUD::BeginPlay()
{
	Super::BeginPlay();

    MenuWidgets.Add(ESTUMenuState::MainMenu, CreateWidget<USTUBaseWidget>(GetWorld(), MenuWidgetClass));
    MenuWidgets.Add(ESTUMenuState::Loading, CreateWidget<USTUBaseWidget>(GetWorld(), LoadingWidgetClass));

    for (auto MenuWidgetPair : MenuWidgets)
    {
        const auto MenuWidget = MenuWidgetPair.Value;
        if (!MenuWidget) continue;
        MenuWidget->AddToViewport();
        MenuWidget->SetVisibility(ESlateVisibility::Hidden);
    }

    if (GetWorld())
    {
        const auto GameMode = Cast<ASTUMenuGameModeBase>(GetWorld()->GetAuthGameMode());
        if (GameMode)
        {
            GameMode->OnMenuStateChanged.AddUObject(this, &ASTUMenuHUD::OnMenuStateChanged);
        }
    }
}

void ASTUMenuHUD::OnMenuStateChanged(ESTUMenuState State)
{
    if (CurrentWidget)
    {
        CurrentWidget->SetVisibility(ESlateVisibility::Hidden);
    }

    if (MenuWidgets.Contains(State))
    {
        CurrentWidget = MenuWidgets[State];
    }

    if (CurrentWidget)
    {
        CurrentWidget->SetVisibility(ESlateVisibility::Visible);
        CurrentWidget->Show();
    }
}