#include "AppHost.h"

#include "MetroBoyApp.h"
#include "GateBoyApp.h"
#include "DummyApp.h"

#pragma warning(disable:4100)
#pragma warning(disable:4189)

void run_microtests();

//-----------------------------------------------------------------------------

int main(int argc, char** argv) {

#if 1
  {
    GateBoy gateboy;
    gateboy.run_benchmark(false);
    printf("\n");
  }
  {
    GateBoy gateboy;
    gateboy.run_benchmark(true);
    printf("\n");
  }
#else
  App* app = new GateBoyApp();
  //App* app = new DummyApp();
  //App* app = new MetroBoyApp();

  AppHost* app_host = new AppHost(app);
  int ret = app_host->app_main(argc, argv);
  delete app;
  return ret;
#endif
}