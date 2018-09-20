// Fill out your copyright notice in the Description page of Project Settings.

#include "MySceneCaptureComponent2D.h"
#include "UDP_Component.h"
#include "Messages.h"
#include "Runtime/Engine/Classes/Engine/TextureRenderTarget2D.h"
#include "Runtime/Core/Public/Misc/DateTime.h"

int UMySceneCaptureComponent2D::global_id = 0;

UMySceneCaptureComponent2D::UMySceneCaptureComponent2D(){
    stream_on = true;
    camera_name = FString("GimbalCamera");
    camera_id = ++global_id;
    PrimaryComponentTick.bCanEverTick = false;    //Disable tick every frame
    toggle_bit = false;
    
    myUDPSocket = CreateDefaultSubobject<UUDP_Component>(TEXT("SceneCommunicationComponent"));
}

void UMySceneCaptureComponent2D::InitCommunication(){
    myUDPSocket->StartUDPComm("CameraCommunicationComponent");
}

void UMySceneCaptureComponent2D::CaptureMyScene(){
    //ScreenMsg("CaptureMyScene");
    if(stream_on){
        //ScreenMsg("streamCamera");
        //if(myUDPSocket->SendImg() == true){
            //I have to send an image
            //GLog->Log("Starting send IMG");
            streamCamera();
            GLog->Log("IMG Sent");
            //myUDPSocket->ImgSent();
        //}
        
        // IF YOU WANT TO SEND ONLY ONE PHOTO THEN UNCOMMENT THIS
        //stream_on = false;
    }
}

void UMySceneCaptureComponent2D::enableStream(){
    stream_on = true;
}

void UMySceneCaptureComponent2D::toggleStream(){
    stream_on = !stream_on;    //Set off if is on and vice-versa
}

bool UMySceneCaptureComponent2D::isStreamEnabled(){
    return stream_on;
}

void UMySceneCaptureComponent2D::disableStream(){
    stream_on = false;
}

void UMySceneCaptureComponent2D::streamCamera(){
    UTextureRenderTarget2D* renderTarget = this->TextureTarget;
    TArray<FColor> RawPixels;
    
    //ScreenMsg("Check renderTarget");
    if(renderTarget == nullptr){
        //ScreenMsg("renderTarget");
        return;
    }else{
        //ScreenMsg("renderTarget not nullptr");
    }
    
    FRenderTarget* TargetResource;
    //Save image from RenderTarget
    TargetResource = renderTarget->GameThread_GetRenderTargetResource();
    if(TargetResource == nullptr){
        //ScreenMsg("TargetResource");
        return;
    }else{
        //ScreenMsg("TargetResource not nullptr");
    }
    
    bool bCopyValid = ReadTargetToBufferValidated(RawPixels, TargetResource);
    if(bCopyValid == false){
        //ScreenMsg("bCopyValid");
        return;
    }else{
        //ScreenMsg("bCopyValid not false");
    }
    
    //ScreenMsg("Ready to sendRaw");
    sendRaw(renderTarget, RawPixels);
}

//BMP VERSION
void UMySceneCaptureComponent2D::sendRaw(UTextureRenderTarget2D* renderTarget, TArray<FColor> RawPixels){
    const int32 Width = renderTarget->SizeX;
    const int32 Height = renderTarget->SizeY;
    int i, count;    //Counters
    TArray<uint8> bytetosend;
    FString r0, r1, r2, r3, header;    //Header building strings
    
    r0 = FString("data:raw;");
    r1 = FString::Printf(TEXT("width:%d;"), Width);
    r2 = FString::Printf(TEXT("height:%d;"), Height);
    r3 = FString::Printf(TEXT("size:%d;"), RawPixels.Num() * 3);
    
    header = (r0 + r1 + r2+ r3);
    
    myUDPSocket->SendData(TArray<uint8>(header.GetCharArray()));    //Send header
    
    bytetosend.Empty();
    count = 0;
    //ScreenMsg("Image to send");
    for(i = 0; i < RawPixels.Num(); i++){    //Send each pixel
        //For each pixel I have to send the three RGB components
        bytetosend.Add(RawPixels[i].R);
        bytetosend.Add(RawPixels[i].G);
        bytetosend.Add(RawPixels[i].B);
        
        count++;
        //The maximum packet size I can send is 60kbyte, each pixel is 8bits(1 byte) for color so is 24bits (3 byte). So I manually split into packets of less than 60kbyte
        if(count > 1999){    //1999
            myUDPSocket->SendData(bytetosend);
            bytetosend.Empty();
            count = 0;
            //ScreenMsg("Packet sent");
        }
    }
    if (count > 0) {
        myUDPSocket->SendData(bytetosend);
        bytetosend.Empty();
        count = 0;
        //ScreenMsg("Packet sent");
    }
    
    //ScreenMsg("Image sent");
    //DEBUG:
    //FString data = FString("TEST PACKET");
    //myUDPSocket->SendData(TArray<uint8>(data.GetCharArray()));
}

//JPEG VERSION
/* void UMySceneCaptureComponent2D::sendRaw(UTextureRenderTarget2D* renderTarget, TArray<FColor> RawPixels){
 const int32 Width = renderTarget->SizeX;
 const int32 Height = renderTarget->SizeY;
 int SizePayload = 0;
 int NPack = 0;
 int i, k;
 TArray<uint8> bytetosend;
 
 IImageWrapperPtr ImageWrapper = NULL;
 IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("Image Wrapper"));
 
 ImageWrapper = IImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG);
 
 if(ImageWrapper.IsValid()){
 FDateTime DateTime = FDateTime::UtcNow();
 int32 Time_ms = DateTime.GetMillisecond();
 int64 Time_full = DateTime.ToUnixTimestamp() * 1000.0 + Time_ms;
 
 //TOGGLE BIT
 //Flag used by the blueprint managing the hud (Box for the photodiode measurement)
 toggle_bit = !toggle_bit;    //Invert the bit value
 
 if(ImageWrapper->SetRaw(RawPixels.GetData(), RawPixels.Num() * sizeof(FColor), Width, Height, ERGBFormat::RGBA, 8)){
 TArray<uint8> OutData = ImageWrapper->GetCompressed();
 
 SizePayload = OutData.Num();
 
 //Send header
 FString r = FString::Printf(TEXT("size:"));
 bytetosend.Empty();
 bytetosend = (TArray<uint8>)r.GetCharArray();
 
 bytetosend.Add(*((uint8_t*)(&SizePayload)));
 bytetosend.Add(*((uint8_t*)(&SizePayload) + 1));
 bytetosend.Add(*((uint8_t*)(&SizePayload) + 2));
 bytetosend.Add(*((uint8_t*)(&SizePayload) + 3));
 
 for(i = 0; i < 8; i++){
 uint8_t byte2send = *((uint8_t*)(&Time_full) + i);
 bytetosend.Add(byte2send);
 }
 
 myUDPSocket->SendData(bytetosend);
 
 //Send Payload
 if(SizePayload > MAXPAYLOAD){    //I have to divide the payload into multiple packets
 NPack = SizePayload / MAXPAYLOAD;    //Number of packets to send
 
 for(i = 0; i < NPack; i++){    //Send each packet
 bytetosend.Empty();
 for(k = i * MAXPAYLOAD; k < (i + 1) * MAXPAYLOAD; k++){
 bytetosend.Add(OutData[k]);
 }
 myUDPSocket->SendData(bytetosend);
 }
 
 //Send the tail
 bytetosend.Empty();
 for(k  = NPack * MAXPAYLOAD; k < SizePayload; k++){
 bytetosend.Add(OutData[k]);
 }
 myUDPSocket->SendData(bytetosend);
 }else{
 myUDPSocket->SendData(OutData);
 }
 }
 
 }
 } */

UFUNCTION(BlueprintCallable, Category = "Toggle")
bool UMySceneCaptureComponent2D::getToggleBit(){
    return toggle_bit;
}

/*void UMySceneCaptureComponent2D::SavePixelBufferToFile(IImageWrapperPtr p, int i){
 //Save Texture to jpg
 FString Index = FString::Printf(TEXT("%d"), i);
 FString GameDir = FPaths::ConvertRelativePathToFull(FPaths::GameDir());
 FString PicsDir = FPaths::Combine(*GameDir, TEXT("PhonePics"));
 FString Filename = TEXT("DC-") + Index + TEXT("-") + FGuid::NewGuid().ToString();
 
 FString FileLocation = FPaths::Combine(*PicsDir, *Filename);
 FileLocation += ".jpg";    //Add file extension
 
 FFileHelper::SaveArrayToFile(p->GetCompressed(), *FileLocation);
 }*/

bool UMySceneCaptureComponent2D::ReadTargetToBufferValidated(TArray<FColor>& pPixelBuffer, FRenderTarget* pTargetResource){
    if(!pTargetResource->ReadPixels(pPixelBuffer)){
        return false;
    }else{
        for(FColor& Pixel : (pPixelBuffer)){
            // ^Confirmed - S.
            const uint8 R = Pixel.B;
            const uint8 B = Pixel.R;
            Pixel.R = R;
            Pixel.B = B;
            Pixel.A = 255;
        }
        return true;
    }
}
