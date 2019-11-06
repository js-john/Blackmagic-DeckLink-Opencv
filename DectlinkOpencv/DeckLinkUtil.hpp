//
//  DeckLinkUtil.hpp
//  DectlinkOpencv
//
//  Created by John Smith on 2019/10/12.
//  Copyright © 2019 John Smith. All rights reserved.
//

#ifndef DeckLinkUtil_hpp
#define DeckLinkUtil_hpp

#include <DeckLinkAPI.h>
#include <stdio.h>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv/highgui.h>
#include <mutex>

typedef int BMDCheckDisplayModeStatus;
enum _BMDCheckDisplayModeStatus {
   bmdCheckDisplayModeStatusNoSignal = -1,
   bmdCheckDisplayModeStatusOK = 1,
   bmdCheckDisplayModeStatusChecking = 0
};

class DeckLinkUtil: public IDeckLinkInputCallback {
public:
    DeckLinkUtil();
   
    int startCaptureWithDisplayMode(int mode);
    int startCapture();
    
    virtual HRESULT             VideoInputFormatChanged (/* in */ BMDVideoInputFormatChangedEvents notificationEvents, /* in */ IDeckLinkDisplayMode *newDisplayMode, /* in */ BMDDetectedVideoInputFormatFlags detectedSignalFlags);
    virtual HRESULT             VideoInputFrameArrived (/* in */ IDeckLinkVideoInputFrame* videoFrame, /* in */ IDeckLinkAudioInputPacket* audioPacket);
    virtual HRESULT             QueryInterface (REFIID iid, LPVOID *ppv);
    virtual ULONG               AddRef();
    virtual ULONG               Release();
    cv::Mat capture();
    bool supportAutoVideoModeDetection = false;
    
private:
    std::vector<IDeckLinkDisplayMode *> displayModeList;
    IDeckLinkDisplayMode *selectedDisplayMode = NULL;
    IDeckLinkInput *input;
    IDeckLink *instance;
    IDeckLinkProfileAttributes *profileAttributes;
    BMDCheckDisplayModeStatus displayModeCheckStatus = bmdCheckDisplayModeStatusNoSignal;
    IDeckLinkIterator *deckLinkIterator;
    cv::Mat frame;
    std::mutex mtx, statusMtx;
    
    
};


#endif /* DeckLinkUtil_hpp */
