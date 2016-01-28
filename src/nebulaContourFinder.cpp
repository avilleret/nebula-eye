
/************************/
/* Contour finder class */
/************************/
#include "nebulaContourFinder.h"

void nebulaContourFinder::setup(){
  // setup contour finder parameter GUI
  guiGrp.add(enabled.set("enable",true));
  guiGrp.add(minAreaRad.set("minimum area radius",1,0,320));
  guiGrp.add(minAreaRad.set("maximum area radius",100,0,320));
  guiGrp.add(threshold.set("threshold",15,0,100));
  guiGrp.add(blurAmount.set("blur",10,0,100));

  contourFinder.setMinAreaRadius(minAreaRad);
  contourFinder.setMaxAreaRadius(maxAreaRad);
  contourFinder.setThreshold(15);
  // wait for half a frame before forgetting something
  contourFinder.getTracker().setPersistence(15);
  // an object can move up to 32 pixels per frame
  contourFinder.getTracker().setMaximumDistance(32);
}

void nebulaContourFinder::update(ofPixels &img){
  if(!enabled) return;
  if ( blurAmount > 0 ){
    ofxCv::blur(img, blurred, blurAmount);
    contourFinder.findContours(blurred);
  } else {
    contourFinder.findContours(img);
  }
}

void nebulaContourFinder::draw(int x, int y, int w, int h){
  if(!enabled) return;
}
