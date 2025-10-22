// ========================================
// Updated DBW_MainMenu.h - Simplified Login Only
// ========================================
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DBW_MainMenu.generated.h"

class UEditableTextBox;
class UButton;
class UTextBlock;
class UCheckBox;

UCLASS()
class DAWNBLADE_API UDBW_MainMenu : public UUserWidget
{
    GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    // Header widgets
    UPROPERTY(meta = (BindWidget))
    UButton* HeaderPlayNowButton;

    // Login form widgets
    UPROPERTY(meta = (BindWidget))
    UEditableTextBox* UsernameBox;

    UPROPERTY(meta = (BindWidget))
    UEditableTextBox* PasswordBox;

    UPROPERTY(meta = (BindWidget))
    UButton* LoginButton;

    UPROPERTY(meta = (BindWidget))
    UButton* CreateAccountButton;

    UPROPERTY(meta = (BindWidget))
    UButton* ForgotPasswordButton;

    UPROPERTY(meta = (BindWidget))
    UCheckBox* RememberPasswordCheck;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* StatusText;

public:
    UFUNCTION(BlueprintCallable)
    void ShowLoggedInState();

    UFUNCTION(BlueprintCallable)
    void ResetToLoginState();

private:
    // Button event handlers
    UFUNCTION()
    void OnHeaderPlayNowClicked();

    UFUNCTION()
    void OnLoginClicked();

    UFUNCTION()
    void OnCreateAccountClicked();

    UFUNCTION()
    void OnForgotPasswordClicked();

    // Auth system responses
    UFUNCTION()
    void HandleAuthSuccess(const FString& Message);

    UFUNCTION()
    void HandleAuthError(const FString& Error);

    // Utility functions
    void SetLoginUIEnabled(bool bEnabled);
    bool ValidateLoginInputs();
    void ClearAllInputs();
    void ShowStatusMessage(const FString& Message, bool bIsError);

    // Track login state
    bool bIsLoggedIn = false;
};