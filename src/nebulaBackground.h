#pragma once
#include "ofxCv.h"
#include "ofxGui.h"
#include "opencv2/ocl/ocl.hpp"

class nebulaBackground {
public:
    void learningTimeChanged(int & t);
    void thresholdChanged(int & thresh);
    void algoMOGCb(bool & flag);
    void algoMOG2Cb(bool & flag);
    void algoGMGCb(bool & flag);
    void algoClassicCb(bool & flag);
    void initAlgo();

    void setup();
    void update(ofPixels &img);
    void draw(int x, int y, int w, int h);
    void draw(int x, int y){draw(x,y,0,0);};
    void save(); // save GUI parameters
    void initBgsubGui();
    void saveAlgoParam();

//protected:
    ofxCv::RunningBackground background;

    ofImage thresholded;

    ofParameterGroup guiGrp;
    ofParameter<int> threshold, learningTime;
    ofParameter<bool> enabled, algoClassic, algoGMG, algoMOG, algoMOG2, showBgsubGui, forceCPU;
    bool gpuMode;

    //std::vector<std::unique_ptr<ofAbstractParameter>> bgsubParameters;
    std::vector<ofParameter<float> > bgsubParameters;
    ofxGuiGroup bgsubGui;

    cv::Ptr<cv::BackgroundSubtractor> m_fgbg;

    cv::Mat m_fgmask, m_segm;
    std::string m_algoName;
    std::vector<std::string> m_bgsub_algos;
    ofParameter<int> m_threshold, m_initFrames;

    cv::ocl::MOG m_oclMOG;
    cv::ocl::MOG2 m_oclMOG2;
    //~ cv::ocl::GMG m_oclGMG;
    cv::ocl::oclMat d_input, d_fgmask;
};
