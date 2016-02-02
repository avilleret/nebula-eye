/*********************************/
/* Motion flow computation class */
/*********************************/
#include "nebulaFlow.h"
#include "nebulaUtils.h"

void nebulaFlow::setup(){
    // setup flow parameter GUI
  guiGrp.setName("Motion flow parameters");
  guiGrp.add(enabled.set("enable",true));
  guiGrp.add(forceCPU.set("Force CPU", false));
  guiGrp.add(fbPyrScale.set("fbPyrScale", .5, 0, .99));
  guiGrp.add(fbLevels.set("fbLevels", 4, 1, 8));
  guiGrp.add(fbIterations.set("fbIterations", 2, 1, 8));
  guiGrp.add(fbPolyN.set("fbPolyN", 7, 5, 10));
  guiGrp.add(fbPolySigma.set("fbPolySigma", 1.5, 1.1, 2));
  guiGrp.add(fbUseGaussian.set("fbUseGaussian", false));
  guiGrp.add(fbWinSize.set("winSize", 32, 4, 64));

  // OpenCL setup
  cv::initModule_video();
  cv::setUseOptimized(true);
  cv::setNumThreads(8);

  cv::ocl::DevicesInfo devicesInfo;
  cv::ocl::getOpenCLDevices(devicesInfo);
  ofLogNotice("nebulaFlow") << "Found " << devicesInfo.size() << " OpenCL device(s).";
  for ( size_t i = 0; i < devicesInfo.size(); i++){
    ofLogNotice("nebulaFlow") << devicesInfo[i]->deviceVendor << " " << devicesInfo[i];
  }

  if ( devicesInfo.size() == 0 ){
    ofLogNotice("nebulaFlow") << "can't find OpenCL device, switch to CPU mode";
    gpuMode = false;
  } else {
    gpuMode = true;
  }
}

void nebulaFlow::update(ofPixels &img){
  if(!enabled) return;

  if ( gpuMode ){
    oclFbFlow.pyrScale=fbPyrScale;
    oclFbFlow.numLevels=fbLevels;
    oclFbFlow.winSize=fbWinSize;
    oclFbFlow.numIters=fbIterations;
    oclFbFlow.polyN=fbPolyN;
    oclFbFlow.polySigma=fbPolySigma;

    cv::Mat input = ofxCv::toCv(img), gray;
    if(input.channels() == 1){
      gray = input;
    } else {
        cv::cvtColor(input, gray, CV_RGB2GRAY);
    }
    d_input = gray;
    if ( m_flow.size()  != d_input.size() ) m_flow.create(d_input.size(), CV_32FC2);
    if ( d_flow.size()  != d_input.size() ) d_flow.create(d_input.size(), CV_32FC2);
    if ( d_prev.size()  != d_input.size() ) d_prev.create(d_input.size(), CV_8UC1);
    if ( d_flowx.size() != d_input.size() ) d_flowx.create(d_input.size(), CV_32FC1);
    if ( d_flowy.size() != d_input.size() ) d_flowy.create(d_input.size(), CV_32FC1);
    oclFbFlow(d_prev, d_input, d_flowx, d_flowy);
    const vector<cv::ocl::oclMat> matVec = { d_flowx, d_flowy };
    cv::ocl::merge(matVec, d_flow);
    d_flow.download(m_flow);
    d_input.copyTo(d_prev);

  } else {
    flow.setPyramidScale(fbPyrScale);
    flow.setNumLevels(fbLevels);
    flow.setWindowSize(fbWinSize);
    flow.setNumIterations(fbIterations);
    flow.setPolyN(fbPolyN);
    flow.setPolySigma(fbPolySigma);
    flow.setUseGaussian(fbUseGaussian);

    flow.calcOpticalFlow(img);
    m_flow = flow.getFlow();
  }
}

void nebulaFlow::draw(int x, int y, int w, int h){
  if(!enabled) return;
  if ( gpuMode ){
    ofDrawBitmapStringHighlight("GPU Mode", ofPoint(x+10,y+10), ofColor(255,0,0), ofColor(255,255,255));
    ofRectangle rect(x,y,w,h);
    ofVec2f offset(rect.x,rect.y);
    ofVec2f scale(rect.width/m_flow.cols, rect.height/m_flow.rows);
    int stepSize = 4; //TODO: make class-level parameteric
    for(int y = 0; y < m_flow.rows; y += stepSize) {
      for(int x = 0; x < m_flow.cols; x += stepSize) {
        ofVec2f cur = ofVec2f(x, y) * scale + offset;
        const cv::Vec2f& vec = m_flow.at<cv::Vec2f>(y, x);
        ofDrawLine(cur, ofVec2f(x + vec[0], y + vec[1]) * scale + offset);
      }
    }
  } else {
    flow.draw(x,y,w,h);
  }
}

double nebulaFlow::getFlowInMask(cv::Mat mask, cv::Mat * subFlow){
  if ( m_flow.size() != mask.size() ) return 0.;
  if ( subFlow ){
    m_flow.copyTo(*subFlow, mask);
  }

  return cv::norm(m_flow, cv::NORM_L2, mask);
}
