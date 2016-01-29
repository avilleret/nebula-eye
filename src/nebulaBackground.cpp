
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
  guiGrp.add(showBgsubGui.set("show bgsub parameters",true));

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
  bgsubGui.setup();

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
      m_fgbg = cv::Algorithm::create<cv::BackgroundSubtractorGMG>(m_bgsub_algos[0]);
    } else {
      m_fgbg = cv::Algorithm::create<cv::BackgroundSubtractor>(m_bgsub_algos[0]);
    }
    if (m_fgbg.empty() /* && m_fgbgGMG.empty() */)
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

  if (!m_fgbg.empty()){
    (*m_fgbg)(input, m_fgmask);
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
  if ( showBgsubGui )
    bgsubGui.draw();
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

  if ( !m_fgbg.empty() ) m_fgbg.release();

  if ( algoGMG ) m_algoName = "BackgroundSubtractor.GMG";
  else if ( algoMOG ) m_algoName = "BackgroundSubtractor.MOG";
  else if ( algoMOG2 ) m_algoName = "BackgroundSubtractor.MOG2";
  else m_algoName = "Common";

  ofLogVerbose("nebulaBackground") << "Try to load algo " << m_algoName;

   for ( int i = 0; i < m_bgsub_algos.size(); i++){
    if (  m_bgsub_algos[i] == m_algoName ){
      if ( m_bgsub_algos[i] == "BackgroundSubtractor.GMG"){
        m_fgbg = cv::Algorithm::create<cv::BackgroundSubtractorGMG>(m_bgsub_algos[i]);
      } else {
        m_fgbg = cv::Algorithm::create<cv::BackgroundSubtractor>(m_bgsub_algos[i]);
      }
      if (m_fgbg.empty() )
      {
        ofLogError("nebulaBackground") << "Failed to create " <<  m_bgsub_algos[i] << " algo.";
      } else {
        ofLogVerbose("nebulaBackground") << "bgsub " << i << " : " << m_bgsub_algos[i] << " created.";
      }
    }
  }
  initBgsubGui();
}

void nebulaBackground::save(){
  bgsubGui.saveToFile(m_algoName + ".xml");
}

void nebulaBackground::initBgsubGui(){
  bgsubGui.clear();
  bgsubGui.setName(m_algoName);
  vector<string> paramList;
  bgsubParameters.clear();

  if (!m_fgbg.empty()){
    m_fgbg->getParams(paramList);
    for (size_t i=0; i < paramList.size(); i++){
      ofLogVerbose("nebulaBackground") << paramList[i] << " type : " << m_fgbg->paramType(paramList[i]);
      ofLogVerbose("nebulaBackground") << m_fgbg->paramHelp(paramList[i]);
      float defaultValue(0.);

      switch ( m_fgbg->paramType(paramList[i]) ){
        case cv::Param::BOOLEAN:
          defaultValue = static_cast<float>(m_fgbg->getBool(paramList[i]));
          //bgsubParameters.emplace_back(new ofParameter<bool>());
          //ofAbstractParameter* p = bgsubParameters[i];
          //bgsubGui.add(static_cast<ofParameter<bool> >(bgsubParameters[i])->set(paramList[i]));
          break;
        case cv::Param::REAL:
        case cv::Param::FLOAT:
          defaultValue = static_cast<float>(m_fgbg->getDouble(paramList[i]));
          //bgsubParameters.emplace_back(new ofParameter<float>());
          //bgsubGui.add(bgsubParameters[i].set(paramList[i]));
          break;
        case cv::Param::UNSIGNED_INT:
        case cv::Param::UINT64:
        case cv::Param::UCHAR:
          defaultValue = static_cast<float>(m_fgbg->getInt(paramList[i]));
          //bgsubParameters.emplace_back(new ofParameter<int>());
          //bgsubGui.add(bgsubParameters[i].set(paramList[i]));
          break;
        default:
          break;
      }

      bgsubParameters.emplace_back();
      bgsubGui.add(bgsubParameters[i].set(paramList[i],
                                          defaultValue,
                                          defaultValue!=0 ? defaultValue/4 : 0,
                                          defaultValue!=0 ? defaultValue*4 : 100));
      }
  } else {
    ofLogVerbose("nebulaBackground") << 0 << " Learning time";
    ofLogVerbose("nebulaBackground") << 1 << " Treshold";
    paramList.push_back("Learning time");
    paramList.push_back("Threshold");
  }

  ofLogVerbose("nebulaBackground") << "algo " << m_algoName << " has " << paramList.size() << " parameter(s).";


  bgsubGui.loadFromFile(m_algoName + ".xml");
}
