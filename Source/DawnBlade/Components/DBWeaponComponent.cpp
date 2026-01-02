// DBWeaponComponent.cpp
#include "Components/DBWeaponComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "DBEquipmentComponent.h"
#include "DBJobComponent.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "TimerManager.h"

UDBWeaponComponent::UDBWeaponComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    WeaponState = EWeaponState::Unarmed;
}

void UDBWeaponComponent::BeginPlay()
{
    Super::BeginPlay();

    // Start unarmed
    if (DefaultUnarmedData)
    {
        CurrentWeaponData = DefaultUnarmedData;
        UE_LOG(LogTemp, Warning, TEXT("✅ Weapon Component initialized - Unarmed"));
    }
}

bool UDBWeaponComponent::CanEquipWeapon(UDBWeaponAnimationSet* WeaponData, FString& OutFailureReason) const
{
    if (!WeaponData)
    {
        OutFailureReason = "Invalid weapon data";
        return false;
    }

    ACharacter* Character = Cast<ACharacter>(GetOwner());
    if (!Character) return false;

    // Check job requirement
    UDBJobComponent* JobComp = Character->FindComponentByClass<UDBJobComponent>();
    if (JobComp)
    {
        if (!JobComp->CanUseWeaponType(WeaponData))
        {
            OutFailureReason = FString::Printf(TEXT("Your job cannot use this weapon type"));
            return false;
        }

        // Check level requirement
        if (JobComp->GetLevel() < WeaponData->RequiredLevel)
        {
            OutFailureReason = FString::Printf(TEXT("Requires level %d"), WeaponData->RequiredLevel);
            return false;
        }
    }

    return true;
}

void UDBWeaponComponent::EquipWeapon(UDBWeaponAnimationSet* WeaponData)
{
    if (!WeaponData)
    {
        UE_LOG(LogTemp, Error, TEXT("❌ EquipWeapon: WeaponData is null"));
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("⚔️ Equipping weapon: %s"), *WeaponData->WeaponTypeName.ToString());

    CurrentWeaponData = WeaponData;

    // Start in sheathed state - DON'T switch ABP yet!
    SetWeaponState(EWeaponState::Sheathed);
    
    // Equipment Component will handle visual mesh spawning
    // ABP will switch when player draws weapon (presses L)
    
    UE_LOG(LogTemp, Warning, TEXT("✅ Weapon equipped (sheathed) - Press L to draw"));
}

void UDBWeaponComponent::UnequipWeapon()
{
    UE_LOG(LogTemp, Warning, TEXT("⚔️ Unequipping weapon"));

    CurrentWeaponData = nullptr;
    
    // Switch back to unarmed/generic ABP
    if (UDBJobComponent* JobComp = GetOwner()->FindComponentByClass<UDBJobComponent>())
    {
        JobComp->SetAnimationBlueprintForJob(EDBJob::None, false); // Generic locomotion
    }

    SetWeaponState(EWeaponState::Unarmed);
    
    UE_LOG(LogTemp, Warning, TEXT("✅ Weapon unequipped - using generic ABP"));
}

void UDBWeaponComponent::DrawWeapon()
{
    if (!CurrentWeaponData)
    {
        UE_LOG(LogTemp, Warning, TEXT("⚠️ No weapon equipped"));
        return;
    }

    if (WeaponState != EWeaponState::Sheathed)
    {
        UE_LOG(LogTemp, Warning, TEXT("⚠️ Weapon not sheathed (current state: %d)"), (int32)WeaponState);
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("⚔️ Draw weapon requested"));

    // Set state to Engaging immediately to prevent double-triggers
    SetWeaponState(EWeaponState::Engaging);

    // REMOVED: Don't switch ABP anymore!
    // SwitchAnimationBlueprintWithBlend(CurrentWeaponData->AnimationBlueprint, ...);

    // NEW: Just set the boolean in the Master ABP
    PlayDrawMontage();
    

    // Play draw montage after short delay
    FTimerHandle TimerHandle;
    GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
    {
        SetIsWeaponDrawnInABP(true);
    }, 0.2f, false);

    if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner()))
    {
        FGameplayEventData EventData;
        EventData.Instigator = GetOwner();
        
        FGameplayTag DrawTag = FGameplayTag::RequestGameplayTag(TEXT("Weapon.Drawn"));
        ASC->HandleGameplayEvent(DrawTag, &EventData);
        
        UE_LOG(LogTemp, Log, TEXT("⚔️ Sent Weapon.Drawn event"));
    }
}

void UDBWeaponComponent::PlayDrawMontage()
{
    if (!CurrentWeaponData || !CurrentWeaponData->DrawWeaponMontage)
    {
        UE_LOG(LogTemp, Warning, TEXT("⚠️ No draw montage - instant draw"));
        SetWeaponState(EWeaponState::Drawn);
        return;
    }

    USkeletalMeshComponent* BodyMesh = GetBodyMesh();
    if (!BodyMesh || !BodyMesh->GetAnimInstance())
    {
        UE_LOG(LogTemp, Error, TEXT("❌ No body mesh or anim instance"));
        SetWeaponState(EWeaponState::Drawn);
        return;
    }

    UAnimInstance* AnimInstance = BodyMesh->GetAnimInstance();
    
    UE_LOG(LogTemp, Warning, TEXT("🎮 Current ABP: %s"), 
        *AnimInstance->GetClass()->GetName());
    UE_LOG(LogTemp, Warning, TEXT("🎬 Trying to play montage: %s"), 
        *CurrentWeaponData->DrawWeaponMontage->GetName());
    
    // Check if montage is compatible with current skeleton
    if (CurrentWeaponData->DrawWeaponMontage->GetSkeleton() != BodyMesh->SkeletalMesh->GetSkeleton())
    {
        UE_LOG(LogTemp, Error, TEXT("❌ Montage skeleton mismatch!"));
        UE_LOG(LogTemp, Error, TEXT("   Montage Skeleton: %s"), 
            *CurrentWeaponData->DrawWeaponMontage->GetSkeleton()->GetName());
        UE_LOG(LogTemp, Error, TEXT("   Mesh Skeleton: %s"), 
            *BodyMesh->SkeletalMesh->GetSkeleton()->GetName());
        SetWeaponState(EWeaponState::Drawn);
        return;
    }
    
    // Get blend settings from weapon data
    float PlayRate = CurrentWeaponData->DrawMontagePlayRate;
    float BlendInTime = CurrentWeaponData->MontageBlendInTime;
    float BlendOutTime = CurrentWeaponData->MontageBlendOutTime;
    
    // Create blend settings for smooth start
    FAlphaBlendArgs BlendIn;
    BlendIn.BlendTime = BlendInTime;
    BlendIn.BlendOption = EAlphaBlendOption::Linear;
    
    // Play montage with blend settings
    float Duration = AnimInstance->Montage_PlayWithBlendSettings(
        CurrentWeaponData->DrawWeaponMontage,
        BlendIn,
        PlayRate,
        EMontagePlayReturnType::MontageLength,
        0.0f,
        true
    );

    if (Duration > 0.0f)
    {
        float ActualDuration = Duration / PlayRate;
        
        UE_LOG(LogTemp, Warning, TEXT("✅ Montage playing! Duration: %.2fs"), ActualDuration);
        
        // Set state to Drawn after montage duration
        FTimerHandle TimerHandle;
        GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
        {
            if (WeaponState == EWeaponState::Engaging)
            {
                SetWeaponState(EWeaponState::Drawn);
                UE_LOG(LogTemp, Warning, TEXT("✅ Weapon drawn!"));
            }
        }, ActualDuration, false);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("❌ Failed to play draw montage - Duration = 0!"));
        
        // Check active montages
        UAnimMontage* CurrentMontage = AnimInstance->GetCurrentActiveMontage();
        UE_LOG(LogTemp, Warning, TEXT("   Currently playing: %s"), 
            CurrentMontage ? *CurrentMontage->GetName() : TEXT("None"));
        
        SetWeaponState(EWeaponState::Drawn);
    }
}

void UDBWeaponComponent::SheathWeapon()
{
    if (WeaponState != EWeaponState::Drawn)
    {
        UE_LOG(LogTemp, Warning, TEXT("⚠️ Weapon not drawn (current state: %d)"), (int32)WeaponState);
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("⚔️ Sheathing weapon"));

    SetWeaponState(EWeaponState::Disengaging);

    // Play sheath montage
    PlaySheathMontage();

    if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner()))
    {
        FGameplayEventData EventData;
        EventData.Instigator = GetOwner();
        
        FGameplayTag SheathTag = FGameplayTag::RequestGameplayTag(TEXT("Weapon.Sheathed"));
        ASC->HandleGameplayEvent(SheathTag, &EventData);
        
        UE_LOG(LogTemp, Log, TEXT("⚔️ Sent Weapon.Sheathed event"));
    }
}

void UDBWeaponComponent::PlaySheathMontage()
{
    if (!CurrentWeaponData || !CurrentWeaponData->SheathWeaponMontage)
    {
        UE_LOG(LogTemp, Warning, TEXT("⚠️ No sheath montage - instant sheath"));
        
        MoveWeaponsToSheathedSockets();
        SetIsWeaponDrawnInABP(false);
        SetWeaponState(EWeaponState::Sheathed);
        return;
    }

    USkeletalMeshComponent* BodyMesh = GetBodyMesh();
    if (!BodyMesh || !BodyMesh->GetAnimInstance())
    {
        UE_LOG(LogTemp, Error, TEXT("❌ No body mesh or anim instance"));
        
        MoveWeaponsToSheathedSockets();
        SetIsWeaponDrawnInABP(false);
        SetWeaponState(EWeaponState::Sheathed);
        return;
    }

    UAnimInstance* AnimInstance = BodyMesh->GetAnimInstance();
    
    UE_LOG(LogTemp, Warning, TEXT("🎮 Current ABP: %s"), 
        *AnimInstance->GetClass()->GetName());
    
    // Get blend settings from weapon data
    float PlayRate = CurrentWeaponData->SheathMontagePlayRate;
    float BlendInTime = CurrentWeaponData->MontageBlendInTime;
    
    // Create blend settings
    FAlphaBlendArgs BlendIn;
    BlendIn.BlendTime = BlendInTime;
    BlendIn.BlendOption = EAlphaBlendOption::Linear;
    
    // Play montage with blend settings
    float Duration = AnimInstance->Montage_PlayWithBlendSettings(
        CurrentWeaponData->SheathWeaponMontage,
        BlendIn,
        PlayRate,
        EMontagePlayReturnType::MontageLength,
        0.0f,
        true
    );

    if (Duration > 0.0f)
    {
        float ActualDuration = Duration / PlayRate;
        
        UE_LOG(LogTemp, Warning, TEXT("▶️ Playing sheath montage: %s"), 
            *CurrentWeaponData->SheathWeaponMontage->GetName());
        UE_LOG(LogTemp, Warning, TEXT("   Duration: %.2fs | PlayRate: %.2fx | BlendIn: %.2fs"), 
            ActualDuration, PlayRate, BlendInTime);
        
        // CRITICAL: Bind to montage end delegate so we know when it finishes
        FOnMontageEnded MontageEndDelegate;
        MontageEndDelegate.BindUObject(this, &UDBWeaponComponent::OnSheathMontageEnded);
        AnimInstance->Montage_SetEndDelegate(MontageEndDelegate, CurrentWeaponData->SheathWeaponMontage);
        
        UE_LOG(LogTemp, Warning, TEXT("✅ Bound to sheath montage end delegate"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("❌ Failed to play sheath montage"));
        
        MoveWeaponsToSheathedSockets();
        SetIsWeaponDrawnInABP(false);
        SetWeaponState(EWeaponState::Sheathed);
    }
}

void UDBWeaponComponent::SwitchToGenericABPWithBlend()
{
    UDBJobComponent* JobComp = GetOwner()->FindComponentByClass<UDBJobComponent>();
    if (!JobComp)
    {
        UE_LOG(LogTemp, Error, TEXT("❌ No Job Component found!"));
        SetWeaponState(EWeaponState::Sheathed);
        SetIsWeaponDrawnInABP(false);
        return;
    }
    
    if (!JobComp->GenericAnimationBlueprint)
    {
        UE_LOG(LogTemp, Error, TEXT("❌ GenericAnimationBlueprint is NULL in Job Component!"));
        SetWeaponState(EWeaponState::Sheathed);
        SetIsWeaponDrawnInABP(false);
        return;
    }
    
    UE_LOG(LogTemp, Warning, TEXT("🎭 Switching to generic ABP: %s"), 
        *JobComp->GenericAnimationBlueprint->GetName());
    
    // Use default blend time if no weapon data
    float BlendTime = CurrentWeaponData ? CurrentWeaponData->ABPSwitchBlendTime : 0.3f;
    
    SwitchAnimationBlueprintWithBlend(JobComp->GenericAnimationBlueprint, BlendTime);
    
    // Set weapon state to Sheathed AFTER the blend completes
    FTimerHandle TimerHandle;
    GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
    {
        SetWeaponState(EWeaponState::Sheathed);
        SetIsWeaponDrawnInABP(false);
        UE_LOG(LogTemp, Warning, TEXT("✅ Back to unarmed stance!"));
    }, BlendTime, false);
}

void UDBWeaponComponent::SetIsWeaponDrawnInABP(bool bDrawn)
{
    USkeletalMeshComponent* BodyMesh = GetBodyMesh();
    if (!BodyMesh) return;

    UAnimInstance* AnimInstance = BodyMesh->GetAnimInstance();
    if (!AnimInstance) return;

    // Find the Is Weapon Drawn property (try multiple variations)
    FProperty* IsWeaponDrawnProperty = AnimInstance->GetClass()->FindPropertyByName(FName("bIsWeaponDrawn"));
    
    if (!IsWeaponDrawnProperty)
    {
        IsWeaponDrawnProperty = AnimInstance->GetClass()->FindPropertyByName(FName("IsWeaponDrawn"));
    }
    if (!IsWeaponDrawnProperty)
    {
        IsWeaponDrawnProperty = AnimInstance->GetClass()->FindPropertyByName(FName("Is Weapon Drawn"));
    }
    
    if (IsWeaponDrawnProperty)
    {
        FBoolProperty* BoolProp = CastField<FBoolProperty>(IsWeaponDrawnProperty);
        if (BoolProp)
        {
            void* PropertyValuePtr = BoolProp->ContainerPtrToValuePtr<void>(AnimInstance);
            BoolProp->SetPropertyValue(PropertyValuePtr, bDrawn);
            
            UE_LOG(LogTemp, Warning, TEXT("✅ bIsWeaponDrawn set to: %s"), bDrawn ? TEXT("true") : TEXT("false"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("❌ Could not find 'bIsWeaponDrawn' property"));
        
        // Debug: List all properties
        UE_LOG(LogTemp, Warning, TEXT("📋 Available bool properties:"));
        for (TFieldIterator<FProperty> It(AnimInstance->GetClass()); It; ++It)
        {
            if (FBoolProperty* BoolProperty = CastField<FBoolProperty>(*It))
            {
                UE_LOG(LogTemp, Warning, TEXT("   - %s"), *BoolProperty->GetName());
            }
        }
    }
}

void UDBWeaponComponent::SetWeaponStateInABP(EWeaponState State)
{
    USkeletalMeshComponent* BodyMesh = GetBodyMesh();
    if (!BodyMesh) return;

    UAnimInstance* AnimInstance = BodyMesh->GetAnimInstance();
    if (!AnimInstance) return;

    // Find WeaponState property
    FProperty* WeaponStateProperty = AnimInstance->GetClass()->FindPropertyByName(FName("WeaponState"));
    
    if (WeaponStateProperty)
    {
        FEnumProperty* EnumProp = CastField<FEnumProperty>(WeaponStateProperty);
        if (EnumProp)
        {
            void* PropertyValuePtr = EnumProp->ContainerPtrToValuePtr<void>(AnimInstance);
            EnumProp->GetUnderlyingProperty()->SetIntPropertyValue(PropertyValuePtr, static_cast<int64>(State));
            
            UE_LOG(LogTemp, Warning, TEXT("✅ WeaponState in ABP set to: %d"), (int32)State);
        }
    }
}

void UDBWeaponComponent::SwitchAnimationBlueprintWithBlend(TSubclassOf<UAnimInstance> NewAnimBP, float BlendTime)
{
    if (!NewAnimBP)
    {
        UE_LOG(LogTemp, Error, TEXT("❌ SwitchAnimationBlueprint: NewAnimBP is NULL!"));
        return;
    }

    USkeletalMeshComponent* BodyMesh = GetBodyMesh();
    if (!BodyMesh)
    {
        UE_LOG(LogTemp, Error, TEXT("❌ SwitchAnimationBlueprint: No body mesh!"));
        return;
    }

    UAnimInstance* CurrentAnimInstance = BodyMesh->GetAnimInstance();
    
    // Check if already using target ABP
    if (CurrentAnimInstance && CurrentAnimInstance->GetClass() == NewAnimBP)
    {
        UE_LOG(LogTemp, Warning, TEXT("⚠️ Already using target ABP, skipping switch"));
        return;
    }
    
    // CRITICAL: Stop all montages with blend before switching ABP
    if (CurrentAnimInstance)
    {
        UE_LOG(LogTemp, Warning, TEXT("🛑 Stopping all active montages (blend: %.2fs)"), BlendTime);
        CurrentAnimInstance->Montage_Stop(BlendTime);
    }
    
    // Wait for blend to complete before switching
    FTimerHandle BlendTimerHandle;
    GetWorld()->GetTimerManager().SetTimer(BlendTimerHandle, [this, BodyMesh, NewAnimBP]()
    {
        PerformABPSwitch(BodyMesh, NewAnimBP);
    }, BlendTime, false);
}

void UDBWeaponComponent::PerformABPSwitch(USkeletalMeshComponent* BodyMesh, TSubclassOf<UAnimInstance> NewAnimBP)
{
    if (!BodyMesh || !NewAnimBP) return;
    
    UAnimInstance* CurrentAnimInstance = BodyMesh->GetAnimInstance();
    UClass* OldABP = CurrentAnimInstance ? CurrentAnimInstance->GetClass() : nullptr;
    
    UE_LOG(LogTemp, Warning, TEXT("🎭 ABP Switch: %s → %s"), 
        OldABP ? *OldABP->GetName() : TEXT("None"),
        *NewAnimBP->GetName());

    // Perform the switch
    BodyMesh->SetAnimInstanceClass(NewAnimBP);
    
    UE_LOG(LogTemp, Warning, TEXT("✅ ABP switched successfully!"));
}

void UDBWeaponComponent::SwitchAnimationBlueprint(TSubclassOf<UAnimInstance> NewAnimBP)
{
    // Use default blend time
    SwitchAnimationBlueprintWithBlend(NewAnimBP, 0.3f);
}

void UDBWeaponComponent::ToggleWeapon()
{
    UE_LOG(LogTemp, Warning, TEXT("🔄 Toggle weapon (current state: %d)"), (int32)WeaponState);

    switch (WeaponState)
    {
    case EWeaponState::Sheathed:
        DrawWeapon();
        break;
            
    case EWeaponState::Drawn:
        SheathWeapon();
        break;
            
    case EWeaponState::Unarmed:
        UE_LOG(LogTemp, Warning, TEXT("⚠️ No weapon equipped"));
        break;
    
    case EWeaponState::Engaging:
    case EWeaponState::Disengaging:
        UE_LOG(LogTemp, Warning, TEXT("⚠️ Cannot toggle - weapon is transitioning (spam protection)"));
        break;
            
    default:
        UE_LOG(LogTemp, Warning, TEXT("⚠️ Unknown weapon state"));
        break;
    }
}

void UDBWeaponComponent::OnDrawComplete()
{
    UE_LOG(LogTemp, Warning, TEXT("🎬 Draw animation complete"));
    
    if (WeaponState != EWeaponState::Sheathed)
    {
        UE_LOG(LogTemp, Warning, TEXT("⚠️ OnDrawComplete but not sheathed (state: %d)"), (int32)WeaponState);
        return;
    }

    // Transition to Drawn state
    SetWeaponState(EWeaponState::Drawn);
    
    // CRITICAL: Defer ABP switch to next frame (can't switch during anim evaluation)
    if (CurrentWeaponData && CurrentWeaponData->AnimationBlueprint)
    {
        FTimerHandle TimerHandle;
        GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
        {
            if (CurrentWeaponData && CurrentWeaponData->AnimationBlueprint)
            {
                UE_LOG(LogTemp, Warning, TEXT("🎭 Switching to combat ABP: %s"), 
                    *CurrentWeaponData->AnimationBlueprint->GetName());
                SwitchAnimationBlueprint(CurrentWeaponData->AnimationBlueprint);
            }
        });
    }
    
    UE_LOG(LogTemp, Warning, TEXT("✅ Weapon now drawn - ready for combat"));
}

void UDBWeaponComponent::OnSheathComplete()
{
    UE_LOG(LogTemp, Warning, TEXT("✅ Sheath animation complete"));
    
    if (WeaponState != EWeaponState::Disengaging)
    {
        UE_LOG(LogTemp, Warning, TEXT("⚠️ OnSheathComplete but not disengaging (state: %d)"), (int32)WeaponState);
        return;
    }

    // Transition to Sheathed state
    SetWeaponState(EWeaponState::Sheathed);
    
    // Defer ABP switch back to GENERIC/UNARMED ABP
    FTimerHandle TimerHandle;
    FTimerDelegate TimerDelegate;
    TimerDelegate.BindLambda([this]()
    {
        if (UDBJobComponent* JobComp = GetOwner()->FindComponentByClass<UDBJobComponent>())
        {
            UE_LOG(LogTemp, Warning, TEXT("🎭 Switching back to generic unarmed ABP"));
            
            // Use the GenericAnimationBlueprint (ABP_Unarmed)
            if (JobComp->GenericAnimationBlueprint)
            {
                SwitchAnimationBlueprint(JobComp->GenericAnimationBlueprint);
            }
        }
    });
    GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, 0.1f, false);
}

void UDBWeaponComponent::MoveWeaponsToDrawnSockets()
{
    if (!CurrentWeaponData)
    {
        UE_LOG(LogTemp, Error, TEXT("❌ No weapon data to get socket names"));
        return;
    }

    UDBEquipmentComponent* EquipComp = GetOwner()->FindComponentByClass<UDBEquipmentComponent>();
    if (!EquipComp)
    {
        UE_LOG(LogTemp, Error, TEXT("❌ No Equipment Component found"));
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("📋 Current Weapon Data: %s"), *CurrentWeaponData->WeaponTypeName.ToString());
    UE_LOG(LogTemp, Warning, TEXT("   bUsesOffHand: %s"), CurrentWeaponData->bUsesOffHand ? TEXT("YES") : TEXT("NO"));
    UE_LOG(LogTemp, Warning, TEXT("   MainHandDrawnSocket: %s"), *CurrentWeaponData->MainHandDrawnSocket.ToString());
    UE_LOG(LogTemp, Warning, TEXT("   MainHandSheathedSocket: %s"), *CurrentWeaponData->MainHandSheathedSocket.ToString());
    UE_LOG(LogTemp, Warning, TEXT("   OffHandDrawnSocket: %s"), *CurrentWeaponData->OffHandDrawnSocket.ToString());
    UE_LOG(LogTemp, Warning, TEXT("   OffHandSheathedSocket: %s"), *CurrentWeaponData->OffHandSheathedSocket.ToString());

    // Move main hand weapon to drawn socket
    if (!CurrentWeaponData->MainHandDrawnSocket.IsNone())
    {
        EquipComp->MoveEquipmentToSocket(
            EEquipmentSlot::MainHand,
            CurrentWeaponData->MainHandDrawnSocket
        );
        
        UE_LOG(LogTemp, Warning, TEXT("🗡️ Moved main hand to: %s"), 
            *CurrentWeaponData->MainHandDrawnSocket.ToString());
    }
    
    // Move off-hand weapon if applicable
    if (CurrentWeaponData->bUsesOffHand && !CurrentWeaponData->OffHandDrawnSocket.IsNone())
    {
        EquipComp->MoveEquipmentToSocket(
            EEquipmentSlot::OffHand,
            CurrentWeaponData->OffHandDrawnSocket
        );
        
        UE_LOG(LogTemp, Warning, TEXT("🛡️ Moved off-hand to: %s"), 
            *CurrentWeaponData->OffHandDrawnSocket.ToString());
    }
}

void UDBWeaponComponent::MoveWeaponsToSheathedSockets()
{
    if (!CurrentWeaponData)
    {
        UE_LOG(LogTemp, Error, TEXT("❌ No weapon data to get socket names"));
        return;
    }

    UDBEquipmentComponent* EquipComp = GetOwner()->FindComponentByClass<UDBEquipmentComponent>();
    if (!EquipComp)
    {
        UE_LOG(LogTemp, Error, TEXT("❌ No Equipment Component found"));
        return;
    }

    // Move main hand weapon to sheathed socket
    if (!CurrentWeaponData->MainHandSheathedSocket.IsNone())
    {
        EquipComp->MoveEquipmentToSocket(
            EEquipmentSlot::MainHand,
            CurrentWeaponData->MainHandSheathedSocket
        );
        
        UE_LOG(LogTemp, Warning, TEXT("🔙 Moved main hand to: %s"), 
            *CurrentWeaponData->MainHandSheathedSocket.ToString());
    }

    UDBEquipmentItemData* MainHandItem = EquipComp->GetEquippedItem(EEquipmentSlot::MainHand);
    UDBEquipmentItemData* OffHandItem = EquipComp->GetEquippedItem(EEquipmentSlot::OffHand);
    
    UE_LOG(LogTemp, Warning, TEXT("📦 Equipped Items:"));
    UE_LOG(LogTemp, Warning, TEXT("   MainHand: %s"), MainHandItem ? *MainHandItem->ItemName.ToString() : TEXT("NONE"));
    UE_LOG(LogTemp, Warning, TEXT("   OffHand: %s"), OffHandItem ? *OffHandItem->ItemName.ToString() : TEXT("NONE"));
    
    // Move off-hand weapon if applicable
    if (CurrentWeaponData->bUsesOffHand && !CurrentWeaponData->OffHandSheathedSocket.IsNone())
    {
        EquipComp->MoveEquipmentToSocket(
            EEquipmentSlot::OffHand,
            CurrentWeaponData->OffHandSheathedSocket
        );
        
        UE_LOG(LogTemp, Warning, TEXT("🔙 Moved off-hand to: %s"), 
            *CurrentWeaponData->OffHandSheathedSocket.ToString());
    }
}

void UDBWeaponComponent::SwitchToGenericABP()
{
    UDBJobComponent* JobComp = GetOwner()->FindComponentByClass<UDBJobComponent>();
    if (!JobComp)
    {
        UE_LOG(LogTemp, Error, TEXT("❌ No Job Component found!"));
        return;
    }
    
    if (!JobComp->GenericAnimationBlueprint)
    {
        UE_LOG(LogTemp, Error, TEXT("❌ GenericAnimationBlueprint is NULL in Job Component!"));
        return;
    }
    
    UE_LOG(LogTemp, Warning, TEXT("🎭 Switching to generic ABP: %s"), 
        *JobComp->GenericAnimationBlueprint->GetName());
    
    SwitchAnimationBlueprint(JobComp->GenericAnimationBlueprint);
    
    // Set weapon state to Sheathed
    SetWeaponState(EWeaponState::Sheathed);
    
    UE_LOG(LogTemp, Warning, TEXT("✅ Back to unarmed stance!"));
}

void UDBWeaponComponent::OnDrawMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    if (bInterrupted)
    {
        UE_LOG(LogTemp, Warning, TEXT("⚠️ Draw montage interrupted"));
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("🎬 Draw montage ended"));
    
    // Switch to combat ABP
    if (CurrentWeaponData && CurrentWeaponData->AnimationBlueprint)
    {
        UE_LOG(LogTemp, Warning, TEXT("🎭 Switching to combat ABP: %s"), 
            *CurrentWeaponData->AnimationBlueprint->GetName());
        SwitchAnimationBlueprint(CurrentWeaponData->AnimationBlueprint);
    }
    
    // Transition to Drawn state
    SetWeaponState(EWeaponState::Drawn);
    
    UE_LOG(LogTemp, Warning, TEXT("✅ Weapon drawn and ready for combat!"));
}

void UDBWeaponComponent::OnSheathMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    UE_LOG(LogTemp, Warning, TEXT("🎬 Sheath montage ended (Interrupted: %s)"), 
        bInterrupted ? TEXT("YES") : TEXT("NO"));
    
    if (WeaponState == EWeaponState::Disengaging)
    {
        UE_LOG(LogTemp, Warning, TEXT("🎭 Completing sheath - updating state"));
        
        // Set to false AFTER animation completes
        SetIsWeaponDrawnInABP(false);
        SetWeaponState(EWeaponState::Sheathed);
        
        UE_LOG(LogTemp, Warning, TEXT("✅ Weapon sheathed!"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("⚠️ Sheath montage ended but state is %d (not Disengaging)"), 
            (int32)WeaponState);
    }
}

void UDBWeaponComponent::SetWeaponState(EWeaponState NewState)
{
    if (WeaponState == NewState) return;

    EWeaponState OldState = WeaponState;
    WeaponState = NewState;
    
    UE_LOG(LogTemp, Warning, TEXT("🔄 Weapon state: %d → %d"), (int32)OldState, (int32)NewState);
    
    OnWeaponStateChanged.Broadcast(NewState);
}

float UDBWeaponComponent::GetAutoAttackRange() const
{
    return CurrentWeaponData ? CurrentWeaponData->AutoAttackRange : 0.0f;
}

float UDBWeaponComponent::GetEngageRange() const
{
    return CurrentWeaponData ? CurrentWeaponData->EngageRange : 0.0f;
}

bool UDBWeaponComponent::CanAutoAttack() const
{
    return CurrentWeaponData && CurrentWeaponData->bCanAutoAttack && IsWeaponDrawn();
}

UAnimMontage* UDBWeaponComponent::GetRandomBasicAttack() const
{
    if (!CurrentWeaponData || CurrentWeaponData->BasicAttackMontages.Num() == 0)
    {
        return nullptr;
    }

    int32 RandomIndex = FMath::RandRange(0, CurrentWeaponData->BasicAttackMontages.Num() - 1);
    UAnimMontage* SelectedMontage = CurrentWeaponData->BasicAttackMontages[RandomIndex];
    
    UE_LOG(LogTemp, Warning, TEXT("🎲 Random attack %d/%d: %s"), 
        RandomIndex + 1, 
        CurrentWeaponData->BasicAttackMontages.Num(),
        SelectedMontage ? *SelectedMontage->GetName() : TEXT("NULL"));
    
    return SelectedMontage;
}

USkeletalMeshComponent* UDBWeaponComponent::GetBodyMesh() const
{
    AActor* Owner = GetOwner();
    if (!Owner) return nullptr;

    // For metahuman characters, the main mesh is usually "CharacterMesh0" or just "Mesh"
    if (ACharacter* Character = Cast<ACharacter>(Owner))
    {
        // Try GetMesh() first - this is the main character mesh
        USkeletalMeshComponent* CharMesh = Character->GetMesh();
        if (CharMesh)
        {
            UE_LOG(LogTemp, Log, TEXT("Using Character Mesh: %s"), *CharMesh->GetName());
            return CharMesh;
        }
    }

    // Fallback: search components
    TArray<USkeletalMeshComponent*> MeshComps;
    Owner->GetComponents<USkeletalMeshComponent>(MeshComps);

    // Try to find by name
    for (USkeletalMeshComponent* Mesh : MeshComps)
    {
        FString MeshName = Mesh->GetName();
        if (MeshName == TEXT("CharacterMesh0") || 
            MeshName == TEXT("Mesh") ||
            MeshName.Contains(TEXT("Body")))
        {
            UE_LOG(LogTemp, Log, TEXT("Found mesh: %s"), *MeshName);
            return Mesh;
        }
    }

    // Last resort - return first skeletal mesh found
    if (MeshComps.Num() > 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("Using first skeletal mesh: %s"), *MeshComps[0]->GetName());
        return MeshComps[0];
    }

    UE_LOG(LogTemp, Error, TEXT("No skeletal mesh found!"));
    return nullptr;
}
