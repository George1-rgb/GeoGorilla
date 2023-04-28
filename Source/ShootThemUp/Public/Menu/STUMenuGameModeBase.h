// Shoot Them Up Game. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "STUCoreTypes.h"
#include "STUMenuGameModeBase.generated.h"

UCLASS()
class SHOOTTHEMUP_API ASTUMenuGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	ASTUMenuGameModeBase();

	FOnMenuStateChangedSignature OnMenuStateChanged;
	virtual void StartPlay() override;
	void SetMenuLoading();
private:

	ESTUMenuState MatchState = ESTUMenuState::WaitingToStart;

	void SetMenuState(ESTUMenuState State);
};
