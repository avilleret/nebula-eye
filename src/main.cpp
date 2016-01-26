#include "ofApp.h"

int main()
{
  ofGLFWWindowSettings settings;
  settings.width = 1280;
  settings.height = 960;
  auto window = ofCreateWindow(settings);
  auto app = make_shared<nebulaEye>();
  ofRunApp(window, app);

  return ofRunMainLoop();
}
