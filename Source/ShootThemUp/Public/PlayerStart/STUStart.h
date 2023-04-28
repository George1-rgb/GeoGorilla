// Shoot Them Up Game. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "STUStart.generated.h"

UCLASS()
class SHOOTTHEMUP_API ASTUStart : public AActor
{
	GENERATED_BODY()
	
public:	

	ASTUStart();
	int32 GetPlayerStartID() { return PlayerStartID; }
protected:

	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, Category = "PlayerStart")
		int32 PlayerStartID = 0;
};
