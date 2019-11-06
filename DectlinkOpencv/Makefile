xx = g++
BMD_SDK = /usr/local/include/blackmagic/Linux/include
source = *.cpp $(BMD_SDK)/DeckLinkAPIDispatch.cpp
prom = decklink_opencv
libs = -lopencv_core -lopencv_highgui -lopencv_imgproc -lpthread -lstdc++ -ldl -lDeckLinkAPI
decklink_opencv: $(source)
	$(xx) -o $(prom) -Wall -std=c++11 $(source) -I $(BMD_SDK) -L /usr/local/lib $(libs) -O3