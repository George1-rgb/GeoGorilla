// Shoot Them Up Game. All rights reserved.

#include "UI/STUPlayerHUDWidget.h"
#include "Components/STUHealthComponent.h"
#include "Components/STUWeaponComponent.h"
#include "../Player/STUPlayerCharacter.h"
#include "STUUtils.h"
#include "../Weapon/STUBaseWeapon.h"
#include "Kismet/GameplayStatics.h"


float USTUPlayerHUDWidget::GetHelthPercent() const
{

    const auto HealthComponent = STUUtils::GetSTUPlayerComponent<USTUHealthComponent>(GetOwningPlayerPawn());
    if (!HealthComponent)
        return 0.0f;
    return HealthComponent->GetHealthPercent();
}

bool USTUPlayerHUDWidget::GetCurrentWeaponUIData(FWeaponUIData& UIData) const
{

    const auto WeaponComponent = STUUtils::GetSTUPlayerComponent<USTUWeaponComponent>(GetOwningPlayerPawn());
    if (!WeaponComponent)
        return false;
    return WeaponComponent->GetCurrentWeaponUIData(UIData);
}

bool USTUPlayerHUDWidget::GetCurrentWeaponAmmoData(FAmmoData& AmmoData) const
{

    const auto WeaponComponent = STUUtils::GetSTUPlayerComponent<USTUWeaponComponent>(GetOwningPlayerPawn());
    if (!WeaponComponent)
        return false;
    return WeaponComponent->GetCurrentWeaponAmmoData(AmmoData);
}

bool USTUPlayerHUDWidget::IsPlayerAlive() const
{

    const auto HealthComponent = STUUtils::GetSTUPlayerComponent<USTUHealthComponent>(GetOwningPlayerPawn());
    return HealthComponent && !HealthComponent->IsDead();
}

bool USTUPlayerHUDWidget::IsPlayerSpectating() const
{

    const auto Controller = GetOwningPlayer();
    return Controller && Controller->GetStateName() == NAME_Spectating;
}

bool USTUPlayerHUDWidget::IsShowCross() const
{
    return (IsPlayerAlive() && !m_bAiming);
}

void USTUPlayerHUDWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();
    if (GetOwningPlayer())
    {
        GetOwningPlayer()->GetOnNewPawnNotifier().AddUObject(this, &USTUPlayerHUDWidget::OnNewPawn);
        OnNewPawn(GetOwningPlayerPawn());
    }
}

void USTUPlayerHUDWidget::OnHealthChanged(float Health, float HealthDelta) 
{
    if (HealthDelta < 0.0f)
    {
        OnTakeDamage();
        if (!IsAnimationPlaying(DamageAnimation))
        {
            PlayAnimation(DamageAnimation);
        }
    }
    
}


void USTUPlayerHUDWidget::OnDamageActor()
{
    if (!IsAnimationPlaying(HitAnimation))
    {
        PlayAnimation(HitAnimation);
    }
    if (DamageMakeSound)
        UGameplayStatics::PlaySound2D(GetWorld(), DamageMakeSound);
}

void USTUPlayerHUDWidget::OnHeadShot()
{
    if (!IsAnimationPlaying(HeadShotAnimation))
    {
        PlayAnimation(HeadShotAnimation);
    }
    if (HeadShotSound)
        UGameplayStatics::PlaySound2D(GetWorld(), HeadShotSound);
}

void USTUPlayerHUDWidget::OnAiming(bool bAiming)
{
    
    m_bAiming = bAiming;
}

void USTUPlayerHUDWidget::OnNewPawn(APawn* NewPawn)
{
    const auto HealthComponent = STUUtils::GetSTUPlayerComponent<USTUHealthComponent>(NewPawn);
    if (HealthComponent && !HealthComponent->OnHealthChanged.IsBoundToObject(this) )
    {
        HealthComponent->OnHealthChanged.AddUObject(this, &USTUPlayerHUDWidget::OnHealthChanged);
    }

    if (NewPawn)
    {
        ASTUPlayerCharacter* pCharacter = Cast<ASTUPlayerCharacter>(NewPawn);
		if (pCharacter && (!pCharacter->OnDamageActor.IsBoundToObject(this) || !pCharacter->OnHeadShoot.IsBoundToObject(this)))
		{
			pCharacter->OnDamageActor.AddUObject(this, &USTUPlayerHUDWidget::OnDamageActor);
			pCharacter->OnHeadShoot.AddUObject(this, &USTUPlayerHUDWidget::OnHeadShot);
			pCharacter->OnAiming.AddUObject(this, &USTUPlayerHUDWidget::OnAiming);
		}
    }
}


