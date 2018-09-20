// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UDP_Component.h"
#include "IImageWrapperModule.h"
#include "Components/SceneCaptureComponent2D.h"
#include "MySceneCaptureComponent2D.generated.h"

//#define MAXPAYLOAD 65500

/**
 * 
 */
UCLASS()
class DRONE_FOLLOWER_API UMySceneCaptureComponent2D : public USceneCaptureComponent2D
{
	GENERATED_BODY()
	
	bool stream_on;
	FString camera_name;
	static int global_id;
	int camera_id;
	
	void sendRaw(UTextureRenderTarget2D* renderTarget, TArray<FColor> RawPixels);
	
	bool toggle_bit;
	
public:
	UMySceneCaptureComponent2D();
	
	void InitCommunication();
	void CaptureMyScene();
	void enableStream();
	void disableStream();
	void toggleStream();
	bool isStreamEnabled();
	
	//void SavePixelBufferToFile(IImageWrapperPtr p, int i);
	void streamCamera();
	
	bool ReadTargetToBufferValidated(TArray<FColor>& pPixelBuffer, FRenderTarget* pTargetResource);
	
	UFUNCTION(BlueprintCallable, Category="Toggle")
	bool getToggleBit();
	
	//Communication Component 
	UPROPERTY(EditAnywhere, Category = "Remote Address")
	class UUDP_Component* myUDPSocket;
	
	
};
