#pragma once

#include "ofxCv.h"
#include "ofxGui.h"

class nebulaContourFinder {
public:
    void setup();
    void update(ofPixels &img);
    void draw(int x, int y, int w, int h);
    void draw(int x, int y){draw(x,y,0,0);};

//protected:
    ofxCv::ContourFinder contourFinder;

    ofParameterGroup guiGrp;
    ofParameter<int> minAreaRad, maxAreaRad, threshold, blurAmount;
    cv::Mat blurred;
};
