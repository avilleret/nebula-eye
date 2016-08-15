#pragma once

#include "ofxGem.h"
#include "ofxCv.h"

class nebulaPix_share {

public:
  enum source {
    SOURCE_NONE,
    SOURCE_FGMASK,
    SOURCE_FLOW
  };

  nebulaPix_share();

  void setup(std::string name);
  void setPixels(cv::Mat mat);
  void setPixels(ofPixels pix);
  source getSource(){ return m_source; }
  void setSource(source s) { m_source = s; }

private:
  ofxGem m_pix_share;
  std::string m_name;
  source m_source;
};
