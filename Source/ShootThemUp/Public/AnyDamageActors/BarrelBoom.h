// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "STUCoreTypes.h"
#include "BarrelBoom.generated.h"

class UCapsuleComponent;
class USTUHealthComponent;
class USoundBase;
class USTUWeaponFXComponent;

UCLASS()
class SHOOTTHEMUP_API ABarrelBoom : public APawn
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABarrelBoom();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
    USTUHealthComponent* HealthComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
    UStaticMeshComponent* m_pBarrelMesh;

	UPROPERTY(EditAnywhere, Category = "Weapon")
    float DamageRadius = 500.0f;

    UPROPERTY(EditAnywhere, Category = "Weapon")
    float DamageAmount = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(EditAnywhere, Category="Sound") 
    USoundBase* FireSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VFX")
    FImpactData DefaultImpactData;
	
    UPROPERTY(VisibleAnywhere, Category = "VFX")
    USTUWeaponFXComponent* WeaponFXComponent;

	virtual void OnHealthChanged(float Health, float HealthDelta);
	virtual void OnDeath();
private:
	AController* GetController() const;
};
