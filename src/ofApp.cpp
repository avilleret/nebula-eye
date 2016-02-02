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
  displayGuiGrp.add(showDebug.set("show flow mask",0,0,4));

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

    // for the last zone, get a white image and draw a black circle
    zoneMask.push_back( cv::Mat::ones(flow.m_flow.rows, flow.m_flow.cols , CV_8UC1)*255 );
    rad = zone.radius.get()[2] * zoneMask[2].cols / ofGetWidth();
    cv::circle(zoneMask[zoneMask.size()-1], center, rad, 0, -1);

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
    ofRectangle rect = ofRectangle(zoneMask[i].cols/2,zoneMask[i].rows/2, zoneMask[i].cols, zoneMask[i].rows);
    ofxCv::drawMat(zoneMask[i],rect.x, rect.y);
    cv::Mat m_flow;
    double zoneFlow = flow.getFlowInMask(zoneMask[i], &m_flow);
    ofVec2f offset(rect.x,rect.y);
    ofVec2f scale(rect.width/m_flow.cols, rect.height/m_flow.rows);
    int stepSize = 4;
    ofPopStyle();
    ofSetColor(0,255,0);
    for(int y = 0; y < m_flow.rows; y += stepSize) {
      for(int x = 0; x < m_flow.cols; x += stepSize) {
        ofVec2f cur = ofVec2f(x, y) * scale + offset;
        const cv::Vec2f& vec = m_flow.at<cv::Vec2f>(y, x);
        ofDrawLine(cur, ofVec2f(x + vec[0], y + vec[1]) * scale + offset);
      }
    }
    ofPushStyle();
    stringstream ss;
    ss << "zone flow : " << zoneFlow;
    ofDrawBitmapString(ss.str(), ofPoint(rect.x+10,rect.y-10));
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
  //vector<ofPoint> centroids = contour.getCentroids();
  for (int i = 0; i < contour.finder.size(); i++ ){
    ofxOscMessage m;    
    m.setAddress("/b");
    m.addInt32Arg(contour.finder.getLabel(i));
    ofVec2f centroid = ofxCv::toOf(contour.finder.getCentroid(i));
    centroid -= zone.center;
    ofVec2f centroidPol = nebula::Utils::carToPol(centroid);
    centroidPol.y -= zone.angleOrigin;
    m.addFloatArg(centroidPol.x);
    m.addFloatArg(centroidPol.y);
    m.addFloatArg(contour.finder.getContourArea(i));
    m.addInt32Arg(zone.inside(ofxCv::toOf(contour.finder.getCentroid(i))));
    bundle.addMessage(m);
  }

  ofxOscMessage m;
  m.setAddress("/f");
  for ( int i = 0; i < zoneMask.size() ; i++ ){
      double f = flow.getFlowInMask(zoneMask[i], NULL);
      m.addFloatArg(f);
  }
  bundle.addMessage(m);
  sender.sendBundle(bundle);
}
