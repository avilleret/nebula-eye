#pragma once

#include "ofxGui.h"
#include "ofxGstRTPClient.h"

class nebulaVideoSrc
{
public:
    void setup();
    void update();
    void draw(int x, int y, int w, int h);
    void draw(int x, int y){draw(x,y,0,0);};
    bool isFrameNew();
    ofPixels& getPixels();

    void srcMovieCb(bool & flag);
    void srcRTPCb(bool & flag);

    ofVideoPlayer movie;
    ofxGstRTPClient client;
    ofTexture texture;
    ofParameterGroup guiGrp;

    ofParameter<bool> srcMovie, srcRTP;
};
