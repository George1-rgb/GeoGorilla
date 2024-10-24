// Fill out your copyright notice in the Description page of Project Settings.


#include "AnyDamageActors/BarrelBoom.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/STUHealthComponent.h"
#include "Sound/SoundBase.h"
#include "Weapon/Components/STUWeaponFXComponent.h"
#include "NiagaraFunctionLibrary.h"
// Sets default values
ABarrelBoom::ABarrelBoom()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

    m_pBarrelMesh = CreateDefaultSubobject<UStaticMeshComponent>("BarrelComponent");
	SetRootComponent(m_pBarrelMesh);

    HealthComponent = CreateDefaultSubobject<USTUHealthComponent>("HealthComponent");
}

// Called when the game starts or when spawned
void ABarrelBoom::BeginPlay()
{
	Super::BeginPlay();
    check(m_pBarrelMesh);
    check(HealthComponent);

    OnHealthChanged(HealthComponent->GetHealth(), 0.0f);
    HealthComponent->OnDeath.AddUObject(this, &ABarrelBoom::OnDeath);
    HealthComponent->OnHealthChanged.AddUObject(this, &ABarrelBoom::OnHealthChanged);
}

void ABarrelBoom::OnDeath()
{
    UGameplayStatics::ApplyRadialDamage(GetWorld(), //
        DamageAmount,                               //
        GetActorLocation(),                         //
        DamageRadius,                               //
        DamageType,                 //
        {},                                         //
        this,                                       //
        GetController(),                            //
        true);

    UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), DefaultImpactData.NiagaraEffect, GetActorLocation(), GetActorRotation());
    if (FireSound)
       UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
    Destroy();
}

void ABarrelBoom::OnHealthChanged(float Health, float HealthDelta)
{
    
}

AController* ABarrelBoom::GetController() const
{

    const auto Pawn = Cast<APawn>(GetOwner());
    return Pawn ? Pawn->GetController() : nullptr;

}