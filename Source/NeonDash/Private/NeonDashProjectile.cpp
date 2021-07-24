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
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;
	ProjectileMovement->ProjectileGravityScale = 0.f; // No gravity
	ProjectileMovement->Bounciness = 1.f;
	// Die after 3 seconds by default
	//InitialLifeSpan = 3.0f;
	bCanProjectileDamage = false;
}

void ANeonDashProjectile::BeginPlay()
{
	Super::BeginPlay();
	ProjectileMaterialInstance = UMaterialInstanceDynamic::Create(ProjectileMesh->GetMaterial(0), nullptr);
	ProjectileMesh->SetMaterial(0, ProjectileMaterialInstance);
}

void ANeonDashProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * 20.0f, GetActorLocation());
	}
	
	if (!AllowedBounceCount)
		Destroy();

	AllowedBounceCount--;

}

void ANeonDashProjectile::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (GetInstigator() != OtherActor) {

		auto HitPawn = Cast<ANeonDashPawn>(OtherActor);

		if (HitPawn) {

			auto HitPlayerState = Cast<ANeonDashPlayerState>(HitPawn->GetPlayerState());
			auto InsigatorPlayerState = Cast<ANeonDashPlayerState>(GetInstigator()->GetPlayerState());

			if (HitPlayerState && InsigatorPlayerState) {
				if (HitPlayerState->GetPlayerTeamNumber() != InsigatorPlayerState->GetPlayerTeamNumber()) {

					//A projectile Spawned by an opposing Pawn, hit the Player Pawn: apply damage
					if(bCanProjectileDamage)
						UGameplayStatics::ApplyDamage(HitPlayerState, 1, GetInstigatorController(), InsigatorPlayerState, nullptr);
					
					Destroy();
				}

			}
		}
	}

}

void ANeonDashProjectile::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	bCanProjectileDamage = true;
	if (OtherActor->GetName().Contains("Barrier"))
	{
		UStaticMeshComponent* BarrierMesh = Cast<UStaticMeshComponent>(OtherActor->GetComponentByClass(UStaticMeshComponent::StaticClass()));
		FLinearColor BarrierColor;
		BarrierMesh->GetMaterial(0)->GetVectorParameterValue(TEXT("DiffuseColor"), BarrierColor);
		ProjectileMaterialInstance->SetVectorParameterValue(TEXT("DiffuseColor"), BarrierColor);
		PowerUpProjectile();
	}
}

void ANeonDashProjectile::SetProjectileChargeState(int chargeCount)
{
	int count = chargeCount + 1;
	AllowedBounceCount = chargeCount;
	SetActorScale3D(GetActorScale3D() * count);
	ProjectileMovement->InitialSpeed /= count;
	ProjectileMovement->MaxSpeed /= count;
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
