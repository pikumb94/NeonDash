// Fill out your copyright notice in the Description page of Project Settings.


#include "ReplicatedStateComponent.h"
#include "NeonDashGameMode.h"
#include "Net/UnrealNetwork.h"
#include "NeonDashPlayerState.h"

// Sets default values for this component's properties
UReplicatedStateComponent::UReplicatedStateComponent()
{
	DefaultHealth = Health = 3.f;
	bIsDead = false;

	TeamNum = 255;

	SetIsReplicatedByDefault(true);
}


// Called when the game starts
void UReplicatedStateComponent::BeginPlay()
{
	Super::BeginPlay();

	// Only hook if we are server
	if (GetOwnerRole() == ROLE_Authority)
	{
		AActor* MyOwner = GetOwner();
		if (MyOwner)
		{
			MyOwner->OnTakeAnyDamage.AddDynamic(this, &UReplicatedStateComponent::HandleTakeAnyDamage);
		}
	}

	Health = DefaultHealth;
}


void UReplicatedStateComponent::OnRep_Health(float OldHealth)
{
	float Damage = Health - OldHealth;

	OnHealthChanged.Broadcast(this, Health, Damage, nullptr, nullptr, nullptr);
}


void UReplicatedStateComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy,
	AActor* DamageCauser)
{
	if (Damage <= 0.0f || bIsDead)
	{
		return;
	}

	if (DamageCauser != DamagedActor && IsFriendly(DamagedActor, DamageCauser))
	{
		return;
	}

	// Update health clamped
	Health = FMath::Clamp(Health - Damage, 0.0f, DefaultHealth);

	UE_LOG(LogTemp, Log, TEXT("Health Changed: %s"), *FString::SanitizeFloat(Health));

	bIsDead = Health <= 0.0f;

	OnHealthChanged.Broadcast(this, Health, Damage, DamageType, InstigatedBy, DamageCauser);

	if (bIsDead)
	{
		ANeonDashGameMode* GM = Cast<ANeonDashGameMode>(GetWorld()->GetAuthGameMode());
		if (GM)
		{
			auto PlayerStateActor = Cast<ANeonDashPlayerState>(DamagedActor);
			auto PlayerStateCauser = Cast<ANeonDashPlayerState>(DamageCauser);
			
			GM->OnActorKilled.Broadcast(PlayerStateActor->GetPawn(), PlayerStateCauser->GetPawn(), InstigatedBy);
		}
	}
}


void UReplicatedStateComponent::Heal(float HealAmount)
{
	if (HealAmount <= 0.0f || Health <= 0.0f)
	{
		return;
	}

	Health = FMath::Clamp(Health + HealAmount, 0.0f, DefaultHealth);

	UE_LOG(LogTemp, Log, TEXT("Health Changed: %s (+%s)"), *FString::SanitizeFloat(Health), *FString::SanitizeFloat(HealAmount));

	OnHealthChanged.Broadcast(this, Health, -HealAmount, nullptr, nullptr, nullptr);
}


bool UReplicatedStateComponent::IsFriendly(AActor* ActorA, AActor* ActorB)
{
	if (ActorA == nullptr || ActorB == nullptr)
	{
		// Assume Friendly
		return true;
	}

	UReplicatedStateComponent* HealthCompA = Cast<UReplicatedStateComponent>(ActorA->GetComponentByClass(UReplicatedStateComponent::StaticClass()));
	UReplicatedStateComponent* HealthCompB = Cast<UReplicatedStateComponent>(ActorB->GetComponentByClass(UReplicatedStateComponent::StaticClass()));

	if (HealthCompA == nullptr || HealthCompB == nullptr)
	{
		// Assume friendly
		return true;
	}

	return HealthCompA->TeamNum == HealthCompB->TeamNum;
}


float UReplicatedStateComponent::GetHealth() const
{
	return Health;
}

void UReplicatedStateComponent::SetHealth(float NewHealth)
{
	Health = NewHealth;
}


void UReplicatedStateComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UReplicatedStateComponent, Health);
}

