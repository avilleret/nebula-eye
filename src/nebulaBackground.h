#pragma once
#include "ofxCv.h"
#include "ofxGui.h"

class nebulaBackground {
public:
    void learningTimeChanged(int & t);
    void thresholdChanged(int & thresh);

    void setup();
    void update(ofPixels &img);
    void draw(int x, int y, int w, int h);
    void draw(int x, int y){draw(x,y,0,0);};

//protected:
    ofxCv::RunningBackground background;

    ofImage thresholded;

    ofParameterGroup guiGrp;
    ofParameter<int> threshold, learningTime;
};
