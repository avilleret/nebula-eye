
/************************/
/* Contour finder class */
/************************/
#include "nebulaContourFinder.h"

void nebulaContourFinder::setup(){
  // setup contour finder parameter GUI
  guiGrp.setName("Blob tracker");
  guiGrp.add(enabled.set("enable",true));
  guiGrp.add(minAreaRad.set("minimum area radius",1,0,320));
  guiGrp.add(maxAreaRad.set("maximum area radius",100,0,320));
  guiGrp.add(threshold.set("threshold",15,0,100));
  guiGrp.add(blurAmount.set("blur",10,0,100));
  guiGrp.add(persistence.set("persistence", 15,0,200));
  guiGrp.add(maxDistance.set("max distance",32,0,200));
  guiGrp.add(showLabels.set("show label",true));

  contourFinder.setMinAreaRadius(minAreaRad);
  contourFinder.setMaxAreaRadius(maxAreaRad);
  contourFinder.setThreshold(threshold);
  // wait for half a frame before forgetting something
  contourFinder.getTracker().setPersistence(persistence);
  // an object can move up to 32 pixels per frame
  contourFinder.getTracker().setMaximumDistance(maxDistance);
}

void nebulaContourFinder::update(ofPixels &img){
  if(!enabled) return;
  ofxCv::blur(img, blurred, blurAmount);
  ofxCv::toOf(blurred, blurredImg);
  //blurredImg = ofImage(img);
  contourFinder.findContours(blurred);

  if(blurredImg.getWidth() != fbo.getWidth() || blurredImg.getHeight() != fbo.getHeight() ){
    fbo.allocate(blurredImg.getWidth(), blurredImg.getHeight(), GL_RGB);
    fbo.begin();
    ofClear(0,0,255, 0); // blue screen after allocation
    fbo.end();
  }
}

void nebulaContourFinder::draw(int x, int y, int w, int h){
  if(!enabled) return;
  ofxCv::RectTracker& tracker = contourFinder.getTracker();

  fbo.begin();
  ofClear(0, 0, 0, 0);

  if ( blurredImg.isAllocated() )
    blurredImg.draw(x,y,w,h);
  contourFinder.draw();

  if(showLabels) {
    ofSetColor(255);
    for(int i = 0; i < contourFinder.size(); i++) {
      ofPoint center = ofxCv::toOf(contourFinder.getCenter(i));
      ofPushMatrix();
      ofTranslate(center.x, center.y);
      int label = contourFinder.getLabel(i);
      string msg = ofToString(label) + ":" + ofToString(tracker.getAge(label));
      ofDrawBitmapString(msg, 0, 0);
      ofVec2f velocity = ofxCv::toOf(contourFinder.getVelocity(i));
      ofScale(5, 5);
      ofDrawLine(0, 0, velocity.x, velocity.y);
      ofPopMatrix();
    }
  } else {
    for(int i = 0; i < contourFinder.size(); i++) {
      unsigned int label = contourFinder.getLabel(i);
      // only draw a line if this is not a new label
      if(tracker.existsPrevious(label)) {
        // use the label to pick a random color
        ofSeedRandom(label << 24);
        ofSetColor(ofColor::fromHsb(ofRandom(255), 255, 255));
        // get the tracked object (cv::Rect) at current and previous position
        const cv::Rect& previous = tracker.getPrevious(label);
        const cv::Rect& current = tracker.getCurrent(label);
        // get the centers of the rectangles
        ofVec2f previousPosition(previous.x + previous.width / 2, previous.y + previous.height / 2);
        ofVec2f currentPosition(current.x + current.width / 2, current.y + current.height / 2);
        ofDrawLine(previousPosition, currentPosition);
      }
    }
  }
  fbo.end();

  fbo.draw(x,y,w,h);

  ofPushMatrix();
  ofTranslate(x,y);
  // this chunk of code visualizes the creation and destruction of labels
  const vector<unsigned int>& currentLabels = tracker.getCurrentLabels();
  const vector<unsigned int>& previousLabels = tracker.getPreviousLabels();
  const vector<unsigned int>& newLabels = tracker.getNewLabels();
  const vector<unsigned int>& deadLabels = tracker.getDeadLabels();
  ofSetColor(ofxCv::cyanPrint);
  for(int i = 0; i < currentLabels.size(); i++) {
    int j = currentLabels[i];
    ofDrawLine(j, 0, j, 4);
  }
  ofSetColor(ofxCv::magentaPrint);
  for(int i = 0; i < previousLabels.size(); i++) {
    int j = previousLabels[i];
    ofDrawLine(j, 4, j, 8);
  }
  ofSetColor(ofxCv::yellowPrint);
  for(int i = 0; i < newLabels.size(); i++) {
    int j = newLabels[i];
    ofDrawLine(j, 8, j, 12);
  }
  ofSetColor(ofColor::white);
  for(int i = 0; i < deadLabels.size(); i++) {
    int j = deadLabels[i];
    ofDrawLine(j, 12, j, 16);
  }
  ofPopMatrix();
}
