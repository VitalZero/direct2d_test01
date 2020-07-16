#include "demoapp.h"
#include <memory>

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
  HeapSetInformation(nullptr, HeapEnableTerminationOnCorruption, nullptr, 0);

  if(SUCCEEDED(CoInitialize(nullptr)))
  {
    {
      std::unique_ptr<DemoApp> app = std::make_unique<DemoApp>();

      if(SUCCEEDED(app->Initialize()))
      {
        app->RunMessageLoop();
      }
    }
    CoUninitialize();
  }

  return 0;
}