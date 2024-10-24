// Shoot Them Up Game. All rights reserved.

#include "Weapon/STUProjecttile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Weapon/Components/STUWeaponFXComponent.h"
#include "Sound/SoundBase.h"
#include "DamageTypes/ProjectTileDamageType.h"
#include "DamageTypes/RifileDamageType.h"


ASTUProjecttile::ASTUProjecttile()
{

    PrimaryActorTick.bCanEverTick = false;
    CollisionComponent = CreateDefaultSubobject<USphereComponent>("CollisionComponent");
    CollisionComponent->InitSphereRadius(5.0f);
    CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    CollisionComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
    CollisionComponent->bReturnMaterialOnMove = true;
    SetRootComponent(CollisionComponent);

    MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("MovementComponent");
    MovementComponent->InitialSpeed = 20000.0f;
    MovementComponent->ProjectileGravityScale = 0.3f;

    WeaponFXComponent = CreateDefaultSubobject<USTUWeaponFXComponent>("WeaponFXComponent");
}

void ASTUProjecttile::BeginPlay()
{
    Super::BeginPlay();

    check(MovementComponent);
    check(CollisionComponent);
    check(WeaponFXComponent);
    MovementComponent->Velocity = ShotDirection * MovementComponent->InitialSpeed;
    CollisionComponent->IgnoreActorWhenMoving(GetOwner(), true);
    CollisionComponent->OnComponentHit.AddDynamic(this, &ASTUProjecttile::OnProjectileHit);
    SetLifeSpan(LifeSeconds);
}

void ASTUProjecttile::OnProjectileHit(
    UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{

    if (!GetWorld())
        return;
    MovementComponent->StopMovementImmediately();
   
    if (!m_bRifile)
    {
        // make damage
        UGameplayStatics::ApplyRadialDamage(GetWorld(), //
            DamageAmount,                               //
            GetActorLocation(),                         //
            DamageRadius,                               //
            DamageType,                 //
            {},                                         //
            this,                                       //
            GetController(),                            //
            DoFullDamage);

    }
    else
    {
        const auto DamagedActor = m_hitResult.GetActor();
        FVector HitFromDirection = m_hitResult.TraceStart - m_hitResult.TraceStart;
        UGameplayStatics::ApplyPointDamage(DamagedActor, DamageAmount, HitFromDirection, m_hitResult, GetController(), this, DamageType);
    }
   // DrawDebugSphere(GetWorld(), GetActorLocation(), DamageRadius, 24, FColor::Red, false, 5.0f);
    WeaponFXComponent->PlayImpactFX(Hit);
    if (FireSound)
        UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
    Destroy();
}

 AController* ASTUProjecttile::GetController() const 
 {
 
    const auto Pawn = Cast<APawn>(GetOwner());
     return Pawn ? Pawn->GetController() : nullptr;
 
 }