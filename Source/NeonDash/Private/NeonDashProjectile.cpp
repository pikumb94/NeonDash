// Copyright Epic Games, Inc. All Rights Reserve

#include "NeonDashProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Engine/StaticMesh.h"
#include "NeonDashPawn.h"
#include "NeonDashPlayerState.h"
#include "Kismet/GameplayStatics.h"

ANeonDashProjectile::ANeonDashProjectile() 
{
	// Static reference to the mesh to use for the projectile
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ProjectileMeshAsset(TEXT("/Game/TwinStick/Meshes/TwinStickProjectile.TwinStickProjectile"));

	// Create mesh component for the projectile sphere
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh0"));
	ProjectileMesh->SetStaticMesh(ProjectileMeshAsset.Object);
	ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileMesh->BodyInstance.SetCollisionProfileName("Projectile");
	ProjectileMesh->OnComponentHit.AddDynamic(this, &ANeonDashProjectile::OnHit);		// set up a notification for when this component hits something
	ProjectileMesh->OnComponentBeginOverlap.AddDynamic(this, &ANeonDashProjectile::OnBeginOverlap);
	ProjectileMesh->OnComponentEndOverlap.AddDynamic(this, &ANeonDashProjectile::OnEndOverlap);

	RootComponent = ProjectileMesh;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement0"));
	ProjectileMovement->UpdatedComponent = ProjectileMesh;
	ProjectileMovement->InitialSpeed = 1500.f;
	ProjectileMovement->MaxSpeed = 1500.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;
	ProjectileMovement->ProjectileGravityScale = 0.f; // No gravity
	ProjectileMovement->Bounciness = 1.f;
	// Die after 3 seconds by default
	//InitialLifeSpan = 3.0f;
	bCanProjectileDamage = false;

	CurrentProjectileState = 0;

}

void ANeonDashProjectile::BeginPlay()
{
	Super::BeginPlay();
	InitialProjectileMaterial = ProjectileMesh->GetMaterial(0);
	ProjectileMaterialInstance = UMaterialInstanceDynamic::Create(InitialProjectileMaterial, nullptr);
	ProjectileMesh->SetMaterial(0, ProjectileMaterialInstance);
}

void ANeonDashProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{

	//Wall-Projectile Hit-Bounce
	auto OtherProjectile = Cast<ANeonDashProjectile>(OtherActor);
	if (OtherProjectile) {

		if (CurrentProjectileState != 0 && OtherProjectile->CurrentProjectileState != 0 && CurrentProjectileState != OtherProjectile->CurrentProjectileState) {

			auto Size = GetActorScale3D().SizeSquared();
			auto OtherSize = OtherProjectile->GetActorScale3D().SizeSquared();
			if (Size > OtherSize) {

				RecolorProjectile(ProjectileMesh, CurrentProjectileState);

			}
			else {
				if (Size > OtherSize)
					RecolorProjectile(OtherProjectile->ProjectileMesh, OtherProjectile->CurrentProjectileState);
				else {
					RecolorProjectile(ProjectileMesh, CurrentProjectileState, true);
					RecolorProjectile(OtherProjectile->ProjectileMesh, OtherProjectile->CurrentProjectileState, true);

				}

			}
		}
	}
	else {
		if (!AllowedBounceCount)
			Destroy();

		AllowedBounceCount--;
	}

}

void ANeonDashProjectile::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//Define behaviour of projectiles overlapping with pawns

	if (GetInstigator() != OtherActor) {

		auto HitPawn = Cast<ANeonDashPawn>(OtherActor);

		if (HitPawn) {

			auto HitPlayerState = Cast<ANeonDashPlayerState>(HitPawn->GetPlayerState());

			if (HitPlayerState && CurrentProjectileState > 0) {
				if (HitPlayerState->GetPlayerTeamNumber() != CurrentProjectileState) {

					//A projectile Spawned by an opposing Pawn, hit the Player Pawn: apply damage
					if (bCanProjectileDamage) {

						switch ((int)HitPlayerState->GetHealth()) {
						case 3:
							HitPawn->Bump1ShipMaterialInstance->SetScalarParameterValue(TEXT("Emissive"), -10);
							break;
						case 2:
							HitPawn->Bump2ShipMaterialInstance->SetScalarParameterValue(TEXT("Emissive"), -10);
							break;
						default:
							HitPawn->Bump3ShipMaterialInstance->SetScalarParameterValue(TEXT("Emissive"), -10);
						}

						auto InsigatorPlayerState = Cast<ANeonDashPlayerState>(GetInstigator()->GetPlayerState());
						UGameplayStatics::ApplyDamage(HitPlayerState, 1, GetInstigatorController(), InsigatorPlayerState, nullptr);
					}

					Destroy();
				}

				else {
					//If I'm hit by my projectiles=> reset alla barriers
					//HitPawn->ResetAllBarriers();
				}

			}

		}
	}

}

void ANeonDashProjectile::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//Define behaviour of projectiles when exiting a barrier

	bCanProjectileDamage = true;
	if (OtherActor->GetName().Contains("Barrier"))
	{
		auto OtherPlayerState = Cast<ANeonDashPlayerState>(OtherActor->GetInstigator()->GetPlayerState());

		UStaticMeshComponent* BarrierMesh = Cast<UStaticMeshComponent>(OtherActor->GetComponentByClass(UStaticMeshComponent::StaticClass()));

		RecolorProjectile(BarrierMesh, OtherPlayerState->GetPlayerTeamNumber());

		PowerUpProjectile();
	}
}

void ANeonDashProjectile::RecolorProjectile(UStaticMeshComponent* CollidingMesh, uint8 CollidingMeshState, bool bForceResetMaterial)
{
	if (CurrentProjectileState == CollidingMeshState)
		return;

	FLinearColor CollidingColor;
	CurrentProjectileState = CollidingMeshState;

	if (bForceResetMaterial) {
		InitialProjectileMaterial->GetVectorParameterValue(TEXT("DiffuseColor"), CollidingColor);
		ProjectileMaterialInstance->SetVectorParameterValue(TEXT("DiffuseColor"), CollidingColor);
		return;
	}

	CollidingMesh->GetMaterial(0)->GetVectorParameterValue(TEXT("DiffuseColor"), CollidingColor);
	ProjectileMaterialInstance->SetVectorParameterValue(TEXT("DiffuseColor"), CollidingColor);

}

void ANeonDashProjectile::SetProjectileChargeState(int chargeCount)
{
	int count = chargeCount + 1;
	AllowedBounceCount = chargeCount;
	SetActorScale3D(GetActorScale3D() * count);
	ProjectileMovement->InitialSpeed /= count;
	ProjectileMovement->MaxSpeed /= count;
}

void ANeonDashProjectile::SetSniperProjectileChargeState(int chargeCount)
{	


	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::SanitizeFloat(chargeCount));

	if (chargeCount > 0) {
		ProjectileMovement->MaxSpeed *= 2;
		ProjectileMovement->Velocity *= 2;
		AllowedBounceCount = chargeCount+1;
		SetActorScale3D(GetActorScale3D() * FMath::Clamp(chargeCount,1,5));
	}
	else {
		AllowedBounceCount = 1;
	}

	//else
		//SetActorScale3D(GetActorScale3D() /2);
}

void ANeonDashProjectile::PowerUpProjectile()
{
	//ProjectileMovement->InitialSpeed *= 2;
	ProjectileMovement->MaxSpeed *= 2;
	ProjectileMovement->Velocity *= 2;
	float EmissiveValue = 0.f;
	ProjectileMaterialInstance->GetScalarParameterValue(TEXT("Emissive"), EmissiveValue);
	ProjectileMaterialInstance->SetScalarParameterValue(TEXT("Emissive"), (EmissiveValue == 0 ? 1 : EmissiveValue * 5));

}
