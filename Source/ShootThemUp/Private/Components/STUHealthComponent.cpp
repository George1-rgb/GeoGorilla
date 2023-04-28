// Shoot Them Up Game. All rights reserved.

#include "Components/STUHealthComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Camera/CameraShakeBase.h"
#include "STUGameModeBase.h"
#include "Sound/SoundCue.h"
#include "../Player/STUPlayerCharacter.h"
#include "../Weapon/DamageTypes/ProjectTileDamageType.h"
#include "../Weapon/DamageTypes/RifileDamageType.h"

DEFINE_LOG_CATEGORY_STATIC(LogHealthComponent, All, All);

USTUHealthComponent::USTUHealthComponent()
{

    PrimaryComponentTick.bCanEverTick = false;
}

void USTUHealthComponent::BeginPlay()
{
    Super::BeginPlay();
    check(MaxHealth > 0);
    SetHealth(MaxHealth);

    AActor* ComponentOwner = GetOwner();
    if (ComponentOwner)
    {
        ComponentOwner->OnTakeAnyDamage.AddDynamic(this, &USTUHealthComponent::OnTakeAnyDamage);
        ComponentOwner->OnTakePointDamage.AddDynamic(this, &USTUHealthComponent::OnTakePointDamage);
        ComponentOwner->OnTakeRadialDamage.AddDynamic(this, &USTUHealthComponent::OnTakeRadialDamage);
    }
}

void USTUHealthComponent::OnTakeAnyDamage(
    AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
    if (DamageType != GetDefault<UDamageType>())
        return;

    if (Damage <= 0.0f || IsDead() || !GetWorld())
        return;

    ASTUPlayerCharacter* pPlayerCharacter = nullptr;
    if (DamageCauser)
        pPlayerCharacter = Cast<ASTUPlayerCharacter>(DamageCauser->GetOwner());


    if (pPlayerCharacter)
        pPlayerCharacter->MakeDamage();


    SetHealth(Health - Damage);

    DefaultTakeDamage(InstigatedBy);
}

void USTUHealthComponent::OnTakePointDamage(AActor* DamagedActor, float Damage, class AController* InstigatedBy, FVector HitLocation,
    class UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const class UDamageType* DamageType, AActor* DamageCauser)
{
    if (DamageType != GetDefault<URifileDamageType>())
        return;

    if (Damage <= 0.0f || IsDead() || !GetWorld())
        return;
	ASTUPlayerCharacter* pPlayerCharacter = nullptr;
	if (DamageCauser)
		pPlayerCharacter = Cast<ASTUPlayerCharacter>(DamageCauser->GetOwner());


	if (pPlayerCharacter)
		pPlayerCharacter->MakeDamage();

    if (BoneName == HeadShotBoneName)
    {
        if (pPlayerCharacter)
            pPlayerCharacter->HeadShot();
        SetHealth(0.0);
    }
    else
        SetHealth(Health - Damage);

    DefaultTakeDamage(InstigatedBy);
}

void USTUHealthComponent::OnTakeRadialDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, FVector Origin, FHitResult HitInfo, 
    class AController* InstigatedBy, AActor* DamageCauser)
{
    if (Damage <= 0.0f || IsDead() || !GetWorld())
        return;

    if (DamageType != GetDefault<UProjectTileDamageType>())
        return;

    ASTUPlayerCharacter* pPlayerCharacter = nullptr;
    if (DamageCauser)
        pPlayerCharacter = Cast<ASTUPlayerCharacter>(DamageCauser->GetOwner());


    if (pPlayerCharacter)
        pPlayerCharacter->MakeDamage();

    SetHealth(Health - Damage);

    DefaultTakeDamage(InstigatedBy);
}

void USTUHealthComponent::HealUpdate()
{
    SetHealth(Health + HealModifier);

    if (IsHealthFull() && GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(HealTimerHandle);
    }
}

void USTUHealthComponent::SetHealth(float NewHealth)
{
    const auto NextHealth = FMath::Clamp(NewHealth, 0.0f, MaxHealth);
    const auto HealthDelta = NextHealth - Health;
    Health = NextHealth;
    OnHealthChanged.Broadcast(Health, HealthDelta);
}

 bool USTUHealthComponent::TryToAddHealth(float HelthAmount) 
 {
     if(IsHealthFull() || IsDead()) return false;
     SetHealth(Health + HelthAmount);
     return true;
 }
bool USTUHealthComponent::IsHealthFull() const 
{
    return FMath::IsNearlyEqual(Health, MaxHealth);
}

 void USTUHealthComponent::PlayCameraShake() 
 {
     if (IsDead())
         return;
     const auto Player = Cast<APawn>(GetOwner());
     if (!Player)
         return;

     const auto Controller = Player->GetController<APlayerController>();
     if (!Controller || !Controller->PlayerCameraManager)
         return;

     Controller->PlayerCameraManager->StartCameraShake(CameraShake);
 }

 void USTUHealthComponent::Killed(AController* KillerController)
 {
     if (!GetWorld()) return;
     const auto GameMode = Cast<ASTUGameModeBase>(GetWorld()->GetAuthGameMode());
     if (!GameMode) return;
     const auto Player = Cast<APawn>(GetOwner());
     const auto VictimController = Player ? Player->Controller : nullptr;

     GameMode->Killed(KillerController, VictimController);
 }

 void USTUHealthComponent::DefaultTakeDamage(class AController* InstigatedBy)
 {
     GetWorld()->GetTimerManager().ClearTimer(HealTimerHandle);

     if (IsDead())
     {
         Killed(InstigatedBy);
         OnDeath.Broadcast();
     }
     else if (AutoHeal)
     {
         GetWorld()->GetTimerManager().SetTimer(HealTimerHandle, this, &USTUHealthComponent::HealUpdate, HealUpdateTime, true, HealDelay);
     }
     PlayCameraShake();
 }
