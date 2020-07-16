#include "demoapp.h"

DemoApp::DemoApp()
  : 
  hwnd(nullptr), pDirect2dFactory(nullptr), pRenderTarget(nullptr),
  pLightSlateGrayBrush(nullptr), pCornflowerBlueBrush(nullptr)
{}

DemoApp::~DemoApp()
{
  SafeRelease(&pDirect2dFactory);
  SafeRelease(&pRenderTarget);
  SafeRelease(&pLightSlateGrayBrush);
  SafeRelease(&pCornflowerBlueBrush);
}

void DemoApp::RunMessageLoop()
{  
  MSG msg;
  while(GetMessage(&msg, nullptr, 0, 0))
  {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
}

HRESULT DemoApp::Initialize()
{
  HRESULT hr;
  hr = CreateDeviceIndependentResources();

  if(SUCCEEDED(hr))
  {
    WNDCLASSEX wcx = { 0 };
    wcx.cbSize = sizeof(WNDCLASSEX);
    wcx.style = CS_HREDRAW | CS_VREDRAW;
    wcx.lpfnWndProc = DemoApp::WndProc;
    wcx.cbWndExtra  = sizeof(LONG_PTR);
    wcx.hInstance = HINST_THISCOMPONENT;
    wcx.hCursor = LoadCursor(nullptr, IDI_APPLICATION);
    wcx.lpszClassName = L"D2DDemoApp";

    RegisterClassEx(&wcx);

    float dpiX;
    float dpiY;

    pDirect2dFactory->GetDesktopDpi(&dpiX, &dpiY);

    hwnd = CreateWindow(
      L"D2DDemoApp", L"Direct2D Demo App", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
      (UINT)ceil(640.f * dpiX / 96.f), (UINT)ceil(640.f * dpiY / 96.f), nullptr, nullptr, 
      HINST_THISCOMPONENT, this);

    hr = hwnd ? S_OK : E_FAIL;
    if(SUCCEEDED(hr))
    {
      ShowWindow(hwnd, SW_SHOWNORMAL);
      UpdateWindow(hwnd);
    }
  }

  return hr;
}

HRESULT DemoApp::CreateDeviceIndependentResources()
{
  HRESULT hr = S_OK;

  hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pDirect2dFactory);

  return hr;
}

HRESULT DemoApp::CreateDeviceResources()
{
  HRESULT hr;

  if(!pRenderTarget)
  {
    RECT rc;
    GetClientRect(hwnd, &rc);

    D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

    hr = pDirect2dFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), 
    D2D1::HwndRenderTargetProperties(hwnd, size), &pRenderTarget);

    if(SUCCEEDED(hr))
    {
      hr = pRenderTarget->CreateSolidColorBrush(
        D2D1::ColorF(D2D1::ColorF::LightSlateGray), &pLightSlateGrayBrush);
    }

    if(SUCCEEDED(hr))
    {
      hr = pRenderTarget->CreateSolidColorBrush(
        D2D1::ColorF(D2D1::ColorF::CornflowerBlue), &pCornflowerBlueBrush);
    }
  }

  return hr;
}

void DemoApp::DiscardDeviceResources()
{
  SafeRelease(&pRenderTarget);
  SafeRelease(&pLightSlateGrayBrush);
  SafeRelease(&pCornflowerBlueBrush);
}

void DemoApp::OnResize(UINT width, UINT height)
{
  if(pRenderTarget)
  {
    pRenderTarget->Resize(D2D1::SizeU(width, height));
  }
}

HRESULT DemoApp::OnRender()
{
  HRESULT hr = S_OK;

  hr = CreateDeviceResources();

  if(SUCCEEDED(hr))
  {
    pRenderTarget->BeginDraw();
    pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
    pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

    D2D1_SIZE_F rtSize = pRenderTarget->GetSize();

    int width = (int)rtSize.width;
    int height = (int)rtSize.height;

    for(int x = 0; x < width; x += 10)
    {
      pRenderTarget->DrawLine(D2D1::Point2F((float)x, 0.f),
        D2D1::Point2F((float)x, rtSize.height), pLightSlateGrayBrush, 0.5f);
    }

    for(int y = 0; y < height; y += 10)
    {
      pRenderTarget->DrawLine(D2D1::Point2F(0.f, (float)y),
        D2D1::Point2F(rtSize.width, (float)y), pLightSlateGrayBrush, 0.5f);
    }

    D2D1_RECT_F rectangle1 = D2D1::RectF(
      rtSize.width / 2 - 50.f, rtSize.height / 2 - 50.f,
      rtSize.width / 2 + 50.f, rtSize.height / 2 + 50.f);

      D2D1_RECT_F rectangle2 = D2D1::RectF(
        rtSize.width / 2 - 100.f, rtSize.height / 2 - 100.f,
        rtSize.width / 2 + 100.f, rtSize.height / 2 + 100.f);

    pRenderTarget->FillRectangle(&rectangle1, pLightSlateGrayBrush);
    pRenderTarget->DrawRectangle(&rectangle2, pCornflowerBlueBrush);

    hr = pRenderTarget->EndDraw();

    if(hr == D2DERR_RECREATE_TARGET)
    {
      hr = S_OK;
      DiscardDeviceResources();
    }
  }

  return hr;
}

LRESULT CALLBACK DemoApp::WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  LRESULT result = 0;

  if(msg == WM_CREATE)
  {
    CREATESTRUCT* pcs = (CREATESTRUCT*)lparam;
    DemoApp* pDemoApp = (DemoApp*)pcs->lpCreateParams;

    SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pDemoApp);

    result = 1;
  }
  else
  {
    DemoApp* pDemoApp = (DemoApp*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

    bool wasHandled = false;

    if(pDemoApp)
    {
      switch(msg)
      {
        case WM_SIZE:
        {
          UINT width = LOWORD(lparam);
          UINT height = LOWORD(lparam);
          pDemoApp->OnResize(width, height);
        }
          result = 0;
          wasHandled = true;
        break;

        case WM_DISPLAYCHANGE:
        {
          InvalidateRect(hwnd, nullptr, FALSE);
        }
          result = 0;
          wasHandled = true;
        break;

        case WM_PAINT:
        {
          pDemoApp->OnRender();
          ValidateRect(hwnd, nullptr);
        }
          result = 0;
          wasHandled = 0;
        break;

        case WM_DESTROY:
          PostQuitMessage(0);
          result = 1;
          wasHandled = true;
        break;
      }
    }

    if(!wasHandled)
    {
      result = DefWindowProc(hwnd, msg, wparam, lparam);
    }
  }

  return result;
}