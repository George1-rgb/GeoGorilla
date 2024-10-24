// Shoot Them Up Game. All rights reserved.

#include "STUGameModeBase.h"
#include "GameFramework/PlayerStart.h"
#include "Player/STUBaseCharacter.h"
#include "Player/STUPlayerController.h"
#include "UI/STUGameHUD.h"
#include "AIController.h"
#include "Player/STUPlayerState.h"
#include "STUUtils.h"
#include "Components/STURespawnComponent.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerStart/STUStart.h"
#include "STUGameInstance.h"


constexpr static int32 MinRoundTimeForRespawn = 10;

ASTUGameModeBase::ASTUGameModeBase() {
    DefaultPawnClass = ASTUBaseCharacter::StaticClass();
    PlayerControllerClass = ASTUPlayerController::StaticClass();
    HUDClass = ASTUGameHUD::StaticClass();
    PlayerStateClass = ASTUPlayerState::StaticClass();
}

void ASTUGameModeBase::StartPlay()
{
    Super::StartPlay();
  //  LoadLevel();
    TArray<AActor*> tmp;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), tmp);
    for (auto& nId : m_vCommandIds)
    {
        Starts.Add(nId, tmp[nId]);
    }
    for (auto& nId: m_vCommandIds)
    {
		SpawnBots();
    }
	CreateTeamsInfo();
    ResetPlayers();
    CurrentRound = 1;
    StartRound();
    SetMatchState(ESTUMatchState::InProgress);
}

void ASTUGameModeBase::LoadLevel()
{
    SetMatchState(ESTUMatchState::Loading);
    FLatentActionInfo LatentInfo;
    for (auto LevelName : LevelNames)
    {
        UGameplayStatics::LoadStreamLevel(GetWorld(), LevelName, true, true, LatentInfo);
    }
}

void ASTUGameModeBase::SpawnBots()
{
    if (!GetWorld()) return;
    for (int32 i = 0; i < GameData.PlayersNum - 1; ++i)
    {
       FActorSpawnParameters SpawnInfo;
       SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
       const auto STUAIController = GetWorld()->SpawnActor<AAIController>(AIControllerClass, SpawnInfo);
    }
}

UClass* ASTUGameModeBase::GetDefaultPawnClassForController_Implementation(AController* InController)
{
    if (InController && InController->IsA<AAIController>())
    {
        return AIPawnClass;
    }
    return Super::GetDefaultPawnClassForController_Implementation(InController);
}

void ASTUGameModeBase::StartRound()
{
    RoundCountDown = GameData.RoundTime;
    GetWorldTimerManager().SetTimer(GameRoundTimerHandle, this, &ASTUGameModeBase::GameTimerUpdate, 1.0f, true);
}
void ASTUGameModeBase::GameTimerUpdate()
{
    if (--RoundCountDown == 0)
    {
        GetWorldTimerManager().ClearTimer(GameRoundTimerHandle);
        if (CurrentRound + 1 <= GameData.RoundNum)
        {
            ++CurrentRound;
            ResetPlayers();
            StartRound();
        }
        else
        {
            GameOver();
        }
    }
}

void  ASTUGameModeBase::ResetPlayers()
{
    if (!GetWorld()) return;
    for (auto It = GetWorld()->GetControllerIterator(); It; ++It)
    {
        ResetOnePlayer(It->Get());
    }
}

void  ASTUGameModeBase::ResetOnePlayer(AController* Controller)
{
    if (Controller && Controller->GetPawn())
    {
        Controller->GetPawn()->Reset();
    }

    const auto PlayerState = Cast<ASTUPlayerState>(Controller->PlayerState);
    if (!PlayerState) return;

    for (auto& nId : m_vCommandIds)
    {
        if(PlayerState->GetTeamID() != nId)
            continue;
        const auto PlayerStart = Cast<APlayerStart>(Starts[nId]);
    
        if (PlayerStart)
        {
            RestartPlayerAtPlayerStart(Controller, PlayerStart);
        }
    }

    /*for (auto Start : Starts)
    {
        const auto PlayerStart = Cast<APlayerStart>(Start);

        if (PlayerState->GetTeamID() == 1 && PlayerStart->PlayerStartTag == FName("BLUE"))
            RestartPlayerAtPlayerStart(Controller, PlayerStart);
        else if (PlayerState->GetTeamID() == 2 && PlayerStart->PlayerStartTag == FName("RED"))
            RestartPlayerAtPlayerStart(Controller, PlayerStart);
    }*/

    SetPlayerColor(Controller);
}

void ASTUGameModeBase::CreateTeamsInfo()
{
    if (!GetWorld()) return;
    int64 nBotId = 1;
    int64 nTeamsCount = m_vCommandIds.Num();
    int64 nTeamId = 0;
    for (auto It = GetWorld()->GetControllerIterator(); It; ++It)
    {
        const auto Controller = It->Get();
        if (!Controller) continue;
        const auto PlayerState = Cast<ASTUPlayerState>(Controller->PlayerState);
        if (!PlayerState) continue;

        PlayerState->SetTeamID(nTeamId);
        PlayerState->SetTeamColor(DetermineColorByTeamID(nTeamId));
        PlayerState->SetPlayerName(Controller->IsPlayerController() ? "Player" : "Bot" + FString::FromInt(nBotId));
        nBotId++;
        SetPlayerColor(Controller);
        nTeamId++;
        if (nTeamId >= nTeamsCount)
            nTeamId = 0;
    }

}

FLinearColor ASTUGameModeBase::DetermineColorByTeamID(int32 TeamID) const
{
    if (TeamID < GameData.TeamColors.Num())
    {
        return GameData.TeamColors[TeamID];
    }
       return GameData.DefaultTeamColor;
}

void ASTUGameModeBase::SetPlayerColor(AController* Controller)
{
    if (!Controller) return;

    const auto Character = Cast<ASTUBaseCharacter>(Controller->GetPawn());
    if (!Character) return;

    const auto PlayerState = Cast<ASTUPlayerState>(Controller->PlayerState);
    if (!PlayerState) return;

    Character->SetPlayerColor(PlayerState->GetTeamColor());
}

void ASTUGameModeBase::Killed(AController* KillerController, AController* VictimController)
{
    ASTUPlayerState* KillerPlayerState = nullptr;
    ASTUPlayerState* VictimPlayerState = nullptr;
    if (KillerController)
    {
        KillerPlayerState = Cast<ASTUPlayerState>(KillerController->PlayerState);
        if (KillerPlayerState)
            KillerPlayerState->AddKill();
    }
    if (VictimController)
    {
        VictimPlayerState = Cast<ASTUPlayerState>(VictimController->PlayerState);
        if (VictimPlayerState)
            VictimPlayerState->AddDeath();
        StartRespawn(VictimController);
    }
}

void ASTUGameModeBase::LogPlayerInfo()
{
    if (!GetWorld()) return;
    for (auto It = GetWorld()->GetControllerIterator(); It; ++It)
    {
        const auto Controller = It->Get();
        if (!Controller) continue;
        const auto PlayerState = Cast<ASTUPlayerState>(Controller->PlayerState);
        if (!PlayerState) continue;
        PlayerState->LogInfo();
    }
}

void ASTUGameModeBase::StartRespawn(AController* Controller)
{
    const auto RespawnAviable = RoundCountDown > MinRoundTimeForRespawn + GameData.RespawnTime;
    if (!RespawnAviable) return;
    const auto RespawnComponent = STUUtils::GetSTUPlayerComponent<USTURespawnComponent>(Controller);
    if (!RespawnComponent) return;

    RespawnComponent->Respawn(GameData.RespawnTime);
}

void ASTUGameModeBase::RespawnRequest(AController* Controller)
{
    ResetOnePlayer(Controller);
}

void ASTUGameModeBase::GameOver()
{
         LogPlayerInfo();

    for (auto Pawn : TActorRange<APawn>(GetWorld()))
    {
        if (Pawn)
        {
            Pawn->TurnOff();
            Pawn->DisableInput(nullptr);
        }
    }
    SetMatchState(ESTUMatchState::GameOver);
}

void ASTUGameModeBase::SetMatchState(ESTUMatchState State)
{
    if (MatchState == State) return;
    MatchState = State;
    OnMatchStateChanged.Broadcast(MatchState);
}

bool ASTUGameModeBase::SetPause(APlayerController* PC, FCanUnpause CanUnpauseDelegate)
{
    const auto PauseSet = Super::SetPause(PC, CanUnpauseDelegate);

    if (PauseSet)
    {
        SetMatchState(ESTUMatchState::Pause);
    } 
    return PauseSet;
}
bool ASTUGameModeBase::ClearPause()
{
    const auto PauseClear = Super::ClearPause();

    if (PauseClear)
    {
        SetMatchState(ESTUMatchState::InProgress);
    }

    return PauseClear;
}