xx = g++
BMD_SDK = /Users/js/env/bmd_sdk/Mac/include
OPENCV = /usr/local/Cellar/opencv/4.4.0_1/include/opencv4
source = *.cpp $(BMD_SDK)/DeckLinkAPIDispatch.cpp
prom = decklink_opencv
libs = -lopencv_core -lopencv_highgui -lopencv_imgproc -lpthread -lstdc++ -ldl -lDeckLinkAPI
decklink_opencv: $(source)
	$(xx) -o $(prom) -Wall -std=c++11 $(source) -I $(BMD_SDK) -I$(OPENCV) -L /usr/local/lib $(libs) -O3