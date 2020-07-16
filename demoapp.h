#pragma once
#define UNICODE
#define _UNICODE
#include <windows.h>

#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>

#include <cstdlib>
#include <malloc.h>
#include <memory.h>
#include <cwchar>
#include <cmath>

template<class Interface>
inline void SafeRelease(Interface** ppInterfaceToRelease)
{
  if(*ppInterfaceToRelease != nullptr)
  {
    (*ppInterfaceToRelease)->Release();
    (*ppInterfaceToRelease) = nullptr;
  }
}

#ifndef Assert
#if defined(DEBUG) || defined(_DEBUG) 
#define Assert(b) do {if (!(b)) { OutputDebugStringA("Assert: " #b "\n"); }} while(0)
#else
#define Assert(b)
#endif // DEBUG, _DEBUG
#endif // Assert

#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

class DemoApp
{
public:
  DemoApp();
  ~DemoApp();

public:
  HRESULT Initialize();
  void RunMessageLoop();

private:
  HRESULT CreateDeviceIndependentResources();
  HRESULT CreateDeviceResources();
  void DiscardDeviceResources();
  HRESULT OnRender();
  void OnResize(UINT width, UINT height);
  static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
  HWND hwnd;
  ID2D1Factory* pDirect2dFactory;
  ID2D1HwndRenderTarget* pRenderTarget;
  ID2D1SolidColorBrush* pLightSlateGrayBrush;
  ID2D1SolidColorBrush* pCornflowerBlueBrush;
};