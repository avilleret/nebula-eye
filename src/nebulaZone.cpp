//  inspired by :
//  circle.cpp
//  simpleEventsExample
//  Roy Macdonald on 12/25/12.
//
// Created by Antoine Villeret in 2016 for Nebula installation
// Copyright (c) 2016 Antoine Villeret. All rights reserved.
//

#include "nebulaZone.h"
#include <algorithm> // std::max

// the static event, or any static variable, must be initialized outside of the class definition.
ofEvent<ofVec2f> nebula::Zone::clickedInsideGlobal = ofEvent<ofVec2f>();

nebula::Zone::Zone() {
    bRegisteredEvents = false;
}

nebula::Zone::~Zone() {
    clear();
}

void nebula::Zone::setup(){

    if(!bRegisteredEvents) {
        ofRegisterMouseEvents(this); // this will enable our circle class to listen to the mouse events.
        bRegisteredEvents = true;
    }
    guiGrp.setName("Zones settings");

    guiGrp.add(radius.set("zone radius", ofVec3f(20, 120, 400), ofVec3f(20,20,20), ofVec3f(ofGetWidth()/2)));
    guiGrp.add(center.set("center", ofVec2f(ofGetWidth()/2, ofGetHeight()/2), ofVec2f(0,0), ofVec2f(ofGetWidth(), ofGetHeight())));
    guiGrp.add(angleOrigin.set("ref axis",0,-180,180));
}

void nebula::Zone::draw(){
    ofPushStyle();
    ofFill();
    ofSetLineWidth(3);
    ofDrawCircle(center.get(), radius->x);
    ofNoFill();
    ofDrawCircle(center.get(), radius->y);
    ofDrawCircle(center.get(), radius->z);
    ofVec2f originPol = ofVec2f(radius->x,ofDegToRad(angleOrigin.get()));
    ofVec2f originCar = nebula::Utils::polToCar(originPol) + center;
    ofDrawLine(center.get(),originCar);
    ofPopStyle();
}

void nebula::Zone::clear() {
    if(bRegisteredEvents) {
        ofUnregisterMouseEvents(this); // disable litening to mous events.
        bRegisteredEvents = false;
    }
}

void nebula::Zone::mouseMoved(ofMouseEventArgs & args){}
void nebula::Zone::mouseDragged(ofMouseEventArgs & args){
  switch (zcatch) {
    case 0:
      if ( args.button ){
        center.set(args);
      } else {
        float dist = std::max(args.distance(center),float(10.));
        radius.set(ofVec3f(dist,radius->y, radius->z));
        ofVec2f vect = args - center;
        angleOrigin.set(-vect.angle(ofVec2f(1,0)));
      }
      break;
    case 1:
      radius.set(ofVec3f(radius->x, args.distance(center),radius->z));
      break;
    case 2:
      radius.set(ofVec3f(radius->x, radius->y, args.distance(center)));
      break;
    default:
      break;
  }
  createMask();
}
void nebula::Zone::mousePressed(ofMouseEventArgs & args){
  zcatch = inside(args);
}
void nebula::Zone::mouseReleased(ofMouseEventArgs & args){
  zcatch = -1;
}
void nebula::Zone::mouseScrolled(ofMouseEventArgs & args){}
void nebula::Zone::mouseEntered(ofMouseEventArgs & args){}
void nebula::Zone::mouseExited(ofMouseEventArgs & args){}

//this function checks if the passed arguments are inside the circle.
int nebula::Zone::inside(ofVec2f pt){
  if ( pt.distance(center) < radius->x ) return 0;
  if ( pt.distance(center) < radius->y ) return 1;
  if ( pt.distance(center) < radius->z ) return 2;
  else return 3;
}

void nebula::Zone::attach(bool & flag){
  if (flag && !bRegisteredEvents){
    ofRegisterMouseEvents(this); // this will enable our circle class to listen to the mouse events.
    bRegisteredEvents = true;
  } else if (!flag && bRegisteredEvents){
    ofUnregisterMouseEvents(this); // this will enable our circle class to listen to the mouse events.
    bRegisteredEvents = false;
  }
}

void nebula::Zone::createMask(){

  mask.clear();
  // create zone mask
  ofVec2f pt = center.get();

  pt.x = float(pt.x) * float(size.width)/ofGetWidth();
  pt.y = float(pt.y) * float(size.height)/ofGetHeight();
  cv::Point scaledCenter = ofxCv::toCv(pt);
  for ( int i = 0; i < 3 ; i++ ){
    // clear black image
    mask.push_back(cv::Mat::zeros(size.height, size.width , CV_8UC1));
    // first draw a white filled circle
    int rad = radius.get()[i] * mask[i].cols / ofGetWidth();
    cv::circle(mask[i], scaledCenter, rad, cv::Scalar(255,255,255),-1);
  }

  int rad = radius.get()[0] * mask[0].cols / ofGetWidth();
  // then add a black one into the white to mask the other zone(s)
  cv::circle(mask[1], scaledCenter, rad, cv::Scalar(0),-1);
  rad = radius.get()[1] * mask[0].cols / ofGetWidth();
  cv::circle(mask[2], scaledCenter, rad, 0,-1);

  // for the last zone, create a white image and draw a black circle
  mask.push_back( cv::Mat::ones(size.height, size.width, CV_8UC1)*255 );
  rad = radius.get()[2] * mask[2].cols / ofGetWidth();
  cv::circle(mask[mask.size()-1], scaledCenter, rad, 0, -1);
}

void nebula::Zone::setSize(cv::Size s){
  bool changed = s != size;
  size = s;
  if (changed) createMask();
}
