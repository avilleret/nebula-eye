//  inspired by :
//  circle.h
//  simpleEventsExample
//
//  Created by Roy Macdonald on 12/23/12.
//  Copyright (c) 2012 micasa. All rights reserved.
//

#pragma once

#include "ofMain.h"
#include "nebulaUtils.h"
#include "ofxCv.h"

namespace nebula {

  class Zone {
  public:
    Zone();
    ~Zone();
    // to be able to broadcast events from a class we must declare the ofEvent object that will be broadcasted.
    // this object can be declared as class variable so events are broadcasted and listened for each specific class instance.
    // the data type of the event, what's declared between the < > can be whatever you want, event a custom class.
    ofEvent<ofVec2f> clickedInside;

    //this is a shared event for all the instances of this class, so any instance of this class will broadcast to the same event,
    //this way you'll have to register only one listener to listen to any class instance broadcasting to this event.
    // "static" tells the compiler that all of this class instances will share a single variable
    static ofEvent<ofVec2f> clickedInsideGlobal;

    void setup();
    void draw();
    void clear();
    void attach(bool & flag);

    //We need to declare all this mouse events methods to be able to listen to mouse events.
    //All this must be declared even if we are just going to use only one of this methods.
    void mouseMoved(ofMouseEventArgs & args);
    void mouseDragged(ofMouseEventArgs & args);
    void mousePressed(ofMouseEventArgs & args);
    void mouseReleased(ofMouseEventArgs & args);
    void mouseScrolled(ofMouseEventArgs & args);
    void mouseEntered(ofMouseEventArgs & args);
    void mouseExited(ofMouseEventArgs & args);

    //this function checks in which zone the passed arguments are.
    int inside(ofVec2f pt);
    void setSize(cv::Size);
    ofParameterGroup guiGrp;
    ofParameter<ofVec2f> center;
    ofParameter<ofVec3f> radius;
    ofParameter<float> angleOrigin;
    int zcatch; // zone dragged, -1 if mouse is released
    vector<cv::Mat> mask; // one mask for each zone

  protected:
      bool bRegisteredEvents;
      void createMask();
      cv::Size size; // expected size of the incomming image
  };
}
