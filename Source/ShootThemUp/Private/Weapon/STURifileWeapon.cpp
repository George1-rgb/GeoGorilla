// Shoot Them Up Game. All rights reserved.

#include "Weapon/STURifileWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Weapon/Components/STUWeaponFXComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Camera/CameraShakeBase.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY_STATIC(LogRifileWeapon, All, All);

ASTURifileWeapon::ASTURifileWeapon() 
{
    WeaponFXComponent = CreateDefaultSubobject<USTUWeaponFXComponent>("WeaponFXComponent");
}

void ASTURifileWeapon::BeginPlay() 
{
    Super::BeginPlay();
    check(WeaponFXComponent);
}

void ASTURifileWeapon::StartFire()
{
    Super::StartFire();
    InitMuzzleFX();
    GetWorldTimerManager().SetTimer(ShotTimerHandle, this, &ASTURifileWeapon::MakeShot, TimerBetweenShots, true);
    MakeShot();
    
}

void ASTURifileWeapon::StopFire()
{
    Super::StopFire();
    GetWorldTimerManager().ClearTimer(ShotTimerHandle);
    SetMuzzleFXVisibility(false);
}

void ASTURifileWeapon::MakeShot()
{
    Super::MakeShot();
    if (!GetWorld() || IsAmmoEmpty())
    {
        StopFire();
        return;
    }

    FVector TraceStart, TraceEnd;
    if (!GetTraceData(TraceStart, TraceEnd))
    {
        StopFire();
        return;
    }
    FHitResult HitResult;
    MakeHit(HitResult, TraceStart, TraceEnd);
    PlayCameraShake();
    FVector TraceFXEnd = TraceEnd;

    if (HitResult.bBlockingHit)
    {
        TraceEnd = HitResult.ImpactPoint;
        MakeDamage(HitResult);
        WeaponFXComponent->PlayImpactFX(HitResult);
    }
    SpawnTraceFX(GetMuzzleWorldLocation(), TraceFXEnd);
    if (FireSound)
        UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
    DecreaseAmmo();
}

bool ASTURifileWeapon::GetTraceData(FVector& TraceStart, FVector& TraceEnd) const
{

    FVector ViewLocation;
    FRotator ViewRotation;
    if (!GetPlayerViewPoint(ViewLocation, ViewRotation))
        return false;

    TraceStart = ViewLocation;
    const auto HalfRad = FMath::DegreesToRadians(BulletSpread);
    const FVector ShootDirection = FMath::VRandCone(ViewRotation.Vector(), HalfRad); // SocketTransform.GetRotation().GetForwardVector();
    TraceEnd = TraceStart + ShootDirection * TraceMaxDistance;
    return true;
}

void ASTURifileWeapon::MakeDamage(const FHitResult& HitResult)
{
    const auto DamagedActor = HitResult.GetActor();
    FVector HitFromDirection = HitResult.TraceStart - HitResult.TraceStart;
    
    if (!DamagedActor)
        return;
    UGameplayStatics::ApplyPointDamage(DamagedActor, DamageAmount, HitFromDirection, HitResult, GetController(), this, DamageType);
}

void ASTURifileWeapon::InitMuzzleFX() 
{
    if (!MuzzleFXComponent)
    {
        MuzzleFXComponent = SpawnMuzzleFX();
    }
    SetMuzzleFXVisibility(true);
}
void ASTURifileWeapon::SetMuzzleFXVisibility(bool Visible) 
{
    if (MuzzleFXComponent)
    {
        MuzzleFXComponent->SetPaused(!Visible);
        MuzzleFXComponent->SetVisibility(Visible, true);
    }
}

 void ASTURifileWeapon::SpawnTraceFX(const FVector& TraceStart, const FVector& TraceEnd) 
 {
     const auto TraceFXComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), TraceFX, TraceStart);
     if (TraceFXComponent)
     {
         TraceFXComponent->SetNiagaraVariableVec3(TraceTargetName, TraceEnd);
     }
 }

 void ASTURifileWeapon::PlayCameraShake()
 {
     const auto Player = Cast<APawn>(GetOwner());
     if (!Player)
         return;

     const auto Controller = Player->GetController<APlayerController>();
     if (!Controller || !Controller->PlayerCameraManager)
         return;

     Controller->PlayerCameraManager->StartCameraShake(CameraShake);
 }

 AController* ASTURifileWeapon::GetController() const
 {

     const auto Pawn = Cast<APawn>(GetOwner());
     return Pawn ? Pawn->GetController() : nullptr;

 }