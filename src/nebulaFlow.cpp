/*********************************/
/* Motion flow computation class */
/*********************************/
#include "nebulaFlow.h"

void nebulaFlow::setup(){
    // setup flow parameter GUI
  guiGrp.setName("Motion flow parameters");
  guiGrp.add(enabled.set("enable",true));
  guiGrp.add(fbPyrScale.set("fbPyrScale", .5, 0, .99));
  guiGrp.add(fbLevels.set("fbLevels", 4, 1, 8));
  guiGrp.add(fbIterations.set("fbIterations", 2, 1, 8));
  guiGrp.add(fbPolyN.set("fbPolyN", 7, 5, 10));
  guiGrp.add(fbPolySigma.set("fbPolySigma", 1.5, 1.1, 2));
  guiGrp.add(fbUseGaussian.set("fbUseGaussian", false));
  guiGrp.add(fbWinSize.set("winSize", 32, 4, 64));
}

void nebulaFlow::update(ofPixels &img){
  if(!enabled) return;
  flow.setPyramidScale(fbPyrScale);
  flow.setNumLevels(fbLevels);
  flow.setWindowSize(fbWinSize);
  flow.setNumIterations(fbIterations);
  flow.setPolyN(fbPolyN);
  flow.setPolySigma(fbPolySigma);
  flow.setUseGaussian(fbUseGaussian);

  flow.calcOpticalFlow(img);
}

void nebulaFlow::draw(int x, int y, int w, int h){
  if(!enabled) return;
  flow.draw(x,y,w,h);
}
