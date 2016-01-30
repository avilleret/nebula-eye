#pragma once
#include "ofMain.h"
#include "ofxCv.h"
#include "ofxGui.h"
#include "opencv2/ocl/ocl.hpp"

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
    ofParameter<bool> fbUseGaussian, usefb, enabled, forceCPU;
    bool gpuMode;

    cv::ocl::FarnebackOpticalFlow oclFbFlow;
    cv::ocl::oclMat d_prev, d_input, d_flowx, d_flowy; // d for "device"
    // cv::ocl::PyrLKOpticalFlow oclLKFLow;
};
