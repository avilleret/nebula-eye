#pragma once
#include "ofMain.h"
#include "ofxCv.h"
#include "ofxGui.h"

class nebulaContourFinder {
public:
    void setup();
    void update(cv::Mat img){
      if(!enabled || img.size() == cv::Size(0,0)) return;
        
      cv::Mat input = ofxCv::toCv(img), gray;
      if(input.channels() == 1){
        gray = input;
      } else {
        cv::cvtColor(input, gray, CV_RGB2GRAY);
      }
        
      if ( blurred.size() != gray.size() ) blurred.create(gray.size(), CV_8UC1);
        
      ofxCv::erode(gray, blurred, erodeAmount);
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
    cv::Mat blurred;
};
