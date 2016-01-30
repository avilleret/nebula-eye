#pragma once

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
      contourFinder.findContours(blurred);

      if(blurred.getWidth() != fbo.getWidth() || blurred.getHeight() != fbo.getHeight() ){
        fbo.allocate(blurred.getWidth(), blurred.getHeight(), GL_RGBA);
        fbo.begin();
        ofClear(0,0,0,0); // clear screen after allocation
        fbo.end();
      }
    }
    void draw(int x, int y, int w, int h);
    void draw(int x, int y){draw(x,y,0,0);};

    void showLabelsCb(bool& flag);

//protected:
    ofxCv::ContourFinder contourFinder;
    ofFbo fbo;

    ofParameterGroup guiGrp;
    ofParameter<int> minAreaRad, maxAreaRad, threshold, blurAmount, erodeAmount, persistence, maxDistance;
    ofParameter<bool> showLabels, enabled;
    ofImage blurred;
};
