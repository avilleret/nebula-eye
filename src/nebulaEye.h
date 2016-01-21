#pragma once

#include "ofMain.h"
#include "ofxGstRTPClient.h"
#include "ofxGui.h"

class nebulaEye : public ofBaseApp
{
  public:
    void setup  ();
    void update ();
    void draw   ();
    void exit   ();

    ofxGstRTPClient client;
    ofTexture remoteVideo;

    ofxPanel gui;
};
