// ========================================
// Updated DBW_MainMenu.cpp - Simplified Login Only
// ========================================
#include "DBW_MainMenu.h"
#include "Components/EditableTextBox.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/CheckBox.h"
#include "Account/DBAuthSubsystem.h"
#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"

void UDBW_MainMenu::NativeConstruct()
{
    Super::NativeConstruct();

    // Bind header button
    if (HeaderPlayNowButton)
        HeaderPlayNowButton->OnClicked.AddDynamic(this, &UDBW_MainMenu::OnHeaderPlayNowClicked);

    // Bind login form buttons
    if (LoginButton)
        LoginButton->OnClicked.AddDynamic(this, &UDBW_MainMenu::OnLoginClicked);

    if (CreateAccountButton)
        CreateAccountButton->OnClicked.AddDynamic(this, &UDBW_MainMenu::OnCreateAccountClicked);

    if (ForgotPasswordButton)
        ForgotPasswordButton->OnClicked.AddDynamic(this, &UDBW_MainMenu::OnForgotPasswordClicked);

    // Bind to auth subsystem
    if (UGameInstance* GI = GetGameInstance())
    {
        if (UDBAuthSubsystem* AuthSubsystem = GI->GetSubsystem<UDBAuthSubsystem>())
        {
            AuthSubsystem->OnAuthSuccess.AddDynamic(this, &UDBW_MainMenu::HandleAuthSuccess);
            AuthSubsystem->OnAuthError.AddDynamic(this, &UDBW_MainMenu::HandleAuthError);

            // Check if already logged in
            if (AuthSubsystem->IsLoggedIn())
            {
                ShowLoggedInState();
            }
        }
    }
}

void UDBW_MainMenu::NativeDestruct()
{
    // Unbind from auth subsystem
    if (UGameInstance* GI = GetGameInstance())
    {
        if (UDBAuthSubsystem* AuthSubsystem = GI->GetSubsystem<UDBAuthSubsystem>())
        {
            AuthSubsystem->OnAuthSuccess.RemoveAll(this);
            AuthSubsystem->OnAuthError.RemoveAll(this);
        }
    }

    Super::NativeDestruct();
}

void UDBW_MainMenu::OnHeaderPlayNowClicked()
{
    if (bIsLoggedIn)
    {
        // User is logged in - enter the game
        if (UGameInstance* GI = GetGameInstance())
        {
            if (UDBAuthSubsystem* AuthSubsystem = GI->GetSubsystem<UDBAuthSubsystem>())
            {
                if (AuthSubsystem->IsLoggedIn())
                {
                    UE_LOG(LogTemp, Warning, TEXT("Entering realm..."));
                    UGameplayStatics::OpenLevel(this, TEXT("Lvl_ThirdPerson"));
                    return;
                }
            }
        }
    }
    
    // User not logged in - just show message to encourage login
    ShowStatusMessage(TEXT("Please log in to enter the realm"), false);
}

void UDBW_MainMenu::OnLoginClicked()
{
    if (!ValidateLoginInputs())
        return;

    if (UGameInstance* GI = GetGameInstance())
    {
        if (UDBAuthSubsystem* AuthSubsystem = GI->GetSubsystem<UDBAuthSubsystem>())
        {
            SetLoginUIEnabled(false);
            ShowStatusMessage(TEXT("Logging in..."), false);

            FString Username = UsernameBox->GetText().ToString().TrimStartAndEnd();
            FString Password = PasswordBox->GetText().ToString();

            AuthSubsystem->Login(Username, Password);
        }
    }
}

void UDBW_MainMenu::OnCreateAccountClicked()
{
    UE_LOG(LogTemp, Warning, TEXT("Opening registration website..."));
    
    // Open registration website in default browser
    FString RegistrationURL = TEXT("http://localhost:3001/register");
    FPlatformProcess::LaunchURL(*RegistrationURL, nullptr, nullptr);
    
    ShowStatusMessage(TEXT("Registration page opened in browser"), false);
}

void UDBW_MainMenu::OnForgotPasswordClicked()
{
    UE_LOG(LogTemp, Warning, TEXT("Opening password reset website..."));
    
    // Open password reset website in default browser
    FString ResetURL = TEXT("http://localhost:3001/forgot-password");
    FPlatformProcess::LaunchURL(*ResetURL, nullptr, nullptr);
    
    ShowStatusMessage(TEXT("Password reset page opened in browser"), false);
}

void UDBW_MainMenu::HandleAuthSuccess(const FString& Message)
{
    UE_LOG(LogTemp, Warning, TEXT("Auth success: %s"), *Message);
    ShowLoggedInState();
}

void UDBW_MainMenu::HandleAuthError(const FString& Error)
{
    UE_LOG(LogTemp, Error, TEXT("Auth error: %s"), *Error);
    SetLoginUIEnabled(true);
    ShowStatusMessage(Error, true);
}

void UDBW_MainMenu::ShowLoggedInState()
{
    bIsLoggedIn = true;

    // Hide login form elements
    if (UsernameBox) UsernameBox->SetVisibility(ESlateVisibility::Hidden);
    if (PasswordBox) PasswordBox->SetVisibility(ESlateVisibility::Hidden);
    if (LoginButton) LoginButton->SetVisibility(ESlateVisibility::Hidden);
    if (CreateAccountButton) CreateAccountButton->SetVisibility(ESlateVisibility::Hidden);
    if (ForgotPasswordButton) ForgotPasswordButton->SetVisibility(ESlateVisibility::Hidden);
    if (RememberPasswordCheck) RememberPasswordCheck->SetVisibility(ESlateVisibility::Hidden);

    // Show welcome message
    if (UGameInstance* GI = GetGameInstance())
    {
        if (UDBAuthSubsystem* AuthSubsystem = GI->GetSubsystem<UDBAuthSubsystem>())
        {
            FPlayerData PlayerData = AuthSubsystem->GetPlayerData();
            FString WelcomeMessage = FString::Printf(TEXT("Welcome back, %s! Click Play Now to enter the realm."), *PlayerData.Username);
            ShowStatusMessage(WelcomeMessage, false);
        }
    }

    // Enable header button (styling should be done in Blueprint)
    if (HeaderPlayNowButton)
    {
        HeaderPlayNowButton->SetIsEnabled(true);
    }
}

void UDBW_MainMenu::ResetToLoginState()
{
    bIsLoggedIn = false;

    // Show login form elements
    if (UsernameBox) UsernameBox->SetVisibility(ESlateVisibility::Visible);
    if (PasswordBox) PasswordBox->SetVisibility(ESlateVisibility::Visible);
    if (LoginButton) LoginButton->SetVisibility(ESlateVisibility::Visible);
    if (CreateAccountButton) CreateAccountButton->SetVisibility(ESlateVisibility::Visible);
    if (ForgotPasswordButton) ForgotPasswordButton->SetVisibility(ESlateVisibility::Visible);
    if (RememberPasswordCheck) RememberPasswordCheck->SetVisibility(ESlateVisibility::Visible);

    // Clear status and inputs
    ClearAllInputs();
    ShowStatusMessage(TEXT("Enter your credentials"), false);

    // Disable header button styling (done in Blueprint)
    if (HeaderPlayNowButton)
    {
        HeaderPlayNowButton->SetIsEnabled(false);
    }
}

void UDBW_MainMenu::SetLoginUIEnabled(bool bEnabled)
{
    if (UsernameBox) UsernameBox->SetIsEnabled(bEnabled);
    if (PasswordBox) PasswordBox->SetIsEnabled(bEnabled);
    if (LoginButton) LoginButton->SetIsEnabled(bEnabled);
    if (CreateAccountButton) CreateAccountButton->SetIsEnabled(bEnabled);
    if (ForgotPasswordButton) ForgotPasswordButton->SetIsEnabled(bEnabled);
    if (RememberPasswordCheck) RememberPasswordCheck->SetIsEnabled(bEnabled);
}

bool UDBW_MainMenu::ValidateLoginInputs()
{
    if (!UsernameBox || !PasswordBox)
    {
        ShowStatusMessage(TEXT("UI Error: Missing input fields"), true);
        return false;
    }

    FString Username = UsernameBox->GetText().ToString().TrimStartAndEnd();
    FString Password = PasswordBox->GetText().ToString();

    if (Username.IsEmpty())
    {
        ShowStatusMessage(TEXT("Please enter your username"), true);
        return false;
    }

    if (Password.IsEmpty())
    {
        ShowStatusMessage(TEXT("Please enter your password"), true);
        return false;
    }

    if (Password.Len() < 3)
    {
        ShowStatusMessage(TEXT("Password must be at least 3 characters"), true);
        return false;
    }

    return true;
}

void UDBW_MainMenu::ClearAllInputs()
{
    if (UsernameBox) UsernameBox->SetText(FText::GetEmpty());
    if (PasswordBox) PasswordBox->SetText(FText::GetEmpty());
    if (RememberPasswordCheck) RememberPasswordCheck->SetIsChecked(false);
}

void UDBW_MainMenu::ShowStatusMessage(const FString& Message, bool bIsError)
{
    if (StatusText)
    {
        StatusText->SetText(FText::FromString(Message));
        StatusText->SetColorAndOpacity(bIsError ? FLinearColor::Red : FLinearColor::White);
    }
    
    UE_LOG(LogTemp, Log, TEXT("Status: %s"), *Message);
}