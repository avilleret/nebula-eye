#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxCv.h"

#include "nebulaVideoSrc.h"
#include "nebulaFlow.h"
#include "nebulaBackground.h"
#include "nebulaContourFinder.h"
#include "nebulaZone.h"

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

    ofxPanel gui;

    nebulaVideoSrc video;
    nebulaFlow flow;
    nebulaBackground bgSub;
    nebulaContourFinder contour;
    nebula::Zone zone;
};
