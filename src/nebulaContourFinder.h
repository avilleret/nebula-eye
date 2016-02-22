#pragma once
#include "ofMain.h"
#include "ofxCv.h"
#include "ofxGui.h"

class nebulaContourFinder {
public:
    void setup();
    void update(cv::Mat img);
    void draw(int x, int y, int w, int h);
    void draw(int x, int y){draw(x,y,0,0);};

    void showLabelsCb(bool& flag);
    void minAreaCb(int& val);
    void maxAreaCb(int& val);
    void thresholdCb(int& val);
    void persistenceCb(int& val);
    void maxDistanceCb(int& val);

//protected:
    ofxCv::ContourFinder finder;
    ofFbo fbo;

    ofParameterGroup guiGrp;
    ofParameter<int> minAreaRad, maxAreaRad, threshold, blurAmount, erodeAmount, persistence, maxDistance;
    ofParameter<bool> showLabels, enabled;
    cv::Mat blurred;
};
