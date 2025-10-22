#include "DBAuthSubsystem.h"

#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Json.h"
#include "JsonUtilities.h"
#include "Engine/Engine.h"
#include "Types/FCharacterData.h"
#include "Types/FJobData.h"

static const FString BaseUrl = TEXT("http://localhost:3000/api");

void UDBAuthSubsystem::Login(const FString& UsernameOrEmail, const FString& Password)
{
    if (bIsLoading)
    {
        OnAuthError.Broadcast(TEXT("Request already in progress"));
        return;
    }

    // Input validation
    if (UsernameOrEmail.IsEmpty() || Password.IsEmpty())
    {
        OnAuthError.Broadcast(TEXT("Username and password are required"));
        return;
    }

    bIsLoading = true;

    TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(BaseUrl + TEXT("/auth/login"));
    Request->SetVerb(TEXT("POST"));
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

    // Create request body
    TSharedPtr<FJsonObject> RequestBody = MakeShareable(new FJsonObject);
    RequestBody->SetStringField(TEXT("username"), UsernameOrEmail); // API accepts username OR email
    RequestBody->SetStringField(TEXT("password"), Password);

    FString RequestContent;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestContent);
    FJsonSerializer::Serialize(RequestBody.ToSharedRef(), Writer);

    Request->SetContentAsString(RequestContent);
    Request->OnProcessRequestComplete().BindUObject(this, &UDBAuthSubsystem::HandleLoginResponse);
    Request->ProcessRequest();
}

void UDBAuthSubsystem::Logout()
{
    // Clear session data
    SessionToken.Empty();
    CurrentPlayer = FPlayerData();
    CachedCharacters.Empty();
    CachedRaces.Empty();
    CachedJobs.Empty();
    bIsLoading = false;

    // Optionally call logout endpoint to invalidate server-side session
    if (!SessionToken.IsEmpty())
    {
        TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
        Request->SetURL(BaseUrl + TEXT("/auth/logout"));
        Request->SetVerb(TEXT("POST"));
        Request->SetHeader(TEXT("Authorization"), GetAuthHeader());
        Request->ProcessRequest();
    }

    OnAuthSuccess.Broadcast(TEXT("Logged out successfully"));
}

void UDBAuthSubsystem::LoadCharacters()
{
    if (!IsLoggedIn())
    {
        OnAuthError.Broadcast(TEXT("Not logged in"));
        return;
    }

    if (bIsLoading)
    {
        OnAuthError.Broadcast(TEXT("Request already in progress"));
        return;
    }

    bIsLoading = true;

    TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(BaseUrl + TEXT("/characters"));
    Request->SetVerb(TEXT("GET"));
    Request->SetHeader(TEXT("Authorization"), GetAuthHeader());

    Request->OnProcessRequestComplete().BindUObject(this, &UDBAuthSubsystem::HandleCharactersResponse);
    Request->ProcessRequest();
}

void UDBAuthSubsystem::CreateCharacter(const FString& CharacterName, int32 RaceId, int32 JobId)
{
    if (!IsLoggedIn())
    {
        OnAuthError.Broadcast(TEXT("Not logged in"));
        return;
    }

    if (bIsLoading)
    {
        OnAuthError.Broadcast(TEXT("Request already in progress"));
        return;
    }

    if (CharacterName.IsEmpty())
    {
        OnAuthError.Broadcast(TEXT("Character name is required"));
        return;
    }

    bIsLoading = true;

    TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(BaseUrl + TEXT("/characters"));
    Request->SetVerb(TEXT("POST"));
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    Request->SetHeader(TEXT("Authorization"), GetAuthHeader());

    // Create request body
    TSharedPtr<FJsonObject> RequestBody = MakeShareable(new FJsonObject);
    RequestBody->SetStringField(TEXT("character_name"), CharacterName);
    RequestBody->SetNumberField(TEXT("race_id"), RaceId);
    RequestBody->SetNumberField(TEXT("main_job_id"), JobId);

    FString RequestContent;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestContent);
    FJsonSerializer::Serialize(RequestBody.ToSharedRef(), Writer);

    Request->SetContentAsString(RequestContent);
    Request->OnProcessRequestComplete().BindUObject(this, &UDBAuthSubsystem::HandleCreateCharacterResponse);
    Request->ProcessRequest();
}

void UDBAuthSubsystem::LoadRaces()
{
    TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(BaseUrl + TEXT("/characters/options/races"));
    Request->SetVerb(TEXT("GET"));

    Request->OnProcessRequestComplete().BindUObject(this, &UDBAuthSubsystem::HandleRacesResponse);
    Request->ProcessRequest();
}

void UDBAuthSubsystem::LoadJobs()
{
    TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(BaseUrl + TEXT("/characters/options/jobs"));
    Request->SetVerb(TEXT("GET"));

    Request->OnProcessRequestComplete().BindUObject(this, &UDBAuthSubsystem::HandleJobsResponse);
    Request->ProcessRequest();
}

void UDBAuthSubsystem::HandleLoginResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    bIsLoading = false;

    if (!bWasSuccessful || !Response.IsValid())
    {
        OnAuthError.Broadcast(TEXT("Connection failed. Please check your internet connection."));
        return;
    }

    const int32 ResponseCode = Response->GetResponseCode();
    const FString ResponseContent = Response->GetContentAsString();

    if (ResponseCode == 200)
    {
        // Parse successful login response
        TSharedPtr<FJsonObject> JsonResponse;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseContent);
        
        if (FJsonSerializer::Deserialize(Reader, JsonResponse) && JsonResponse.IsValid())
        {
            // Extract token
            SessionToken = JsonResponse->GetStringField(TEXT("token"));
            
            // Extract player data
            TSharedPtr<FJsonObject> PlayerObject = JsonResponse->GetObjectField(TEXT("player"));
            if (PlayerObject.IsValid())
            {
                CurrentPlayer.PlayerId = PlayerObject->GetStringField(TEXT("player_id"));
                CurrentPlayer.Username = PlayerObject->GetStringField(TEXT("username"));
                CurrentPlayer.Email = PlayerObject->GetStringField(TEXT("email"));
            }

            OnAuthSuccess.Broadcast(FString::Printf(TEXT("Welcome back, %s!"), *CurrentPlayer.Username));
        }
        else
        {
            OnAuthError.Broadcast(TEXT("Invalid server response"));
        }
    }
    else
    {
        // Parse error response
        FString ErrorMessage;
        ParseErrorResponse(ResponseContent, ErrorMessage);
        OnAuthError.Broadcast(ErrorMessage);
    }
}

void UDBAuthSubsystem::HandleCharactersResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    bIsLoading = false;

    if (!bWasSuccessful || !Response.IsValid())
    {
        OnAuthError.Broadcast(TEXT("Failed to load characters"));
        return;
    }

    const int32 ResponseCode = Response->GetResponseCode();
    const FString ResponseContent = Response->GetContentAsString();

    if (ResponseCode == 200)
    {
        TSharedPtr<FJsonObject> JsonResponse;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseContent);
        
        if (FJsonSerializer::Deserialize(Reader, JsonResponse) && JsonResponse.IsValid())
        {
            // Clear existing characters
            CachedCharacters.Empty();

            // Parse characters array
            const TArray<TSharedPtr<FJsonValue>>* CharactersArray;
            if (JsonResponse->TryGetArrayField(TEXT("characters"), CharactersArray))
            {
                for (const auto& CharValue : *CharactersArray)
                {
                    const TSharedPtr<FJsonObject>* CharObject;
                    if (CharValue->TryGetObject(CharObject))
                    {
                        FCharacterData CharData;
                        CharData.CharacterId = (*CharObject)->GetStringField(TEXT("character_id"));
                        CharData.Name = (*CharObject)->GetStringField(TEXT("character_name"));
                        CharData.Level = (*CharObject)->GetIntegerField(TEXT("main_job_level"));
                        
                        // Set race data
                        CharData.Race.RaceId = (*CharObject)->GetIntegerField(TEXT("race_id"));
                        CharData.Race.RaceName = (*CharObject)->GetStringField(TEXT("race_name"));
                        CharData.Race.RaceCode = (*CharObject)->GetStringField(TEXT("race_code"));
                        
                        // Set job data
                        CharData.Job.JobId = (*CharObject)->GetIntegerField(TEXT("main_job_id"));
                        CharData.Job.JobName = (*CharObject)->GetStringField(TEXT("job_name"));
                        CharData.Job.JobCode = (*CharObject)->GetStringField(TEXT("job_code"));
                        
                        // Set default HP/MP values since basic character list doesn't include them
                        CharData.CurrentHP = 100;
                        CharData.MaxHP = 100;
                        CharData.CurrentMP = 50;
                        CharData.MaxMP = 50;

                        CachedCharacters.Add(CharData);
                    }
                }
            }

            OnCharactersLoaded.Broadcast(CachedCharacters);
        }
        else
        {
            OnAuthError.Broadcast(TEXT("Invalid server response"));
        }
    }
    else
    {
        FString ErrorMessage;
        ParseErrorResponse(ResponseContent, ErrorMessage);
        OnAuthError.Broadcast(ErrorMessage);
    }
}

void UDBAuthSubsystem::HandleCreateCharacterResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    bIsLoading = false;

    if (!bWasSuccessful || !Response.IsValid())
    {
        OnAuthError.Broadcast(TEXT("Failed to create character"));
        return;
    }

    const int32 ResponseCode = Response->GetResponseCode();
    const FString ResponseContent = Response->GetContentAsString();

    if (ResponseCode == 201)
    {
        TSharedPtr<FJsonObject> JsonResponse;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseContent);
        
        if (FJsonSerializer::Deserialize(Reader, JsonResponse) && JsonResponse.IsValid())
        {
            // Parse the new character data
            TSharedPtr<FJsonObject> CharObject = JsonResponse->GetObjectField(TEXT("character"));
            if (CharObject.IsValid())
            {
                FCharacterData NewCharacter;
                NewCharacter.CharacterId = CharObject->GetStringField(TEXT("character_id"));
                NewCharacter.Name = CharObject->GetStringField(TEXT("character_name"));
                NewCharacter.Level = CharObject->GetIntegerField(TEXT("main_job_level"));
                
                // Set race data
                NewCharacter.Race.RaceId = CharObject->GetIntegerField(TEXT("race_id"));
                NewCharacter.Race.RaceName = CharObject->GetStringField(TEXT("race_name"));
                NewCharacter.Race.RaceCode = CharObject->GetStringField(TEXT("race_code"));
                
                // Set job data
                NewCharacter.Job.JobId = CharObject->GetIntegerField(TEXT("main_job_id"));
                NewCharacter.Job.JobName = CharObject->GetStringField(TEXT("job_name"));
                NewCharacter.Job.JobCode = CharObject->GetStringField(TEXT("job_code"));
                
                NewCharacter.CurrentHP = CharObject->GetIntegerField(TEXT("max_hp"));
                NewCharacter.MaxHP = CharObject->GetIntegerField(TEXT("max_hp"));
                NewCharacter.CurrentMP = CharObject->GetIntegerField(TEXT("max_mp"));
                NewCharacter.MaxMP = CharObject->GetIntegerField(TEXT("max_mp"));

                // Add to cached characters
                CachedCharacters.Add(NewCharacter);

                OnCharacterCreated.Broadcast(NewCharacter);
            }
        }
        else
        {
            OnAuthError.Broadcast(TEXT("Invalid server response"));
        }
    }
    else
    {
        FString ErrorMessage;
        ParseErrorResponse(ResponseContent, ErrorMessage);
        OnAuthError.Broadcast(ErrorMessage);
    }
}

void UDBAuthSubsystem::HandleRacesResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (!bWasSuccessful || !Response.IsValid() || Response->GetResponseCode() != 200)
    {
        return;
    }

    const FString ResponseContent = Response->GetContentAsString();
    TSharedPtr<FJsonObject> JsonResponse;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseContent);
    
    if (FJsonSerializer::Deserialize(Reader, JsonResponse) && JsonResponse.IsValid())
    {
        CachedRaces.Empty();

        const TArray<TSharedPtr<FJsonValue>>* RacesArray;
        if (JsonResponse->TryGetArrayField(TEXT("races"), RacesArray))
        {
            for (const auto& RaceValue : *RacesArray)
            {
                const TSharedPtr<FJsonObject>* RaceObject;
                if (RaceValue->TryGetObject(RaceObject))
                {
                    FRaceData RaceData;
                    RaceData.RaceId = (*RaceObject)->GetIntegerField(TEXT("race_id"));
                    RaceData.RaceName = (*RaceObject)->GetStringField(TEXT("race_name"));
                    RaceData.RaceCode = (*RaceObject)->GetStringField(TEXT("race_code"));
                    RaceData.Description = (*RaceObject)->GetStringField(TEXT("description"));

                    CachedRaces.Add(RaceData);
                }
            }
        }
    }
}

void UDBAuthSubsystem::HandleJobsResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (!bWasSuccessful || !Response.IsValid() || Response->GetResponseCode() != 200)
    {
        return;
    }

    const FString ResponseContent = Response->GetContentAsString();
    TSharedPtr<FJsonObject> JsonResponse;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseContent);
    
    if (FJsonSerializer::Deserialize(Reader, JsonResponse) && JsonResponse.IsValid())
    {
        CachedJobs.Empty();

        const TArray<TSharedPtr<FJsonValue>>* JobsArray;
        if (JsonResponse->TryGetArrayField(TEXT("jobs"), JobsArray))
        {
            for (const auto& JobValue : *JobsArray)
            {
                const TSharedPtr<FJsonObject>* JobObject;
                if (JobValue->TryGetObject(JobObject))
                {
                    FJobData JobData;
                    JobData.JobId = (*JobObject)->GetIntegerField(TEXT("job_id"));
                    JobData.JobName = (*JobObject)->GetStringField(TEXT("job_name"));
                    JobData.JobCode = (*JobObject)->GetStringField(TEXT("job_code"));
                    JobData.Description = (*JobObject)->GetStringField(TEXT("description"));

                    CachedJobs.Add(JobData);
                }
            }
        }
    }
}

void UDBAuthSubsystem::ParseErrorResponse(const FString& ResponseContent, FString& OutErrorMessage)
{
    TSharedPtr<FJsonObject> ErrorJson;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseContent);
    
    if (FJsonSerializer::Deserialize(Reader, ErrorJson) && ErrorJson.IsValid())
    {
        OutErrorMessage = ErrorJson->GetStringField(TEXT("error"));
        if (OutErrorMessage.IsEmpty())
        {
            OutErrorMessage = TEXT("An unknown error occurred");
        }
    }
    else
    {
        OutErrorMessage = TEXT("Server error occurred");
    }
}