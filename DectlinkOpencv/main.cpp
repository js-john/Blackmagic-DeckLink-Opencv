//
//  main.cpp
//  DectlinkOpencv
//
//  Created by John Smith on 2019/10/12.
//  Copyright © 2019 John Smith. All rights reserved.
//

#include <iostream>
#include <stdio.h>

#include "DeckLinkUtil.hpp"

using namespace std;

int main(int argc, const char * argv[]) {
    
    int width = 1920, height = 1080;
    float framerate = 30.0;
    string input = "HDMI";
    DeckLinkUtil util(0);
    util.startCaptureWithDisplayMode(width, height, framerate, input);

    while (cv::waitKey(1)) {
        cv::Mat frame = util.capture();
        if (!frame.empty()) {
             cv::imshow("monitor", frame);
        }
       
    }
    
    
    return 0;
}
