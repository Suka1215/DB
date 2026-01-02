#include "DBAIStateComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"

UDBAIStateComponent::UDBAIStateComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UDBAIStateComponent::BeginPlay()
{
    Super::BeginPlay();

    // Get Ability System Component from owner
    if (GetOwner()->Implements<UAbilitySystemInterface>())
    {
        ASC = Cast<IAbilitySystemInterface>(GetOwner())->GetAbilitySystemComponent();
    }

    if (!ASC)
    {
        UE_LOG(LogTemp, Error, TEXT("❌ DBAIStateComponent: Owner %s does not have an AbilitySystemComponent!"), 
            *GetOwner()->GetName());
    }
}

void UDBAIStateComponent::SetTag(FName TagName)
{
    if (!ASC)
    {
        UE_LOG(LogTemp, Error, TEXT("❌ DBAIStateComponent: No ASC!"));
        return;
    }

    FGameplayTag NewTag = FGameplayTag::RequestGameplayTag(TagName);
    if (!NewTag.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("⚠️ DBAIStateComponent: Invalid tag '%s'"), *TagName.ToString());
        return;
    }

    SetGameplayTag(NewTag);
}

void UDBAIStateComponent::SetGameplayTag(FGameplayTag Tag)
{
    if (!ASC || !Tag.IsValid()) return;

    // Get parent category (e.g., "State.AI" from "State.AI.Idle")
    FGameplayTag ParentTag = GetParentTag(Tag);

    // Remove all tags in the same category
    if (ParentTag.IsValid())
    {
        FGameplayTagContainer CategoryTags = ASC->GetOwnedGameplayTags().Filter(
            FGameplayTagContainer(ParentTag)
        );
        
        if (CategoryTags.Num() > 0)
        {
            ASC->RemoveLooseGameplayTags(CategoryTags);
        }
    }

    // Add the new tag
    ASC->AddLooseGameplayTag(Tag);

    UE_LOG(LogTemp, Log, TEXT("🏷️ %s → %s"), 
        *GetOwner()->GetName(), 
        *Tag.ToString());
}

void UDBAIStateComponent::AddTag(FName TagName)
{
    if (!ASC) return;

    FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TagName);
    if (Tag.IsValid())
    {
        ASC->AddLooseGameplayTag(Tag);
        UE_LOG(LogTemp, Log, TEXT("➕ %s added tag: %s"), *GetOwner()->GetName(), *TagName.ToString());
    }
}

void UDBAIStateComponent::AddGameplayTag(FGameplayTag Tag)
{
    if (!ASC || !Tag.IsValid()) return;

    ASC->AddLooseGameplayTag(Tag);
    UE_LOG(LogTemp, Log, TEXT("➕ %s added tag: %s"), *GetOwner()->GetName(), *Tag.ToString());
}

void UDBAIStateComponent::RemoveTag(FName TagName)
{
    if (!ASC) return;

    FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TagName);
    if (Tag.IsValid())
    {
        ASC->RemoveLooseGameplayTag(Tag);
        UE_LOG(LogTemp, Log, TEXT("➖ %s removed tag: %s"), *GetOwner()->GetName(), *TagName.ToString());
    }
}

void UDBAIStateComponent::RemoveGameplayTag(FGameplayTag Tag)
{
    if (!ASC || !Tag.IsValid()) return;

    ASC->RemoveLooseGameplayTag(Tag);
    UE_LOG(LogTemp, Log, TEXT("➖ %s removed tag: %s"), *GetOwner()->GetName(), *Tag.ToString());
}

void UDBAIStateComponent::RemoveTagsInCategory(FName CategoryTag)
{
    if (!ASC) return;

    FGameplayTag ParentTag = FGameplayTag::RequestGameplayTag(CategoryTag);
    if (!ParentTag.IsValid()) return;

    FGameplayTagContainer CategoryTags = ASC->GetOwnedGameplayTags().Filter(
        FGameplayTagContainer(ParentTag)
    );
    
    if (CategoryTags.Num() > 0)
    {
        ASC->RemoveLooseGameplayTags(CategoryTags);
        UE_LOG(LogTemp, Log, TEXT("🗑️ %s removed all tags in category: %s"), 
            *GetOwner()->GetName(), *CategoryTag.ToString());
    }
}

bool UDBAIStateComponent::HasTag(FName TagName) const
{
    if (!ASC) return false;

    FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TagName);
    return Tag.IsValid() ? ASC->HasMatchingGameplayTag(Tag) : false;
}

FGameplayTag UDBAIStateComponent::GetCurrentTagInCategory(FName CategoryTag) const
{
    if (!ASC) return FGameplayTag();

    FGameplayTag ParentTag = FGameplayTag::RequestGameplayTag(CategoryTag);
    if (!ParentTag.IsValid()) return FGameplayTag();

    FGameplayTagContainer CategoryTags = ASC->GetOwnedGameplayTags().Filter(
        FGameplayTagContainer(ParentTag)
    );

    // Return the first tag found in this category
    if (CategoryTags.Num() > 0)
    {
        return CategoryTags.First();
    }

    return FGameplayTag();
}

FGameplayTag UDBAIStateComponent::GetParentTag(const FGameplayTag& Tag) const
{
    if (!Tag.IsValid()) return FGameplayTag();

    // Get the parent by removing the last part of the tag
    // e.g., "State.AI.Idle" → "State.AI"
    FGameplayTag ParentTag = Tag.RequestDirectParent();
    return ParentTag;
}