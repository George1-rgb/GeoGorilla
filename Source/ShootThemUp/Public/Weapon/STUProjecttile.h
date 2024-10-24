// Shoot Them Up Game. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "STUProjecttile.generated.h"

class USTUWeaponFXComponent;
class USphereComponent;
class UProjectileMovementComponent;
class USoundBase;

UCLASS()
class SHOOTTHEMUP_API ASTUProjecttile : public AActor
{
    GENERATED_BODY()

public:
    ASTUProjecttile();
    void SetShotDirection(const FVector& Direction) { ShotDirection = Direction; }
    void SetHitResult(const FHitResult& res) { m_hitResult = res; }
protected:
    UPROPERTY(VisibleAnywhere, Category = "Weapon")
    USphereComponent* CollisionComponent;

    UPROPERTY(VisibleAnywhere, Category = "Weapon")
    UProjectileMovementComponent* MovementComponent;

    UPROPERTY(EditAnywhere, Category = "Weapon")
    float DamageRadius = 200.0f;

    UPROPERTY(EditAnywhere, Category = "Weapon")
    float DamageAmount = 70.0f;

    UPROPERTY(EditAnywhere, Category = "Weapon")
    bool DoFullDamage = false;

    UPROPERTY(EditAnywhere, Category = "Weapon")
    float LifeSeconds = 5.0f;

    UPROPERTY(EditAnywhere, Category = "Weapon")
    bool m_bRifile = false;

    
    UPROPERTY(VisibleAnywhere, Category = "VFX")
    USTUWeaponFXComponent* WeaponFXComponent;

    UPROPERTY(EditAnywhere, Category="Sound") 
    USoundBase* FireSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	TSubclassOf<UDamageType> DamageType;

    virtual void BeginPlay() override;

private:
    FVector ShotDirection;
    FHitResult m_hitResult;
    UFUNCTION()
    void OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse,
        const FHitResult& Hit);
    AController* GetController() const;
};
