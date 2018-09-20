// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "Drone_followerPawn.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/World.h"

#include "UDP_Component.h"
#include "Messages.h"
#include "CustomData.h"
#include "MySceneCaptureComponent2D.h"
#include "ActorsController.h"
#include "Engine/StaticMesh.h"

ADrone_followerPawn::ADrone_followerPawn()
{
	Rotation = FRotator(0.0f, 0.0f, 0.0f);
	Position = FVector(-470.0f, -270.0f, 630.0f);
	OldPosition = FVector(0, 0, 0);
	
	// Set this pawn to call Tick() every frame.
	PrimaryActorTick.bCanEverTick = true;
    SetActorTickEnabled(true);
	
	// Create the movement component for interacting with the environment
	//MovementComponent = CreateDefaultSubobject<UMovComponent>(TEXT("MovementComponent"));
	//MovementComponent->UpdatedComponent = OurVisibleComponent;
	
	// Structure to hold one-time initialization
	struct FConstructorStatics
	{
		ConstructorHelpers::FObjectFinderOptional<UStaticMesh> PlaneMesh;
		FConstructorStatics()
			: PlaneMesh(TEXT("/Game/Flying/Meshes/UFO.UFO"))
		{
		}
	};
	static FConstructorStatics ConstructorStatics;

	// Create static mesh component and configure it as root component
	OurVisibleComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlaneMesh0"));
	OurVisibleComponent->SetStaticMesh(ConstructorStatics.PlaneMesh.Get());	// Set static mesh
	OurVisibleComponent->SetupAttachment(RootComponent);
	RootComponent = OurVisibleComponent;

	//Instantiate Spring Arm
	CameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArm"));
	
	//CAMERA COMPONENT
	//Instantiate Camera Component
	OnBoardCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("OnBoardCamera"));
	
	//Attach Spring Arm to root component
	CameraSpringArm->SetupAttachment(RootComponent);
	CameraSpringArm->SetRelativeLocation(FVector(-130.0f, 0.0f, 100.0f));
	CameraSpringArm->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	CameraSpringArm->TargetArmLength = 300.0f;
	// Decouple the some rotationale movement of the Pawn from the movement of the camera
	CameraSpringArm->bInheritRoll = false;
	CameraSpringArm->bInheritPitch = false;
	
	//Configure the onboard Spring Arm
	OnBoardSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("OnBoardSpringArm"));
	OnBoardSpringArm->SetupAttachment(RootComponent);
	OnBoardSpringArm->SetRelativeLocation(FVector(0.0f, 0.0f, 0.1f));
	OnBoardSpringArm->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	OnBoardSpringArm->TargetArmLength = 0.0f;
	OnBoardSpringArm->bInheritPitch = false;
	OnBoardSpringArm->bInheritRoll = false;
	
	//Configure onboard Camera
	OnBoardCamera->SetupAttachment(OnBoardSpringArm);
	OnBoardCamera->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	OnBoardCamera->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	
	//CAPTURE CAMERA
	//Instantiate Capture Camera
	CaptureCamera = CreateDefaultSubobject<UMySceneCaptureComponent2D>(TEXT("VideoFeedComponent"));
	
	//Configure Capture Cameras
	CaptureCamera->SetupAttachment(RootComponent);
	CaptureCamera->CaptureSource = SCS_FinalColorLDR;
	CaptureCamera->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	CaptureCamera->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));

	// Set handling parameters
	Acceleration = 500.f;
	TurnSpeed = 50.f;
	MaxSpeed = 4000.f;
	MinSpeed = 500.f;
	CurrentForwardSpeed = 500.f;
	
	// Instantiate the communication component to send out images
	//OurCommunicationComponent = CreateDefaultSubobject<UUDP_Component>(TEXT("PawnCommuncationComponent"));

	//Get ActorsController pointer for receiving position
	/*for (TObjectIterator<AActorsController> Itr; Itr; ++Itr) {
		if (Itr->IsA(AActorsController::StaticClass())) {
			ActorController = *Itr;
		}
	}*/
}

//Start communication component
void ADrone_followerPawn::PreInitializeComponents()
{
	Super::PreInitializeComponents();
	//ScreenMsg("Starting UDP Component");
	//OurCommunicationComponent->StartUDPComm("PawnCommunicationComponent");
	CaptureCamera->InitCommunication();
}

// Called when the game starts or when spawned
void ADrone_followerPawn::BeginPlay()
{
	Super::BeginPlay();

	if (GEngine)
	{
		//ScreenMsg("Drone Pawn Spawned");
	}
}

void ADrone_followerPawn::Tick(float DeltaSeconds)
{
	static int counter = 0;
	
	FVector DeltaMovement(0, 0, 0);
	FVector ImpactNormal(0, 0, 0);
	FVector ImpactPoint(0, 0, 0);
	float PenDepth = 0;

	// Call any parent class Tick implementation
	Super::Tick(DeltaSeconds);
	
	//GetPose(&Rotation, &Position);
	
	//DeltaMovement = Position - OldPosition;
	
	//Acquire image from camera and send through UDP
	CaptureCamera->CaptureMyScene();
	
	counter++;
	
	//OldPosition = Position;
}

void ADrone_followerPawn::NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	// Deflect along the surface when we collide.
	FRotator CurrentRotation = GetActorRotation();
	SetActorRotation(FQuat::Slerp(CurrentRotation.Quaternion(), HitNormal.ToOrientationQuat(), 0.025f));
}


void ADrone_followerPawn::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);
	
	/*
    // Check if PlayerInputComponent is valid (not NULL)
	check(PlayerInputComponent);

	// Bind our control axis' to callback functions
	PlayerInputComponent->BindAxis("Thrust", this, &ADrone_followerPawn::ThrustInput);
	PlayerInputComponent->BindAxis("MoveUp", this, &ADrone_followerPawn::MoveUpInput);
	PlayerInputComponent->BindAxis("MoveRight", this, &ADrone_followerPawn::MoveRightInput);
	*/
}

void ADrone_followerPawn::GetPose(FRotator* Att, FVector* Pos)
{
	//int i;
	//TArray<uint8> ReceivedData;

	// Get Data
	/*OurCommunicationComponent->GetData(&ReceivedData);

	// Serialize the data into the PoseData Structure
	for (i = 0; i < sizeof(struct FCustomPoseData); i++)
	{
		*((uint8*)(&VehiclePose) + i) = ReceivedData[i];
	}*/

	FCustomPoseData RecvdData;

	//ActorController->ReturnNewData(&RecvdData, "ADRONE");

	// Take the measures in Radiants and convert them into degree 
	Att->Roll = RecvdData.drone_Roll * 180 / PI;
	Att->Pitch = RecvdData.drone_Pitch * 180 / PI;
	Att->Yaw = RecvdData.drone_Yaw * 180 / PI;

	// Take the measures in Meters and convert them in centimeters
	Pos->X = 100 * RecvdData.drone_X;
	Pos->Y = 100 * RecvdData.drone_Y;
	Pos->Z = -100 * RecvdData.drone_Z;

    //ScreenMsg("X: ",RecvdData.ball_X);
	/*
	Rotation->Roll = 0;
	Rotation->Pitch = 0;
	Rotation->Yaw = 0;

	Position->X = 950;
	Position->Y = 1200;
	Position->Z = 500;
	*/
}

void ADrone_followerPawn::SetPose(FRotator Rotation, FVector Position){
    
    SetActorLocationAndRotation(Position, Rotation, false, 0, ETeleportType::TeleportPhysics);
    
    //SetActorLocation(Position);
    //SetActorRelativeRotation(Rotation.Quaternion());
}

void ADrone_followerPawn::ThrustInput(float Val)
{
	// Is there any input?
	bool bHasInput = !FMath::IsNearlyEqual(Val, 0.f);
	// If input is not held down, reduce speed
	float CurrentAcc = bHasInput ? (Val * Acceleration) : (-0.5f * Acceleration);
	// Calculate new speed
	float NewForwardSpeed = CurrentForwardSpeed + (GetWorld()->GetDeltaSeconds() * CurrentAcc);
	// Clamp between MinSpeed and MaxSpeed
	CurrentForwardSpeed = FMath::Clamp(NewForwardSpeed, MinSpeed, MaxSpeed);
}

void ADrone_followerPawn::MoveUpInput(float Val)
{
	// Target pitch speed is based in input
	float TargetPitchSpeed = (Val * TurnSpeed * -1.f);

	// When steering, we decrease pitch slightly
	TargetPitchSpeed += (FMath::Abs(CurrentYawSpeed) * -0.2f);

	// Smoothly interpolate to target pitch speed
	CurrentPitchSpeed = FMath::FInterpTo(CurrentPitchSpeed, TargetPitchSpeed, GetWorld()->GetDeltaSeconds(), 2.f);
}

void ADrone_followerPawn::MoveRightInput(float Val)
{
	// Target yaw speed is based on input
	float TargetYawSpeed = (Val * TurnSpeed);

	// Smoothly interpolate to target yaw speed
	CurrentYawSpeed = FMath::FInterpTo(CurrentYawSpeed, TargetYawSpeed, GetWorld()->GetDeltaSeconds(), 2.f);

	// Is there any left/right input?
	const bool bIsTurning = FMath::Abs(Val) > 0.2f;

	// If turning, yaw value is used to influence roll
	// If not turning, roll to reverse current roll value.
	float TargetRollSpeed = bIsTurning ? (CurrentYawSpeed * 0.5f) : (GetActorRotation().Roll * -2.f);

	// Smoothly interpolate roll speed
	CurrentRollSpeed = FMath::FInterpTo(CurrentRollSpeed, TargetRollSpeed, GetWorld()->GetDeltaSeconds(), 2.f);
}
