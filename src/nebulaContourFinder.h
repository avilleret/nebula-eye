#pragma once
#include "ofMain.h"
#include "ofxCv.h"
#include "ofxGui.h"

class nebulaContourFinder {
public:
    void setup();
    template<class S>
    void update(S &img){
      if(!enabled) return;

      ofxCv::erode(img, blurred, erodeAmount);
      ofxCv::blur(blurred, blurAmount);
      finder.findContours(blurred);
    }
    void draw(int x, int y, int w, int h);
    void draw(int x, int y){draw(x,y,0,0);};

    void showLabelsCb(bool& flag);

    vector<ofPoint> getCentroids();

//protected:
    ofxCv::ContourFinder finder;
    ofFbo fbo;

    ofParameterGroup guiGrp;
    ofParameter<int> minAreaRad, maxAreaRad, threshold, blurAmount, erodeAmount, persistence, maxDistance;
    ofParameter<bool> showLabels, enabled;
    ofImage blurred;
};
