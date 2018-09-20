// Fill out your copyright notice in the Description page of Project Settings.

#include "ActorsController.h"
#include "Drone_follower.h"
#include "Messages.h"
#include "CustomData.h"
#include "EngineUtils.h"
#include "Ball.h"
#include "Drone_followerPawn.h"


// Sets default values
AActorsController::AActorsController()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    //Istantiate Communication Component
    OurCommunicationComponent = CreateDefaultSubobject<UUDP_Component>(TEXT("Communication Component"));

    //ScreenMsg("NEW ACTOR CONTROLLER");
}

// Called when the game starts or when spawned
void AActorsController::BeginPlay()
{
	Super::BeginPlay();
    
    for (TObjectIterator<ABall> Itr; Itr; ++Itr) {
        if (Itr->IsA(ABall::StaticClass())) {
            Ball = *Itr;
            //ScreenMsg("[Controller] found ball");
            GLog->Log("[Controller] found ball");
        }
    }
    
    for (TObjectIterator<ADrone_followerPawn> Itr; Itr; ++Itr) {
        if (Itr->IsA(ADrone_followerPawn::StaticClass())) {
            Drone = *Itr;
            //ScreenMsg("[Controller] found drone controller");
            GLog->Log("[Controller] found drone controller");
        }
    }
}

// Called every frame
void AActorsController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Get Data
	//OurCommunicationComponent->GetData(&ReceivedData);	//Store in a local variable the received data
    if(OurCommunicationComponent->CheckForData() == true){
        //If there are new data to update position do that
        ReceivedData = OurCommunicationComponent->GetData();
        
        FString debug;
        
        debug = "[ACTOR]: Received X: " + FString::SanitizeFloat(ReceivedData.drone_X) + " Y: " +  FString::SanitizeFloat(ReceivedData.drone_Y) + " Z: " +  FString::SanitizeFloat(ReceivedData.drone_Z);
        
        //GLog->Log(debug);
        
        FVector BallPos = FVector(ReceivedData.ball_X, ReceivedData.ball_Y, ReceivedData.ball_Z);
        
        Ball->SetPose(BallPos);
        //Ball->SetActorLocation(BallPos);
        
        FVector Position = FVector(ReceivedData.drone_X, ReceivedData.drone_Y, ReceivedData.drone_Z);
        FRotator Rotation;
        
        Rotation.Roll = ReceivedData.drone_Roll;
        Rotation.Pitch = ReceivedData.drone_Pitch;
        Rotation.Yaw = ReceivedData.drone_Yaw;
        
        Drone->SetPose(Rotation, Position);
    }else{
        //GLog->Log("No new data");
    }
}

void AActorsController::ReturnNewData(FCustomPoseData *NewData, FString ClassName) {
	
    
    FString debug;
    
    debug = ClassName + " [RECEIVED DATA] Received X: " + FString::SanitizeFloat(ReceivedData.ball_X) + " Y: " +  FString::SanitizeFloat(ReceivedData.ball_Y) + " Z: " +  FString::SanitizeFloat(ReceivedData.ball_Z);
    
    GLog->Log(debug);
    
    *NewData = ReceivedData;
    
    debug = ClassName + " [NEW DATA]: Received X: " + FString::SanitizeFloat(NewData->ball_X) + " Y: " +  FString::SanitizeFloat(NewData->ball_Y) + " Z: " +  FString::SanitizeFloat(NewData->ball_Z);
    
    GLog->Log(debug);
    
    
    
}

void AActorsController::PreInitializeComponents() {
	Super::PreInitializeComponents();
	OurCommunicationComponent->StartUDPComm("PawnCommunicationComponent");
}

