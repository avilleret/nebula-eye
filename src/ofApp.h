#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxCv.h"
#include "ofxOsc.h"
#include "ofxCsv.h"

#include "nebulaVideoSrc.h"
#include "nebulaFlow.h"
#include "nebulaBackground.h"
#include "nebulaContourFinder.h"
#include "nebulaZone.h"

#define OSC_IP "localhost"
#define OSC_PORT 123456

class nebulaEye : public ofBaseApp
{
  public:
    void setup  ();
    void update ();
    void draw   ();
    void exit   ();
    void mouseMoved(ofMouseEventArgs& mouse);
    void mouseDragged(ofMouseEventArgs& mouse);
    void mousePressed(ofMouseEventArgs& mouse);
    void mouseReleased(ofMouseEventArgs& mouse);
    void mouseScrolled(ofMouseEventArgs& mouse);
    void mouseEntered(ofMouseEventArgs& mouse);
    void mouseExited(ofMouseEventArgs& mouse);
    void keyPressed(int key);

    void learningTimeChanged(int & _time);
    void thresholdChanged(int & tresh);

    void sendOSC();

    void recordCSVData();
    void setupCSVrecorder();
    void csvRecordCb(bool & flag);
    string getDate(), getHour();

    ofxPanel gui, recordPanel;
    ofParameterGroup displayGuiGrp;
    ofParameter<bool> showGui, showVideo, showBgSub, showContour, showFlow, showZone, record;
    ofParameter<int> bgSubIntensity, showDebug;

    nebulaVideoSrc video;
    nebulaFlow flow;
    nebulaBackground bgSub;
    nebulaContourFinder contour;
    nebula::Zone zone;

    ofPixels img;
    ofxOscSender sender;
    wng::ofxCsv csvRecorder;

    vector<cv::Mat> zoneMask;

    vector<float> flowZone;

};
