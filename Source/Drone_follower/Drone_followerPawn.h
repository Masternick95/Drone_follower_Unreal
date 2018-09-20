// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CustomData.h"
#include "UDP_Component.h"
//#include "ActorsController.h"
//#include "MovComponent.h"
#include "Drone_followerPawn.generated.h"

UCLASS(Config=Game)
class ADrone_followerPawn : public APawn
{
	GENERATED_BODY()
	
public:
	ADrone_followerPawn();
	
	// Visual Component
	UPROPERTY(EditAnywhere, Category = "Geometry")
		UStaticMeshComponent* OurVisibleComponent;
	
	//Called when game starts or spawned
	virtual void BeginPlay() override;

	//Called every frame
	virtual void Tick(float DeltaSeconds) override;
	
	//Bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override; // Allows binding actions/axes to functions
	
	virtual void NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;
	
	//Initialize communication components
	virtual void PreInitializeComponents() override;
	
	// Get the Pose from the Data received and return it in the correspondend vectors
	// Position is in [cm]
	// Rotation is in [degrees]
	void GetPose(FRotator* Rotation, FVector* Position);
    
    void SetPose(FRotator Rotation, FVector Position);
	
protected:

	/** Bound to the thrust axis */
	void ThrustInput(float Val);
	
	/** Bound to the vertical axis */
	void MoveUpInput(float Val);

	/** Bound to the horizontal axis */
	void MoveRightInput(float Val);

private:
	//Camera component
	UPROPERTY(EditAnywhere, Category = "Camera")
	class UCameraComponent* OnBoardCamera;
	
	//Streaming camera component
	UPROPERTY(EditAnywhere, Category = "Camera")
	class UMySceneCaptureComponent2D* CaptureCamera;
		
	//Spring arm
	class USpringArmComponent* CameraSpringArm;
	class USpringArmComponent* OnBoardSpringArm;
	
	// Communication Component
	//UPROPERTY(EditAnywhere, Category = "Remote Address")
	//class UUDP_Component* OurCommunicationComponent;
	
	// Movement Component to take 
	//class UMovComponent* MovementComponent;

	// Position and Orientation of the Pawn
	FRotator Rotation;
	FVector Position;
	FVector OldPosition;

	FCustomPoseData VehiclePose;
	FCustomImpactData VehicleImpact;

	//AActorsController* ActorController;
	
	/** How quickly forward speed changes */
	UPROPERTY(Category=Plane, EditAnywhere)
	float Acceleration;

	/** How quickly pawn can steer */
	UPROPERTY(Category=Plane, EditAnywhere)
	float TurnSpeed;

	/** Max forward speed */
	UPROPERTY(Category = Pitch, EditAnywhere)
	float MaxSpeed;

	/** Min forward speed */
	UPROPERTY(Category=Yaw, EditAnywhere)
	float MinSpeed;

	/** Current forward speed */
	float CurrentForwardSpeed;

	/** Current yaw speed */
	float CurrentYawSpeed;

	/** Current pitch speed */
	float CurrentPitchSpeed;

	/** Current roll speed */
	float CurrentRollSpeed;

public:
	/** Returns PlaneMesh subobject **/
	//FORCEINLINE class UStaticMeshComponent* GetPlaneMesh() const { return PlaneMesh; }
	/** Returns SpringArm subobject **/
	//FORCEINLINE class USpringArmComponent* GetSpringArm() const { return SpringArm; }
	/** Returns Camera subobject **/
	//FORCEINLINE class UCameraComponent* GetCamera() const { return Camera; }
};
