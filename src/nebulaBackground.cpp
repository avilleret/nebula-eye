
/********************************/
/* Background substractor class */
/********************************/
#include "nebulaBackground.h"

void nebulaBackground::setup(){
  ofSetLogLevel(OF_LOG_VERBOSE);
  guiGrp.setName("Background substraction");
  guiGrp.add(enabled.set("enable",true));
  guiGrp.add(forceCPU.set("force CPU", false));
  guiGrp.add(threshold.set("threshold", 60, 0, 255));
  guiGrp.add(algoClassic.set("use common algo", false));
  guiGrp.add(algoGMG.set("use GMG algo", true));
  guiGrp.add(algoMOG.set("use MOG algo",false));
  guiGrp.add(algoMOG2.set("use MOG2 algo",false));
  guiGrp.add(showBgsubGui.set("show bgsub parameters",true));

  threshold.addListener(this, &nebulaBackground::thresholdChanged);
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

  // BackgroundSubtractor algorithms initialization
  vector<string> algorithms;
  cv::Algorithm::getList(algorithms);
  string model = "BackgroundSubtractor.";

  for (size_t i=0; i < algorithms.size(); i++){
    if ( model.compare(0,model.length(),algorithms[i],0,model.length()) == 0 ){ // && npos >= model.length() ){
        m_bgsub_algos.push_back(algorithms[i]);
    }
  }

  if ( m_bgsub_algos.size() == 0){
    ofLogError("nebulaBackground") << "Can't find any background subtractor algorithm.";
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

  // OpenCL initialization
  cv::ocl::DevicesInfo devicesInfo;
  cv::ocl::getOpenCLDevices(devicesInfo);
  ofLogNotice("nebulaBackground") << "Found " << devicesInfo.size() << " OpenCL device(s).";
  for ( size_t i = 0; i < devicesInfo.size(); i++){
    ofLogNotice("nebulaBackground") << devicesInfo[i]->deviceVendor << " " << devicesInfo[i];
  }

  if ( devicesInfo.size() == 0 ){
    ofLogNotice("nebulaBackground") << "can't find OpenCL device, switch to CPU mode";
    gpuMode = false;
  } else {
    gpuMode = true;
  }
}

void nebulaBackground::update(cv::Mat& input){
  if(!enabled) return;

  ofImage img;
  ofxCv::toOf(input,img);

  if ( !thresholded.isAllocated() || img.getWidth() != thresholded.getWidth() || img.getHeight() != thresholded.getHeight() ){
    thresholded.allocate(img.getWidth(), img.getHeight(), OF_IMAGE_COLOR_ALPHA);
  }

  double time = ofClamp(learningTime*learningTime/10.,0.,1.);

  if ( gpuMode && !forceCPU ) {
    try  {
      d_input = input;
      if ( m_algoName.substr(21) == "MOG" ){
        m_oclMOG( d_input, d_fgmask, time );
      } else if ( m_algoName.substr(21) == "MOG2" ){
        m_oclMOG2( d_input, d_fgmask, time );
      } else {
        ofLogError("nebulaBackground") << "there is no GPU version of algo " << m_algoName;
        gpuMode = false;
        return;
      }
      d_fgmask.download(m_fgmask);
    } catch (cv::Exception& e) {
      ofLogError("nebulaBackground") << "can't use OpenCL, do you have OpenCL driver installed ?";
      ofLogError("nebulaBackground") << "error " << e.code << " : " << e.err;
      gpuMode = false;
      return;
    }
  } else if (!m_fgbg.empty()){
    try {
      (*m_fgbg)(input, m_fgmask, time);
    } catch (cv::Exception e) {
      ofLogError("nebulaBackground") << "OpenCV error : " << e.code << " " << e.err << " : " << e.what();
      return;
    }
   } else {
    background.update(img, m_fgmask);
  }
  ofImage bg;
  cv::Mat inv = ~m_fgmask;
  ofxCv::toOf(inv, bg);
    
  thresholded.getPixels().setChannel(4,bg);
  thresholded.update();
}

void nebulaBackground::draw(int x, int y, int w, int h){
  if(!enabled) return;
  if ( thresholded.isAllocated() )
    thresholded.draw(x,y,w,h);
  if ( showBgsubGui && m_showGui )
    bgsubGui.draw();
  if ( gpuMode )
    ofDrawBitmapStringHighlight("GPU Mode", ofPoint(x+10,y+10), ofColor(255,0,0), ofColor(255,255,255));
}

void nebulaBackground::learningTimeChanged(float & t){
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
  if( m_algoName != "Common"){
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
          gpuMode = true;
        }
      }
    }
  }
  initBgsubGui();
}

void nebulaBackground::initBgsubGui(){
  bgsubGui.clear();
  bgsubGui.setName(m_algoName);
  vector<string> paramList;
  bgsubParameters.clear();
  bgsubGui.add(learningTime.set("learningTime", 10, 0, 100));
  learningTime.removeListener(this, &nebulaBackground::learningTimeChanged);

  if( m_algoName == "Common" ){
    learningTime.addListener(this, &nebulaBackground::learningTimeChanged);
    // pass
  } else if ( m_algoName.substr(21) == "MOG" ){
    learningTime.setMax(1.);
    learningTime.setMin(0.);
    learningTime = 0.001;
  } else if ( m_algoName.substr(21) == "MOG2" ) {
    learningTime.setMax(1.);
    learningTime.setMin(0.);
    learningTime = 0.1;
  } else if ( m_algoName.substr(21) == "GMG") {
    learningTime.setMax(1.);
    learningTime.setMin(0.);
    learningTime = 0.1;
  }

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
      // set min and max to avoid crash
      if (m_algoName.substr(21) == "MOG" ){
        if ( bgsubParameters[i].getName() == "nmixtures") {
          bgsubParameters[i].setMax(11);
          bgsubParameters[i].setMin(0);
        } if ( bgsubParameters[i].getName() == "backgroundRatio") {
          bgsubParameters[i].setMax(1);
          bgsubParameters[i].setMin(0);
        }
      }

      if ( "initializationFrames" == bgsubParameters[i].getName() ){
        bgsubParameters[i].setMax(1000);
        bgsubParameters[i] = 100;
      }

      bgsubParameters[i].addListener(this, &nebulaBackground::parameterChanged);
    }
  } else {
    ofLogVerbose("nebulaBackground") << 0 << " LearningTime";
    ofLogVerbose("nebulaBackground") << 1 << " Treshold";
    paramList.push_back("LearningTime"); // TODO move those pararmeters from global panel to algo one
    paramList.push_back("Threshold");
  }

  ofLogVerbose("nebulaBackground") << "algo " << m_algoName << " has " << paramList.size() << " parameter(s).";


  bgsubGui.loadFromFile(m_algoName + ".xml");
}

void nebulaBackground::saveAlgoParam(){
  ofLogVerbose("nebulaBackground") << "save algo parameter to file";
  bgsubGui.saveToFile(m_algoName + ".xml");
}

void nebulaBackground::parameterChanged(float& v){
  ofLogVerbose("nebulaBackground") << "--- update algo paramters" << std::endl;
  for (size_t i=0; i < bgsubParameters.size(); i++){
      ofLogVerbose("nebulaBackground") << bgsubParameters[i].getName() << " : " << bgsubParameters[i];
      if(gpuMode){
        if ( m_algoName.substr(21) == "MOG" ){
            if ( "backgroundRatio" == bgsubParameters[i].getName() ){
              m_oclMOG.backgroundRatio=bgsubParameters[i];
            } else if  ( "history" == bgsubParameters[i].getName() ){
              m_oclMOG.history=bgsubParameters[i];
            } else if  ( "noiseSigma" == bgsubParameters[i].getName() ){
              m_oclMOG.noiseSigma=bgsubParameters[i];
            } else if  ( "nmixtures" == bgsubParameters[i].getName() ){
              bgsubParameters[i]=int(bgsubParameters[i]);
              m_oclMOG.varThreshold=bgsubParameters[i];
            }
        } else if ( m_algoName.substr(21) == "MOG2" ){
            if ( "backgroundRatio" == bgsubParameters[i].getName() ){
              m_oclMOG2.backgroundRatio=bgsubParameters[i];
            } else if  ( "history" == bgsubParameters[i].getName() ){
              m_oclMOG2.history=bgsubParameters[i];
            } else if  ( "detectShadows" == bgsubParameters[i].getName() ){
              m_oclMOG2.bShadowDetection=bgsubParameters[i];
            }  else if ( "nShadowDetection" == bgsubParameters[i].getName() ){
                // m_oclMOG2.nShadowDetection=bgsubParameters[i];
                // m_oclMOG2.nShadowDetection=0;
            } else if  ( "fCT" == bgsubParameters[i].getName() ){
              m_oclMOG2.fCT=bgsubParameters[i];
            } else if  ( "fTau" == bgsubParameters[i].getName() ){
                m_oclMOG2.fTau=bgsubParameters[i];
            } else if  ( "fVarInit" == bgsubParameters[i].getName() ){
                m_oclMOG2.fVarInit=bgsubParameters[i];
            } else if  ( "fVarMax" == bgsubParameters[i].getName() ){
                m_oclMOG2.fVarMax=bgsubParameters[i];
            } else if  ( "fVarMin" == bgsubParameters[i].getName() ){
                m_oclMOG2.fVarMin=bgsubParameters[i];
            } else if  ( "varThreshold" == bgsubParameters[i].getName() ){
                m_oclMOG2.varThreshold=bgsubParameters[i];
            }
            // nmixture is missing, but used used in the MOG2 ctr
        }
      } else {
        m_fgbg->setDouble(bgsubParameters[i].getName(), bgsubParameters[i]);
      }
  }
  ofLogVerbose("nebulaBackground") << "--- parameter update done";

}

void nebulaBackground::setParameterFloat(string paramName, float value){

  if ( paramName == "learningTime")
    learningTime = value;
  else {
    for (size_t i=0; i < bgsubParameters.size(); i++){
      if (paramName == bgsubParameters[i].getName()){
        bgsubParameters[i] = value;
      }
    }
  }
}

void nebulaBackground::showGui(bool & flag){
  m_showGui = flag;
}
