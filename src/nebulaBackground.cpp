
/********************************/
/* Background substractor class */
/********************************/
#include "nebulaBackground.h"

void nebulaBackground::setup(){
  ofSetLogLevel(OF_LOG_VERBOSE);
  guiGrp.setName("Background substraction");
  guiGrp.add(enabled.set("enable",true));
  guiGrp.add(learningTime.set("Learning time", 10, 0, 100));
  guiGrp.add(threshold.set("threshold", 60, 0, 255));
  guiGrp.add(algoClassic.set("use common algo", false));
  guiGrp.add(algoGMG.set("use GMG algo", true));
  guiGrp.add(algoMOG.set("use MOG algo",false));
  guiGrp.add(algoMOG2.set("use MOG2 algo",false));

  learningTime.addListener(this, &nebulaBackground::learningTimeChanged);
  threshold.addListener(this, &nebulaBackground::thresholdChanged);
  background.setLearningTime(learningTime);
  background.setThresholdValue(threshold);

  algoClassic.addListener(this, &nebulaBackground::algoClassicCb);
  algoGMG.addListener(this, &nebulaBackground::algoGMGCb);
  algoMOG.addListener(this, &nebulaBackground::algoMOGCb);
  algoMOG2.addListener(this, &nebulaBackground::algoMOG2Cb);
  // OpenCV initialization
  cv::initModule_video();
  cv::setUseOptimized(true);
  cv::setNumThreads(8);

  vector<string> algorithms;
  cv::Algorithm::getList(algorithms);
  string model = "BackgroundSubtractor.";

  for (size_t i=0; i < algorithms.size(); i++){
    if ( model.compare(0,model.length(),algorithms[i],0,model.length()) == 0 ){ // && npos >= model.length() ){
        m_bgsub_algos.push_back(algorithms[i]);
    }
  }

  if ( m_bgsub_algos.size() == 0){
    ofLogError("Can't find any background subtractor algorithm.");
  } else {
    ofLogVerbose("nebulaBackground") << "found " << m_bgsub_algos.size() << " algos.";
    for (size_t i=0; i < m_bgsub_algos.size(); i++){
      ofLogVerbose("nebulaBackground") << i << " " << m_bgsub_algos[i];
    }

    if ( m_bgsub_algos[0] == "BackgroundSubtractor.GMG"){
      m_fgbgGMG = cv::Algorithm::create<cv::BackgroundSubtractorGMG>(m_bgsub_algos[0]);
    } else {
      m_fgbgMOG = cv::Algorithm::create<cv::BackgroundSubtractor>(m_bgsub_algos[0]);
    }
    if (m_fgbgMOG.empty() && m_fgbgGMG.empty())
    {
      ofLogError("Failed to create BackgroundSubtractor Algorithm.");
    }
    initAlgo();
  }
}

void nebulaBackground::update(ofPixels &img){
  if(!enabled) return;

  // ofImage img = ofImage(px);
  cv::Mat input = ofxCv::toCv(img);

  if (!m_fgbgMOG.empty()){
    (*m_fgbgMOG)(input, m_fgmask);
    ofxCv::copy(m_fgmask, thresholded);
  } else if (!m_fgbgGMG.empty()) {
    (*m_fgbgGMG)(input, m_fgmask);
    ofxCv::copy(m_fgmask, thresholded);
  } else {
    background.update(img, thresholded);
  }
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

void nebulaBackground::algoMOGCb(bool & flag){
  if(flag){
    algoClassic = false;
    algoGMG = false;
    algoMOG2 = false;
    initAlgo();
  }
}

void nebulaBackground::algoMOG2Cb(bool & flag){
  if(flag){
    algoClassic = false;
    algoGMG = false;
    algoMOG = false;
    initAlgo();
  }
}

void nebulaBackground::algoGMGCb(bool & flag){
  if(flag){
    algoClassic = false;
    algoMOG = false;
    algoMOG2 = false;
    initAlgo();
  }
}

void nebulaBackground::algoClassicCb(bool & flag){
  if(flag){
    algoMOG = false;
    algoGMG = false;
    algoMOG2 = false;
    initAlgo();
  }
}

void nebulaBackground::initAlgo(){

  if ( !m_fgbgMOG.empty() ) m_fgbgMOG.release();
  if ( !m_fgbgGMG.empty() ) m_fgbgGMG.release();

  if ( algoGMG ) m_algoName = "BackgroundSubtractor.GMG";
  else if ( algoMOG ) m_algoName = "BackgroundSubtractor.MOG";
  else if ( algoMOG2 ) m_algoName = "BackgroundSubtractor.MOG2";
  else m_algoName = "Common";

  ofLogVerbose("nebulaBackground") << "Try to load algo " << m_algoName;

   for ( int i = 0; i < m_bgsub_algos.size(); i++){
    if (  m_bgsub_algos[i] == m_algoName ){
      if ( m_bgsub_algos[i] == "BackgroundSubtractor.GMG"){
        m_fgbgGMG = cv::Algorithm::create<cv::BackgroundSubtractorGMG>(m_bgsub_algos[i]);
      } else {
        m_fgbgMOG = cv::Algorithm::create<cv::BackgroundSubtractor>(m_bgsub_algos[i]);
      }
      if (m_fgbgMOG.empty() && m_fgbgGMG.empty())
      {
        ofLogError("nebulaBackground") << "Failed to create " <<  m_bgsub_algos[i] << " algo.";
      } else {
        ofLogVerbose("nebulaBackground") << "bgsub " << i << " : " << m_bgsub_algos[i] << " created.";
        m_algoName = m_bgsub_algos[i].substr(21);
      }
    }
  }
}
