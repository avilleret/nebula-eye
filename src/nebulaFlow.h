#pragma once

#include "ofxCv.h"
#include "ofxGui.h"

class nebulaFlow {
public:
    void setup();
    void update(ofPixels &img);
    void draw(int x, int y, int w, int h);
    void draw(int x, int y){draw(x,y,0,0);};

    ofxCv::FlowFarneback flow;

//protected:
    ofParameterGroup guiGrp;
    ofParameter<float> fbPyrScale, fbPolySigma;
    ofParameter<int> fbLevels, fbIterations, fbPolyN, fbWinSize;
    ofParameter<bool> fbUseGaussian, usefb, enabled;
};
