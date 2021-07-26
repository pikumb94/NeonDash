// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NeonDashPawn.generated.h"

UCLASS(Blueprintable)
class ANeonDashPawn : public APawn
{
	GENERATED_BODY()

	/* The mesh component */
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* ShipMeshComponent;

	/** The camera */
	//UPROPERTY(Category = Camera, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	//class UCameraComponent* CameraComponent;

	/** Camera boom positioning the camera above the character */
	//UPROPERTY(Category = Camera, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	//class USpringArmComponent* CameraBoom;

public:
	ANeonDashPawn();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Offset from the ships location to spawn projectiles */
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite )
	FVector GunOffset;
	
	/* How fast the weapon will fire */
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
	float FireRate;

	/* How fast the weapon will charge */
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
	float ChargeRate;

	/* The speed our ship moves around the level */
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
	float MoveSpeed;

	/* The speed our ship moves around the level */
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
	float DashOffest;

	/** Barrier Class to spawn when player is dashing */
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> DashBarrierClass;

	/* The maximum number of barriers that are allowed to be present simultaneously during the gameplay */
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
	int MaxBarriersAllowed;

	/* The cooldown to spawn a new barrier */
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
	float NewBarrierCooldown;

	/** Sound to play each time we fire */
	UPROPERTY(Category = Audio, EditAnywhere, BlueprintReadWrite)
	class USoundBase* FireSound;

	// Begin Actor Interface
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End Actor Interface

	/* Fire a shot in the specified direction */
	void FireShot(FVector FireDirection);

	/* Handler for the fire timer expiry */
	void ShotTimerExpired();

	/* Handler for the charge timer expiry */
	void ChargeTimerExpired();

	/* Handler for the dash timer expiry */
	void DashTimerExpired();

	// Static names for axis bindings
	static const FName MoveForwardBinding;
	static const FName MoveRightBinding;
	static const FName FireForwardBinding;
	static const FName FireRightBinding;

	static const FName FireActionBinding;
	static const FName DashBinding;

private:

	/* Flag to control firing  */
	uint32 bCanFire : 1;

	/* Flag to control dash  */
	uint32 bCanDash : 1;

	/* Bool to check if fire is charging  */
	bool bIsFireCharging;

	size_t chargeMultiplier;

	/** Handle for the management of the charging shot fire */
	FTimerHandle TimerHandle_ChargeTimer;
	/** Handle for efficient management of ShotTimerExpired timer */
	FTimerHandle TimerHandle_ShotTimerExpired;
	/** Handle for management of dash cooldown */
	FTimerHandle TimerHandle_DashTimer;
	void OnPawnDash();
	void OnFireActionPressed();
	void OnFireActionReleased();
	void SetChargeValues(int chargeValue);

	TQueue<AActor*> SpawnedBarriers;

	FVector PrevFireDirection;

	float MoveSpeedInit;

public:

	class UMaterialInstanceDynamic* BodyShipMaterialInstance;
	class UMaterialInstanceDynamic* Bump1ShipMaterialInstance;
	class UMaterialInstanceDynamic* Bump2ShipMaterialInstance;
	class UMaterialInstanceDynamic* Bump3ShipMaterialInstance;
	class UMaterialInstanceDynamic* CoreShipMaterialInstance;

	/** Returns ShipMeshComponent subobject **/
	FORCEINLINE class UStaticMeshComponent* GetShipMeshComponent() const { return ShipMeshComponent; }
	/** Returns CameraComponent subobject **/
	//FORCEINLINE class UCameraComponent* GetCameraComponent() const { return CameraComponent; }
	/** Returns CameraBoom subobject **/
	//FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	void InitShipMaterial(UMaterialInterface* PlayerMaterial);
};

