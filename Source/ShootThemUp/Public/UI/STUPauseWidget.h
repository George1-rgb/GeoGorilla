// Shoot Them Up Game. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/STUBaseWidget.h"
#include "STUPauseWidget.generated.h"

class UButton;
UCLASS()
class SHOOTTHEMUP_API USTUPauseWidget : public USTUBaseWidget
{
	GENERATED_BODY()
public:


protected:
	UPROPERTY(meta = (BindWidget))
		UButton* ClearPauseButton;

	UPROPERTY(meta = (BindWidget))
		UButton* MainMenuButton;

	virtual void NativeOnInitialized() override;
private:
	UFUNCTION()
		void OnClearPause();

	UFUNCTION()
		void OnMainMenu();
};
