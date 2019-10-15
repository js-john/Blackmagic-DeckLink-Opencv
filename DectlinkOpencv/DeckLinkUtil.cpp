//
//  DeckLinkUtil.cpp
//  DectlinkOpencv
//
//  Created by John Smith on 2019/10/12.
//  Copyright © 2019 John Smith. All rights reserved.
//

#include "DeckLinkUtil.hpp"
using namespace std;


DeckLinkUtil::DeckLinkUtil(int id) {
    this -> id = id;
    deckLinkIterator = CreateDeckLinkIteratorInstance();
    IDeckLink *instance;
    HRESULT result = deckLinkIterator -> Next(&instance);
    cout << "deckLinkIterator -> Next(&instance) result: " << result << endl;
    CFStringRef name;
    instance -> GetDisplayName(&name);
    void *tempInput = nullptr;
    result = instance->QueryInterface(IID_IDeckLinkInput, &tempInput);
    input = (IDeckLinkInput *) tempInput;
    
    if (!input) {
        cout << "DeckLink device offline!" << endl;
        exit(0);
    }
    
    IDeckLinkDisplayModeIterator *displayModeIterator;
    result = input -> GetDisplayModeIterator(&displayModeIterator);
    IDeckLinkDisplayMode *displayMode;
    cout << "Supported display mode:" << endl;
    while (displayModeIterator->Next(&displayMode) == S_OK) {
        CFStringRef name;
        displayMode -> GetName(&name);
        cout << cfstring2cstring(name) << endl;
        displayModeList.push_back(displayMode);
    }
}

int DeckLinkUtil::startCaptureWithDisplayMode(int width, int height, float framerate, string port) {
    if (!input) {
        cout << "IDeckLinkInput NULL !" << endl;
        return -1;
    }
    IDeckLinkDisplayMode *displayMode = nullptr;
    for (int i = 0; i < displayModeList.size(); i++) {
        displayMode = displayModeList[i];
        long mode_width = displayMode -> GetWidth(), mode_height = displayMode -> GetHeight();
        BMDTimeValue frameDuration = -1, timeScale = -1;
        displayMode -> GetFrameRate(&frameDuration, &timeScale);
        if (mode_width == width && mode_height == height && (int)framerate * 1000 == timeScale) {
            break;
        } else {
            displayMode = nullptr;
        }
    }
    
    bool supported = false;
    if (displayMode) {
        BMDVideoConnection connection = 0;
           if (port.compare("HDMI") == 0) {
               connection = bmdVideoConnectionHDMI;
           } else if (port.compare("SDI") == 0) {
               connection = bmdVideoConnectionSDI;
           }
           input -> DoesSupportVideoMode(connection, displayMode -> GetDisplayMode(), bmdFormat8BitYUV, 0, &supported);
    }
    
    if (!supported) {
        cout << "Unsupported display mode：" << width << "x" << height << "@" << framerate << endl;
        return -1;
    } else {
        input -> EnableVideoInput(displayMode -> GetDisplayMode(), bmdFormat8BitYUV, bmdVideoInputFlagDefault);
    }
    
    input -> SetCallback(this);
    input -> StartStreams();
    return 0;
}

HRESULT        DeckLinkUtil::VideoInputFormatChanged (/* in */ BMDVideoInputFormatChangedEvents notificationEvents, /* in */ IDeckLinkDisplayMode *newMode, /* in */ BMDDetectedVideoInputFormatFlags detectedSignalFlags)
{
    cout << "VideoInputFormatChanged callback" << endl;
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


std::string DeckLinkUtil::cfstring2cstring(CFStringRef cfstring) {
    const CFIndex kCStringSize = 128;
    char temporaryCString[kCStringSize];
    bzero(temporaryCString,kCStringSize);
    CFStringGetCString(cfstring, temporaryCString, kCStringSize, kCFStringEncodingUTF8);
    std::string *bar = new std::string(temporaryCString);
    return *bar;
}

cv::Mat DeckLinkUtil::capture() {
    cv::Mat newFrame;
    if (mtx.try_lock()) {
        newFrame = this -> frame.clone();
    }
    mtx.unlock();
    return newFrame;
}
