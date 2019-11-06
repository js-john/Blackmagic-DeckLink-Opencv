//
//  DeckLinkUtil.cpp
//  DectlinkOpencv
//
//  Created by John Smith on 2019/10/12.
//  Copyright © 2019 John Smith. All rights reserved.
//

#include "DeckLinkUtil.hpp"
using namespace std;


DeckLinkUtil::DeckLinkUtil() {
    deckLinkIterator = CreateDeckLinkIteratorInstance();
    deckLinkIterator -> Next(&instance);
   
    const char *name = (char *) malloc(sizeof(char) * 20);
    if (!instance) {
        cout << "Decklink Device offline!" << endl;
        exit(0);
    }
    #ifdef __APPLE__
            CFStringRef cfname;
            instance -> GetDisplayName(&cfname);
            name = CFStringGetCStringPtr(cfname, kCFStringEncodingUTF8);
    #endif
    
#ifdef linux
    instance -> GetDisplayName(&name);
#endif
    printf("Device online: %s\n", name);
    void *tempInput = nullptr;
    instance->QueryInterface(IID_IDeckLinkInput, &tempInput);
    input = (IDeckLinkInput *) tempInput;
    
    if (!input) {
        cout << "DeckLink device input error!" << endl;
        exit(0);
    }
    
    IDeckLinkDisplayModeIterator *displayModeIterator;
    input -> GetDisplayModeIterator(&displayModeIterator);
    IDeckLinkDisplayMode *displayMode;
    cout << "Supported display mode:" << endl;
    int index = 0;
    while (displayModeIterator->Next(&displayMode) == S_OK) {
        cout << "[" << index << "]";
        const char *name = (char *) malloc(sizeof(char) * 20);
#ifdef __APPLE__
        CFStringRef cfname;
        displayMode -> GetName(&cfname);
        name = CFStringGetCStringPtr(cfname, kCFStringEncodingUTF8);
#endif
#ifdef linux
       displayMode -> GetName(&name);
#endif
        printf("%s\n", name);
        index++;
        displayModeList.push_back(displayMode);
    }
    void *tempProfileAttributes = nullptr;
    instance -> QueryInterface(IID_IDeckLinkProfileAttributes, &tempProfileAttributes);
    profileAttributes = (IDeckLinkProfileAttributes *) tempProfileAttributes;
    if (!profileAttributes) {
        cout << "Get ProfileAttributes Failed!" << endl;
    } else {
        profileAttributes -> GetFlag(BMDDeckLinkSupportsInputFormatDetection, &supportAutoVideoModeDetection);
    }
    
}

int DeckLinkUtil::startCapture() {
    if (!input) {
       cout << "IDeckLinkInput NULL !" << endl;
       return -1;
    }
    
    input -> SetCallback(this);
    input -> EnableVideoInput(displayModeList[0] -> GetDisplayMode(), bmdFormat8BitYUV, bmdVideoInputEnableFormatDetection);
    input -> StartStreams();

    return 0;
}

int DeckLinkUtil::startCaptureWithDisplayMode(int mode) {
    if (!input) {
        cout << "IDeckLinkInput NULL !" << endl;
        return -1;
    }
    if (mode < 0 || mode >= displayModeList.size()) {
        return -1;
    }
    IDeckLinkDisplayMode *displayMode = displayModeList[mode];
    
    input -> EnableVideoInput(displayMode -> GetDisplayMode(), bmdFormat8BitYUV, bmdVideoInputFlagDefault);
    input -> SetCallback(this);
    input -> StartStreams();
    return 0;
}



HRESULT        DeckLinkUtil::VideoInputFormatChanged (/* in */ BMDVideoInputFormatChangedEvents notificationEvents, /* in */ IDeckLinkDisplayMode *newDisplayMode, /* in */ BMDDetectedVideoInputFormatFlags detectedSignalFlags)
{
    const char *name = (char *) malloc(sizeof(char) * 20);
#ifdef __APPLE__
    CFStringRef cfname;
    newDisplayMode -> GetName(&cfname);
    name = CFStringGetCStringPtr(cfname, kCFStringEncodingUTF8);
#endif
#ifdef linux
    newDisplayMode -> GetName(&name);
#endif
    printf("Video Input Format Changed: %s", name);
    input -> PauseStreams();
    input -> EnableVideoInput(newDisplayMode -> GetDisplayMode(), bmdFormat8BitYUV, bmdVideoInputEnableFormatDetection);
    input -> FlushStreams();
    input -> StartStreams();
    return S_OK;
}

HRESULT     DeckLinkUtil::VideoInputFrameArrived (/* in */ IDeckLinkVideoInputFrame* videoFrame, /* in */ IDeckLinkAudioInputPacket* audioPacket)
{
        if (mtx.try_lock()) {
            void *data;
            videoFrame -> GetBytes(&data);
            const int width = (int) videoFrame -> GetWidth();
            const int height = (int) videoFrame -> GetHeight();
            cv::Mat uyvy(height, width, CV_8UC2, data);
            cv::Mat image(height, width, CV_8UC3);
            cv::cvtColor(uyvy, image, cv::COLOR_YUV2BGR_UYVY);
            frame = image.clone();
            mtx.unlock();
        }
    return S_OK;
}


HRESULT         DeckLinkUtil::QueryInterface (REFIID iid, LPVOID *ppv)
{
    CFUUIDBytes        iunknown;
    HRESULT            result = E_NOINTERFACE;
    
    // Initialise the return result
    *ppv = NULL;
    
    // Obtain the IUnknown interface and compare it the provided REFIID
    iunknown = CFUUIDGetUUIDBytes(IUnknownUUID);
    if (memcmp(&iid, &iunknown, sizeof(REFIID)) == 0)
    {
        *ppv = this;
        AddRef();
        result = S_OK;
    }
    else if (memcmp(&iid, &IID_IDeckLinkNotificationCallback, sizeof(REFIID)) == 0)
    {
        *ppv = (IDeckLinkNotificationCallback*)this;
        AddRef();
        result = S_OK;
    }
    
    return result;
}


ULONG       DeckLinkUtil::AddRef (void)
{
    return 0;
}

ULONG       DeckLinkUtil::Release (void)
{
    return 0;
}


cv::Mat DeckLinkUtil::capture() {
    cv::Mat newFrame;
    if (mtx.try_lock()) {
        newFrame = this -> frame.clone();
    }
    mtx.unlock();
    return newFrame;
}
