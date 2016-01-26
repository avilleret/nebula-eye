#include "ofApp.h"

void nebulaEye::setup()
{
  client.setup("127.0.0.1",0);
  client.addVideoChannel(5000);

  roiGuiGrp.setName("ROI");
  roiGuiGrp.add(radius.set("rayon", 250, 1 , ofGetWidth()/2));
  roiGuiGrp.add(center.set("centre", ofPoint(ofGetWidth()/2, ofGetHeight()/2), ofPoint(0,0), ofPoint(ofGetWidth(), ofGetHeight())));

  flow.setup();
  bgSub.setup();
  contour.setup();

  gui.setup("","settings.xml",660,10);
  gui.add(client.parameters);
  gui.add(roiGuiGrp);
  gui.add(bgSub.guiGrp);
  gui.add(flow.guiGrp);

  client.play();

  ofSetCircleResolution(100);

  ofSetBackgroundColor(0);
}

void nebulaEye::update()
{
  client.update();
  if(client.isFrameNewVideo()){
    auto img = client.getPixelsVideo();
    remoteVideo.loadData(img);
    bgSub.update(img);
    flow.update(img);
  }
}

void nebulaEye::draw()
{
  if ( remoteVideo.isAllocated() )
    remoteVideo.draw(0,0,640,480);
  bgSub.draw(640,0,640,480);
  flow.draw(0,480, 640, 480);

  ofSetColor(255,255,255,128);
  ofDrawCircle(center->x, center->y, radius);
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
