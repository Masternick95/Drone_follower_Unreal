// Fill out your copyright notice in the Description page of Project Settings.

#include "Ball.h"
#include "Messages.h"
#include "CustomData.h"
#include "ActorsController.h"
#include "Drone_follower.h"


// Sets default values
ABall::ABall()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetActorLocation(FVector(-170.0f, -270.0f, 630.0f));

	//Instantiate static mesh component
	BallVisibleComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BallVisibleComponent"));
    //ScreenMsg("Ball radius: ", BallVisibleComponent->Bounds.SphereRadius);

	//Get actor controller pointer
	//Get ActorsController pointer for receiving position
	/*for (TObjectIterator<AActorsController> Itr; Itr; ++Itr) {
		if (Itr->IsA(AActorsController::StaticClass())) {
			ActorController = *Itr;
            ScreenMsg("[BALL] found actor controller");
		}
	}*/
}

// Called when the game starts or when spawned
void ABall::BeginPlay()
{
	Super::BeginPlay();
	//SetActorLocation(FVector(0.0f, 0.0f, 0.0f));
}

// Called every frame
void ABall::Tick(float DeltaTime)
{
	FCustomPoseData NewData;

	Super::Tick(DeltaTime);
	
	//ActorController->ReturnNewData(&NewData, "ABALL");
    
    //ScreenMsg("New data ball X = ", NewData.ball_X);
    //UE_LOG(LogClass, Log, "New data ball X = " + FString::SanitizeFloat(NewData.ball_X));
    
	//SetPose(&NewData);
}

void ABall::SetPose(FVector Position){
	/*FVector Position;

	//Received measures are in cm --> convert into m
	Position.X = ReceivedData->ball_X;
	Position.Y = ReceivedData->ball_Y;
	Position.Z = ReceivedData->ball_Z;*/

    FString debug;
    
    debug = "POSITION X BALL: " +  FString::SanitizeFloat(Position.X);
    
    //GLog->Log(debug);
    
    //SetActorLocation(Position);
    
    FQuat Rotation;
    
    SetActorLocationAndRotation(Position, Rotation, false, 0, ETeleportType::TeleportPhysics);
}
