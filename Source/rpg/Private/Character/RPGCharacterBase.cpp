// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/RPGCharacterBase.h"
#include "AbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "AbilitySystem/RPGAbilitySystemComponent.h"
#include "rpg/rpg.h"

// Sets default values
ARPGCharacterBase::ARPGCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetGenerateOverlapEvents(false);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Projectile, ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);
}

UAbilitySystemComponent* ARPGCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UAnimMontage* ARPGCharacterBase::GetHitReactMontage_Implementation()
{
	return HitReactMontage;
}

void ARPGCharacterBase::Die()
{
	MulticastHandleDeath();
}
void ARPGCharacterBase::MulticastHandleDeath_Implementation()
{
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetEnableGravity(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}


void ARPGCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

FVector ARPGCharacterBase::GetCombatSocketLocation()
{
	return GetMesh()->GetSocketLocation(HandRigthSocket);
}

void ARPGCharacterBase::InitAbilityActorInfo()
{
}

void ARPGCharacterBase::ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const
{
	check(IsValid(GetAbilitySystemComponent()));
	check(GameplayEffectClass);
	FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	ContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(GameplayEffectClass, Level, ContextHandle);
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), GetAbilitySystemComponent());
}

void ARPGCharacterBase::InitializeDefaultAttributes() const
{
	ApplyEffectToSelf(DefaultPrimaryAttributes, 1.f);
	ApplyEffectToSelf(DefaultSecondaryAttributes, 1.f);
	ApplyEffectToSelf(DefaultVitalAttributes, 1.f);
}

void ARPGCharacterBase::AddCharacterAbilities()
{
	URPGAbilitySystemComponent* RPGASC = CastChecked<URPGAbilitySystemComponent>(AbilitySystemComponent);
	if (!HasAuthority()) return;
	RPGASC->AddCharacterAbilities(StartupAbilities);
	RPGASC->AddCharacterPassiveAbilities(StartupPassiveAbilities);
}


ECharacterClass ARPGCharacterBase::GetCharacterClass_Implementation()
{
	return CharacterClass;
}