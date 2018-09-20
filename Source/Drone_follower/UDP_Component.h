// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Networking.h"
#include "CustomData.h"
#include "Components/ActorComponent.h"
#include "UDP_Component.generated.h"


UCLASS(ClassGroup = (Communication))
class DRONE_FOLLOWER_API UUDP_Component : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Remote Address")
		FString SourceIP_Address;
	UPROPERTY(EditAnywhere, Category = "Remote Address")
		int32 PortIn;

	UPROPERTY(EditAnywhere, Category = "Remote Address")
		FString DestIP_Address;
	UPROPERTY(EditAnywhere, Category = "Remote Address")
		int32 PortOut;
		
public:	
	// Sets default values for this component's properties
	UUDP_Component();
	
	// Start the communication
	bool StartUDPComm(const FString& YourChosenSocketName);

	// Function called by the receiving thread
	void Recv(const FArrayReaderPtr& ArrayReaderPtr, const FIPv4Endpoint& EndPt);

	// Retrieve Data
	//void GetData(FCustomPoseData* RetData);
    FCustomPoseData GetData();
    bool CheckForData();

	// Send Data
	int SendData(TArray<uint8> Array);
	int SendData(uint8* Data, int Nbytes);
    
    bool SendImg();
    void ImgSent();

	/** Called whenever this actor is being removed from a level */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:	
	
	// ----------------
	// UDP Socket stuff
	FSocket* ListenSocket;   // Input Socket
	FSocket* SendSocket;	 // Output Socket

	ISocketSubsystem* SocketSubsystem;

	FUdpSocketReceiver* UDPReceiver = nullptr;

	// Address of the remote machine
	TSharedPtr<FInternetAddr> RemoteAddr;

	// Received Data
	//TArray<uint8> DataIn;
	FCustomPoseData DataIn;
    
    bool ImgToSend;   //Flag for send img after receiving request
    
    FPThreadsCriticalSection newDataLock;   //Mutex for newData variable
    
    bool newData;   //Flag to signal new data received
    
    bool imgSocket;

	// Output Data
	TArray<uint8> DataOut;

	//protected:
	//	// Called when the game starts
	//	virtual void BeginPlay() override;

	//public:	
	//	// Called every frame
	//	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
};
