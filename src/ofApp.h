#pragma once

#include "ofMain.h"
#include "ofxGstRTPClient.h"
#include "ofxGui.h"
#include "ofxCv.h"

#include "nebulaFlow.h"
#include "nebulaBackground.h"
#include "nebulaContourFinder.h"

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

    void learningTimeChanged(int & _time);
    void thresholdChanged(int & tresh);

    ofxGstRTPClient client;
    ofTexture remoteVideo;
    ofParameter<ofVec2f> center;
    ofParameter<float> radius;

    ofParameterGroup roiGuiGrp;

    ofxPanel gui;

    nebulaFlow flow;
    nebulaBackground bgSub;
    nebulaContourFinder contour;
};
