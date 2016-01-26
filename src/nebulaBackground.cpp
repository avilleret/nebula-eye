
/********************************/
/* Background substractor class */
/********************************/
#include "nebulaBackground.h"

void nebulaBackground::setup(){
  guiGrp.setName("Background substraction");
  guiGrp.add(learningTime.set("Learning time", 10, 0, 100));
  guiGrp.add(threshold.set("threshold", 60, 0, 255));

  learningTime.addListener(this, &nebulaBackground::learningTimeChanged);
  threshold.addListener(this, &nebulaBackground::thresholdChanged);
  background.setLearningTime(900);
  background.setThresholdValue(10);
}

void nebulaBackground::update(ofPixels &img){
  background.update(img, thresholded);
  thresholded.update();
}

void nebulaBackground::draw(int x, int y, int w, int h){
  if ( thresholded.isAllocated() )
    thresholded.draw(x,y,w,h);
}

void nebulaBackground::learningTimeChanged(int & t){
  background.setLearningTime(t);
}

void nebulaBackground::thresholdChanged(int & thresh){
  background.setThresholdValue(thresh);
}
