#include "nebulaEye.h"

void nebulaEye::setup()
{
  client.setup("127.0.0.1",0);
  client.addVideoChannel(5000);

  gui.setup("","settings.xml",660,10);
  gui.add(client.parameters);

  client.play();
}

void nebulaEye::update()
{
  client.update();
  if(client.isFrameNewVideo()){
    remoteVideo.loadData(client.getPixelsVideo());
  }
}

void nebulaEye::draw()
{
  ofSetColor(255);
  remoteVideo.draw(0,0);
  gui.draw();
}

void nebulaEye::exit()
{
}
