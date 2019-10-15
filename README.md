# Blackmagic-DeckLink-Opencv
This is an example of integrating the BlackMagicDesign SDK(for macOS) into an Opencv project.  
The BlackMagicDesign SDK can be found here: http://www.blackmagicdesign.com/support.  
After extracting the SDK, you should add the "SDK/Mac/include" path into "xcode project > Build Settings > Header Search Paths".  

Replace the "DeckLinkAPIDispatch.cpp" file in the project with your own file in "SDK/Mac/include/DeckLinkAPIDispatch.cpp". 

# Build
Just click the "Build and Run" button.  
Check whether the "CoreFoundation.framework" is in the "xcode project > Build Phases > Link Binary With Librares" list in case of Build Failed.

# Config
Make sure the resolution and framerate setting in main.cpp is compatible with your DeckLink capture card.

# Screenshot
![image](https://raw.githubusercontent.com/js-john/Blackmagic-DeckLink-Opencv/master/screenshot.jpg)

# Platform
Tested on Macbook Pro 2014 using BlackMagicDesign Ultrastudio Mini Recorder.  
OS: macOS Catalina 10.15.  
![image](https://github.com/js-john/Blackmagic-DeckLink-Opencv/blob/master/IMG_0428.jpg?raw=true)
