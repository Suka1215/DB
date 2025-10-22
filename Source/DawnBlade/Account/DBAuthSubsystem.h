#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/IHttpRequest.h"
#include "Types/DBTypes.h"
#include "DBAuthSubsystem.generated.h"

USTRUCT(BlueprintType)
struct DAWNBLADE_API FPlayerData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FString PlayerId;

    UPROPERTY(BlueprintReadOnly)
    FString Username;

    UPROPERTY(BlueprintReadOnly)
    FString Email;

    FPlayerData()
    {
        PlayerId = TEXT("");
        Username = TEXT("");
        Email = TEXT("");
    }
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAuthSuccess, const FString&, Message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAuthError, const FString&, ErrorMessage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharactersLoaded, const TArray<FCharacterData>&, Characters);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterCreated, const FCharacterData&, NewCharacter);

UCLASS()
class DAWNBLADE_API UDBAuthSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    // Auth functions - REGISTER REMOVED
    UFUNCTION(BlueprintCallable)
    void Login(const FString& UsernameOrEmail, const FString& Password);

    UFUNCTION(BlueprintCallable)
    void Logout();

    // Character functions
    UFUNCTION(BlueprintCallable)
    void LoadCharacters();

    UFUNCTION(BlueprintCallable)
    void CreateCharacter(const FString& CharacterName, int32 RaceId, int32 JobId);

    UFUNCTION(BlueprintCallable)
    void LoadRaces();

    UFUNCTION(BlueprintCallable)
    void LoadJobs();

    // Getters
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool IsLoggedIn() const { return !SessionToken.IsEmpty(); }

    UFUNCTION(BlueprintCallable, BlueprintPure)
    FString GetAuthHeader() const { return FString::Printf(TEXT("Bearer %s"), *SessionToken); }

    UFUNCTION(BlueprintCallable, BlueprintPure)
    FPlayerData GetPlayerData() const { return CurrentPlayer; }

    UFUNCTION(BlueprintCallable, BlueprintPure)
    TArray<FCharacterData> GetCachedCharacters() const { return CachedCharacters; }

    UFUNCTION(BlueprintCallable, BlueprintPure)
    TArray<FRaceData> GetCachedRaces() const { return CachedRaces; }

    UFUNCTION(BlueprintCallable, BlueprintPure)
    TArray<FJobData> GetCachedJobs() const { return CachedJobs; }

    // Delegates
    UPROPERTY(BlueprintAssignable)
    FOnAuthSuccess OnAuthSuccess;

    UPROPERTY(BlueprintAssignable)
    FOnAuthError OnAuthError;

    UPROPERTY(BlueprintAssignable)
    FOnCharactersLoaded OnCharactersLoaded;

    UPROPERTY(BlueprintAssignable)
    FOnCharacterCreated OnCharacterCreated;

private:
    // Auth data
    FString SessionToken;
    FPlayerData CurrentPlayer;
    
    // Character data
    TArray<FCharacterData> CachedCharacters;
    TArray<FRaceData> CachedRaces;
    TArray<FJobData> CachedJobs;
    
    bool bIsLoading = false;

    // Response handlers - REGISTER HANDLER REMOVED
    void HandleLoginResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
    void HandleCharactersResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
    void HandleCreateCharacterResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
    void HandleRacesResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
    void HandleJobsResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
    
    void ParseErrorResponse(const FString& ResponseContent, FString& OutErrorMessage);
};