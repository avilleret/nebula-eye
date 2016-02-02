#include "ofApp.h"

void nebulaEye::setup()
{
  sender.setup(OSC_IP, OSC_PORT);
  video.setup();
  flow.setup();
  bgSub.setup();
  contour.setup();
  zone.setup();

  displayGuiGrp.setName("display");
  displayGuiGrp.add(showGui.set("show this menu",true));
  displayGuiGrp.add(showVideo.set("show video",true));
  displayGuiGrp.add(showBgSub.set("show bgsub",true));
  displayGuiGrp.add(bgSubIntensity.set("bg instensity",127,0,255));
  displayGuiGrp.add(showContour.set("show contour",true));
  displayGuiGrp.add(showFlow.set("show motion flow",true));
  displayGuiGrp.add(showZone.set("show zone",true));
  displayGuiGrp.add(showDebug.set("show flow mask",0,0,3));

  gui.setup("nebula-eye","settings.xml",660,10);
  gui.add(displayGuiGrp);
  gui.add(video.guiGrp);
  gui.add(bgSub.guiGrp);
  gui.add(flow.guiGrp);
  gui.add(contour.guiGrp);
  gui.add(zone.guiGrp);

  ofAddListener(gui.savePressedE, &bgSub, &nebulaBackground::saveAlgoParam);
  showGui.addListener(&bgSub, &nebulaBackground::showGui);
  showZone.addListener(&zone, &nebula::Zone::attach);

  gui.loadFromFile("settings.xml");

  ofSetCircleResolution(100);

  ofSetBackgroundColor(0,0,0,0);
}

void nebulaEye::update()
{
  video.update();
  if(video.isFrameNew()){
    img = video.getPixels();
    bgSub.update(img);
    flow.update(img);
    contour.update(bgSub.m_fgmask);

    // create zone mask
    // TODO redraw only on zone change
    zoneMask.resize(3);

    ofVec2f scaledCenter = zone.center.get();

    scaledCenter.x = float(scaledCenter.x) * float(flow.m_flow.cols)/ofGetWidth();
    scaledCenter.y = float(scaledCenter.y) * float(flow.m_flow.rows)/ofGetHeight();
    cv::Point center = ofxCv::toCv(scaledCenter);
    for ( int i = 0; i < zoneMask.size() ; i++ ){
      // clear everything
      zoneMask[i] = cv::Mat::zeros(flow.m_flow.rows, flow.m_flow.cols , CV_8UC1);
      // first draw a white filled circle
      int rad = zone.radius.get()[i] * zoneMask[i].cols / ofGetWidth();
      cv::circle(zoneMask[i], center, rad, cv::Scalar(255,255,255),-1);
    }

    int rad = zone.radius.get()[0] * zoneMask[0].cols / ofGetWidth();
    // then add a black one into the white to mask the other zone(s)
    cv::circle(zoneMask[1], center, rad, cv::Scalar(0),-1);
    rad = zone.radius.get()[1] * zoneMask[0].cols / ofGetWidth();
    cv::circle(zoneMask[2], center, rad, 0,-1);

    vector<ofVec2f> zoneFlow;
    zoneFlow.resize(zoneMask.size());

    for ( int i = 0; i < zoneMask.size() ; i++ ){
        flow.getFlowInMask(zoneMask[i], zoneFlow[i]);
    }

    sendOSC();
  }
}

void nebulaEye::draw()
{
  int w(ofGetWidth()), h(ofGetHeight());
  if ( ( ofGetHeight() > 0 && img.getHeight() > 0 ) ) {
    if ( ofGetWidth()/ofGetHeight() > img.getWidth()/img.getHeight()){
      h = ofGetHeight();
      w = h * img.getWidth()/img.getHeight();
    } else if (img.getWidth()>0){
      w = ofGetWidth();
      h = w * img.getHeight()/img.getWidth();
    }
  }
    
  // ofLogVerbose("ofApp") << "drawing resolution  : " << w << " x " << h;
  ofSetColor(255,255,255);
  if(showVideo)
    video.draw(0,0,w,h);
  if(showBgSub){
    ofSetColor(255,255,255,bgSubIntensity);
    bgSub.draw(0,0,w,h);
  }

  if(showFlow){
    ofSetColor(0,255,0,255);
    flow.draw(0,0,w, h);
  }

  if(showContour){
    ofSetColor(255,255,255);
    contour.draw(0,0,w,h);
  }

  if(showZone){
    ofSetColor(255,0,0,64);
    zone.draw();
  }

  if (showDebug){
    ofSetColor(255);
    int i = showDebug.get()-1;
    ofxCv::drawMat(zoneMask[i],zoneMask[i].cols/2,zoneMask[i].rows/2);
  }

  if (showGui){
    gui.draw();
  }
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

void nebulaEye::keyPressed(int key){
  switch (key){
    case 27:
      ofExit();
      break;
    case 'h':
      showGui = !showGui;
      break;
    case 's':
      gui.saveToFile("settings.xml");
      bgSub.saveAlgoParam();
      break;
    default:
      break;
  }
}

void nebulaEye::sendOSC(){
  ofxOscBundle bundle;
  vector<ofPoint> centroids = contour.getCentroids();
  for (int i = 0; i < centroids.size(); i++ ){
    ofxOscMessage m;
    std::stringstream address;
    address << "/b/" << i;
    m.setAddress(address.str());
    m.addFloatArg(centroids[i].x);
    m.addFloatArg(centroids[i].y);
    bundle.addMessage(m);
  }
  sender.sendBundle(bundle);
}
