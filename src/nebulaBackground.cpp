
/********************************/
/* Background substractor class */
/********************************/
#include "nebulaBackground.h"

void nebulaBackground::setup(){

  guiGrp.setName("Background substraction");
  guiGrp.add(enabled.set("enable",true));
  guiGrp.add(learningTime.set("Learning time", 10, 0, 100));
  guiGrp.add(threshold.set("threshold", 60, 0, 255));

  learningTime.addListener(this, &nebulaBackground::learningTimeChanged);
  threshold.addListener(this, &nebulaBackground::thresholdChanged);
  background.setLearningTime(learningTime);
  background.setThresholdValue(threshold);
}

void nebulaBackground::update(ofPixels &img){
  if(!enabled) return;
  background.update(img, thresholded);
  thresholded.update();
}

void nebulaBackground::draw(int x, int y, int w, int h){
  if(!enabled) return;
  if ( thresholded.isAllocated() )
    thresholded.draw(x,y,w,h);
}

void nebulaBackground::learningTimeChanged(int & t){
  background.setLearningTime(t);
}

void nebulaBackground::thresholdChanged(int & thresh){
  background.setThresholdValue(thresh);
}
