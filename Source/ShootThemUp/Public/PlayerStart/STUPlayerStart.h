// Shoot Them Up Game. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "STUPlayerStart.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTTHEMUP_API ASTUPlayerStart : public APlayerStart
{
	GENERATED_BODY()
public:
	int32 GetPlayerStartID() { return PlayerStartID; }
	UPROPERTY(BlueprintReadWrite, Category = "PlayerStartID")
		int32 PlayerStartID = 0;
};
