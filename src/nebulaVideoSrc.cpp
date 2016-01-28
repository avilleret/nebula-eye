#include "nebulaVideoSrc.h"

void nebulaVideoSrc::setup()
{
  guiGrp.setName("video source");
  guiGrp.add(client.parameters);
  guiGrp.add(srcRTP.set("RTP stream", true));
  guiGrp.add(srcMovie.set("movie", false));

  srcMovie.addListener(this, &nebulaVideoSrc::srcMovieCb);
  srcRTP.addListener(this, &nebulaVideoSrc::srcRTPCb);

  client.setup("127.0.0.1",0);
  client.addVideoChannel(5000);
  client.play();

  movie.load("video.mov");
  movie.play();
}

void nebulaVideoSrc::update(){
  if (srcMovie) movie.update();
  else if (srcRTP){
    client.update();
    if (client.isFrameNewVideo()){
      texture.loadData(client.getPixelsVideo());
    }
  }
}

void nebulaVideoSrc::draw(int x, int y, int w, int h){
  if (srcMovie) movie.draw(x,y,w,h);
  else if (srcRTP) {
    if ( texture.isAllocated() )
      texture.draw(x,y,w,h);
    }
}

bool nebulaVideoSrc::isFrameNew(){
  if (srcMovie) return movie.isFrameNew();
  else if (srcRTP) return client.isFrameNewVideo();
}

ofPixels& nebulaVideoSrc::getPixels(){
  if (srcMovie) return movie.getPixels();
  else if (srcRTP) return client.getPixelsVideo();
}

void nebulaVideoSrc::srcMovieCb(bool & flag){
  if (flag) {
    srcRTP=false;
  }
}

void nebulaVideoSrc::srcRTPCb(bool & flag){
  if (flag) {
    srcMovie=false;
  }
}
