// Shoot Them Up Game. All rights reserved.

#include "Weapon/STULauncherWeapon.h"
#include "Weapon/STUProjecttile.h"
#include "Kismet/GameplayStatics.h"

void ASTULauncherWeapon::StartFire()
{
    MakeShot();
}

void ASTULauncherWeapon::MakeShot()
{
    Super::MakeShot();
    if (!GetWorld() ||IsAmmoEmpty())
        return;

    FVector TraceStart, TraceEnd;
    if (!GetTraceData(TraceStart, TraceEnd))
        return;
    FHitResult HitResult;
    MakeHit(HitResult, TraceStart, TraceEnd);

    const FVector EndPoint = HitResult.bBlockingHit ? HitResult.ImpactPoint : TraceEnd;
    const FVector Direction = (EndPoint - GetMuzzleWorldLocation()).GetSafeNormal();

    const FTransform SpawnTransform(FRotator::ZeroRotator, GetMuzzleWorldLocation());
    ASTUProjecttile* Projecttile = GetWorld()->SpawnActorDeferred<ASTUProjecttile>(ProjecttileClass, SpawnTransform);
    if (Projecttile)
    {
        Projecttile->SetShotDirection(Direction);
        Projecttile->SetOwner(GetOwner());
        Projecttile->FinishSpawning(SpawnTransform);

    }
    DecreaseAmmo();
    if (FireSound)
        UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
    SpawnMuzzleFX();
}