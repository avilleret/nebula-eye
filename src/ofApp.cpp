#include "ofApp.h"

void nebulaEye::setup()
{
  roiGuiGrp.setName("ROI");
  roiGuiGrp.add(radius.set("rayon", 250, 1 , ofGetWidth()/2));
  roiGuiGrp.add(center.set("centre", ofPoint(ofGetWidth()/2, ofGetHeight()/2), ofPoint(0,0), ofPoint(ofGetWidth(), ofGetHeight())));

  video.setup();
  flow.setup();
  bgSub.setup();
  contour.setup();

  gui.setup("","settings.xml",660,10);
  gui.add(video.guiGrp);
  gui.add(roiGuiGrp);
  gui.add(bgSub.guiGrp);
  gui.add(flow.guiGrp);
  gui.add(contour.guiGrp);

  ofSetCircleResolution(100);

  ofSetBackgroundColor(0);
}

void nebulaEye::update()
{
  video.update();
  if(video.isFrameNew()){
    auto img = video.getPixels();
    bgSub.update(img);
    flow.update(img);
    contour.update(img);
  }
}

void nebulaEye::draw()
{
  video.draw(0,0,640,480);
  bgSub.draw(640,0,640,480);
  flow.draw(0,480, 640, 480);
  contour.draw(640,480,640,480);

  //ofSetColor(255,255,255,128);
  //ofDrawCircle(center->x, center->y, radius);
  gui.draw();
}

void nebulaEye::exit()
{
}

void nebulaEye::mouseMoved(ofMouseEventArgs& mouse)
{
}

void nebulaEye::mouseDragged(ofMouseEventArgs& mouse)
{
}

void nebulaEye::mousePressed(ofMouseEventArgs& mouse)
{
}

void nebulaEye::mouseReleased(ofMouseEventArgs& mouse)
{
}

void nebulaEye::mouseScrolled(ofMouseEventArgs& mouse)
{
}

void nebulaEye::mouseEntered(ofMouseEventArgs& mouse)
{
}

void nebulaEye::mouseExited(ofMouseEventArgs& mouse)
{
}
