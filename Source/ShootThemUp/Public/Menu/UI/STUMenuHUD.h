// Shoot Them Up Game. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "STUCoreTypes.h"
#include "STUMenuHUD.generated.h"

class USTUBaseWidget;
UCLASS()
class SHOOTTHEMUP_API ASTUMenuHUD : public AHUD
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
		TSubclassOf<UUserWidget> MenuWidgetClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
        TSubclassOf<UUserWidget> LoadingWidgetClass;

	virtual void BeginPlay() override;


private:

    UPROPERTY()
        TMap<ESTUMenuState, USTUBaseWidget*> MenuWidgets;

    UPROPERTY()
        USTUBaseWidget* CurrentWidget = nullptr;

    void OnMenuStateChanged(ESTUMenuState State);
};
