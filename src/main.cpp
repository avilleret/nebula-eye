#include "nebulaEye.h"

int main()
{
  ofGLFWWindowSettings settings;

  auto window = ofCreateWindow(settings);
  auto app = make_shared<nebulaEye>();
  ofRunApp(window, app);

  return ofRunMainLoop();
}
