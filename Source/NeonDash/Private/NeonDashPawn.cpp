// Copyright Epic Games, Inc. All Rights Reserved.

#include "NeonDashPawn.h"
#include "NeonDashProjectile.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/CollisionProfile.h"
#include "Engine/StaticMesh.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"

const FName ANeonDashPawn::MoveForwardBinding("MoveForward");
const FName ANeonDashPawn::MoveRightBinding("MoveRight");
const FName ANeonDashPawn::FireForwardBinding("FireForward");
const FName ANeonDashPawn::FireRightBinding("FireRight");
const FName ANeonDashPawn::DashBinding("Dash");

ANeonDashPawn::ANeonDashPawn()
{	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ShipMesh(TEXT("/Game/TwinStick/Meshes/TwinStickUFO.TwinStickUFO"));
	// Create the mesh component
	ShipMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipMesh"));
	RootComponent = ShipMeshComponent;
	ShipMeshComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	ShipMeshComponent->SetStaticMesh(ShipMesh.Object);
	
	// Cache our sound effect
	static ConstructorHelpers::FObjectFinder<USoundBase> FireAudio(TEXT("/Game/TwinStick/Audio/TwinStickFire.TwinStickFire"));
	FireSound = FireAudio.Object;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when ship does
	CameraBoom->TargetArmLength = 1200.f;
	CameraBoom->SetRelativeRotation(FRotator(-80.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	CameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	CameraComponent->bUsePawnControlRotation = false;	// Camera does not rotate relative to arm

	// Movement
	MoveSpeed = MoveSpeedInit = 1000.0f;
	bCanDash = true;
	// Barrier 
	DashOffest = 500.f;
	MaxBarriersAllowed = 3;
	// Weapon
	GunOffset = FVector(90.f, 0.f, 0.f);
	FireRate = 0.1f;
	ChargeRate = 1.f;
	chargeMultiplier = 0;
	bCanFire = true;
	bIsFireCharging = false;

	AutoPossessPlayer = EAutoReceiveInput::Disabled;
	AutoPossessAI = EAutoPossessAI::Disabled;
	
}

void ANeonDashPawn::BeginPlay()
{
	Super::BeginPlay();

	ShipMaterialInstance = UMaterialInstanceDynamic::Create(ShipMeshComponent->GetMaterial(4), nullptr);
	ShipMeshComponent->SetMaterial(4, ShipMaterialInstance);
}

void ANeonDashPawn::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	// set up gameplay key bindings
	PlayerInputComponent->BindAxis(MoveForwardBinding);
	PlayerInputComponent->BindAxis(MoveRightBinding);
	PlayerInputComponent->BindAxis(FireForwardBinding);
	PlayerInputComponent->BindAxis(FireRightBinding);

	PlayerInputComponent->BindAction(DashBinding, IE_Pressed, this, &ANeonDashPawn::OnPawnDash);

}

void ANeonDashPawn::Tick(float DeltaSeconds)
{
	// Find movement direction
	const float ForwardValue = GetInputAxisValue(MoveForwardBinding);
	const float RightValue = GetInputAxisValue(MoveRightBinding);

	// Clamp max size so that (X=1, Y=1) doesn't cause faster movement in diagonal directions
	const FVector MoveDirection = FVector(ForwardValue, RightValue, 0.f).GetClampedToMaxSize(1.0f);

	// Calculate  movement
	const FVector Movement = MoveDirection * MoveSpeed * DeltaSeconds;

	// If non-zero size, move this actor
	if (Movement.SizeSquared() > 0.0f)
	{
		const FRotator NewRotation = Movement.Rotation();
		FHitResult Hit(1.f);
		RootComponent->MoveComponent(Movement, NewRotation, true, &Hit);
		
		if (Hit.IsValidBlockingHit())
		{
			const FVector Normal2D = Hit.Normal.GetSafeNormal2D();
			const FVector Deflection = FVector::VectorPlaneProject(Movement, Normal2D) * (1.f - Hit.Time);
			RootComponent->MoveComponent(Deflection, NewRotation, true);
		}
	}
	
	// Create fire direction vector
	const float FireForwardValue = GetInputAxisValue(FireForwardBinding);
	const float FireRightValue = GetInputAxisValue(FireRightBinding);
	const FVector FireDirection = FVector(FireForwardValue, FireRightValue, 0.f);

	if ((!bIsFireCharging) && FVector::DistXY(FireDirection, FVector::ZeroVector) > 0.5f) {
		bIsFireCharging = true;

		GetWorld()->GetTimerManager().SetTimer(TimerHandle_ChargeTimer, this, &ANeonDashPawn::ChargeTimerExpired, ChargeRate, true);

	}
	else if (bIsFireCharging && FVector::DistXY(FireDirection, FVector::ZeroVector) < 0.5f) {
		GetWorld()->GetTimerManager().PauseTimer(TimerHandle_ChargeTimer);
		bIsFireCharging = false;

		if (FireDirection != FVector::ZeroVector)
			FireShot(FireDirection);
		else
			FireShot(PrevFireDirection);

		chargeMultiplier = 0;
		SetChargeValues(chargeMultiplier);

	}

	//if (GEngine)
	//	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Charge: %d"), chargeMultiplier));//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("FireDir %s"), *FireDirection.ToString()));

	PrevFireDirection = FireDirection;
}

void ANeonDashPawn::FireShot(FVector FireDirection)
{
	// If it's ok to fire again
	if (bCanFire == true)
	{
		// If we are pressing fire stick in a direction
		if (FireDirection.SizeSquared() > 0.0f)
		{
			const FRotator FireRotation = FireDirection.Rotation();
			// Spawn projectile at an offset from this pawn
			const FVector SpawnLocation = GetActorLocation() + FireRotation.RotateVector(GunOffset);

			UWorld* const World = GetWorld();
			if (World != nullptr)
			{
				// spawn the projectile
				World->SpawnActor<ANeonDashProjectile>(SpawnLocation, FireRotation)->SetProjectileChargeState(chargeMultiplier);
			}

			bCanFire = false;
			World->GetTimerManager().SetTimer(TimerHandle_ShotTimerExpired, this, &ANeonDashPawn::ShotTimerExpired, FireRate);

			// try and play the sound if specified
			if (FireSound != nullptr)
			{
				UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
			}

			bCanFire = false;
		}
	}
}

void ANeonDashPawn::ShotTimerExpired()
{
	bCanFire = true;
}

void ANeonDashPawn::ChargeTimerExpired()
{
	chargeMultiplier++;

	SetChargeValues(chargeMultiplier);
}

void ANeonDashPawn::DashTimerExpired()
{
	bCanDash = true;
}

void ANeonDashPawn::SetChargeValues(int chargeValue)
{
	if (chargeValue == 0) {
		//reset
		ShipMaterialInstance->SetScalarParameterValue(TEXT("Emissive"), 0);
		MoveSpeed = MoveSpeedInit;
	}
	else {
		ShipMaterialInstance->SetScalarParameterValue(TEXT("Emissive"), chargeValue * 5);
		MoveSpeed /= 2;
	}

}

void ANeonDashPawn::OnPawnDash()
{
	if (bCanDash) {

		// Find movement direction
		const float ForwardValue = GetInputAxisValue(MoveForwardBinding);
		const float RightValue = GetInputAxisValue(MoveRightBinding);

		// Clamp max size so that (X=1, Y=1) doesn't cause faster movement in diagonal directions
		const FVector MoveDirection = FVector(ForwardValue, RightValue, 0.f);//GetClampedToMaxSize(1.0f);
		// Calculate  movement
		FVector Movement = MoveDirection.GetSafeNormal() * DashOffest;

		if (Movement.SizeSquared() <= 0.0f)
		{
			Movement = GetActorForwardVector().GetSafeNormal() * DashOffest;
		}

		const FRotator NewRotation = Movement.Rotation();

		RootComponent->MoveComponent(Movement, NewRotation, true);


		if (DashBarrierClass)
		{
			FVector BarrierLocation = GetActorLocation() - (Movement / 2.f);
			FRotator BarrierRotation = GetActorForwardVector().Rotation();

			if (MaxBarriersAllowed > 0)
			{

				FActorSpawnParameters SpawnInfo;
				SpawnInfo.Owner = this;
				AActor* NewBarrier = GetWorld()->SpawnActor<AActor>(DashBarrierClass, BarrierLocation, BarrierRotation, SpawnInfo);
				NewBarrier->SetActorScale3D(FVector(DashOffest / 100.f, 1.f, 1.f));
				UStaticMeshComponent* BarrierMesh = Cast<UStaticMeshComponent>(NewBarrier->GetComponentByClass(UStaticMeshComponent::StaticClass()));

				if (BarrierMesh)
				{
					auto BarrierMaterial = BarrierMesh->GetMaterial(0);
					auto DynamicMat = UMaterialInstanceDynamic::Create(BarrierMaterial, nullptr);
					//auto ShipMaterial = ShipMeshComponent->GetMaterial(0);
					FLinearColor ShipColor;
					ShipMaterialInstance->GetVectorParameterValue(TEXT("DiffuseColor"), ShipColor);
					DynamicMat->SetVectorParameterValue(TEXT("DiffuseColor"), ShipColor);
					BarrierMesh->SetMaterial(0, DynamicMat);
				}

				SpawnedBarriers.Enqueue(NewBarrier);
				MaxBarriersAllowed--;
			}
			else {

				AActor* OldestBarrier = nullptr;

				if (SpawnedBarriers.Dequeue(OldestBarrier)) {
					OldestBarrier->SetActorLocationAndRotation(BarrierLocation, BarrierRotation);
					SpawnedBarriers.Enqueue(OldestBarrier);
				}

			}

		}

		bCanDash = false;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_DashTimer, this, &ANeonDashPawn::DashTimerExpired, ChargeRate);

	}

}