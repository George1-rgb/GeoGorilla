// Shoot Them Up Game. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/STUBaseWeapon.h"
#include "STURifileWeapon.generated.h"



class USTUWeaponFXComponent;
class UNiagaraComponent;
class UNiagaraSystem;

UCLASS()
class SHOOTTHEMUP_API ASTURifileWeapon : public ASTUBaseWeapon
{
    GENERATED_BODY()
public:
    ASTURifileWeapon();
    virtual void StartFire() override;
    virtual void StopFire() override;

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    float TimerBetweenShots = 0.1f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    float BulletSpread = 1.5f;

     UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
     float DamageAmount = 10.0f;

     UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VFX")
     UNiagaraSystem* TraceFX;

     UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Heal")
     TSubclassOf<UCameraShakeBase> CameraShake;

     UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VFX")
     FString TraceTargetName = "TraceTarget";

     UPROPERTY(VisibleAnywhere, Category = "VFX")
     USTUWeaponFXComponent* WeaponFXComponent;

	 UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
		 TSubclassOf<UDamageType> DamageType;

     virtual void BeginPlay() override;
     virtual void MakeShot() override;
     virtual bool GetTraceData(FVector& TraceStart, FVector& TraceEnd) const;
    

private:
    FTimerHandle ShotTimerHandle;

    UPROPERTY()
    UNiagaraComponent* MuzzleFXComponent;

    void MakeDamage(const FHitResult& HitResult);
    void InitMuzzleFX();
    void SetMuzzleFXVisibility(bool Visible);
    void SpawnTraceFX(const FVector& TraceStart, const FVector& TraceEnd);
    void PlayCameraShake();
    AController* GetController() const;
};
