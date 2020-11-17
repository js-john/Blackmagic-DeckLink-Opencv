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
    DeckLinkUtil *util = new DeckLinkUtil(0);
    cout << "[x]Auto Detection" << endl;
    cout << "Please select a display mode:";
    string selection;
    cin >> selection;
    if (!(util -> supportAutoVideoModeDetection)) {
        cout << "Your device does not support auto display mode detection!" << endl;
    }
    if (selection.compare("x") == 0) {
        if (util -> startCapture() < 0) {
            cout << "error!" << endl;
            exit(0);
        }
    } else {
        if (util -> startCaptureWithDisplayMode(atoi(selection.data())) < 0) {
            cout << "error!" << endl;
            exit(0);
        }
    }
    
    cout << "press ESC to exit" << endl;
    while (true) {
        if (cv::waitKey(30) == 27) {
            break;
        }
        cv::Mat frame = util -> capture();
        if (!frame.empty()) {
             cv::imshow("monitor", frame);
        }
       
    }
    
    
    return 0;
}
