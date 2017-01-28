#include <cstdio>

#include <iostream>
#include <memory>

#include <D3D11.h>

#include <spdlog/spdlog.h>

#include "Config.h"

#include "HolographicGraphicsEngine.h"

#include "internal/win32/Win32Impl.h"

namespace spd = spdlog;

namespace Internal
{
    class Direct3D11Impl
    {
        friend class HologrpahicApp_Win32;

    };
}

class HolographicApp_Win32_Impl
{
    friend class HolographicApp_Win32;

    ID3D11DeviceContext* pImmediateContext = nullptr;
    IDXGISwapChain* pSwapChain = nullptr;
    ID3D11RenderTargetView* pRenderTargetView = nullptr;

    Internal::Win32Impl* win32Impl = nullptr;

    HolographicApp_Win32_Impl()
    {
        win32Impl = new Internal::Win32Impl();
    }

    ~HolographicApp_Win32_Impl()
    {
        if ( win32Impl )
        {
            delete win32Impl;
            win32Impl = nullptr;
        }
    }
};

HolographicApp_Win32::HolographicApp_Win32(int argc, char** argv)
{
    impl = new HolographicApp_Win32_Impl();

    impl->win32Impl->Initialize();

    auto console = spd::stderr_color_mt(Config::Log::console);

    // DirectX Initialize
    RECT rc;
    GetClientRect(impl->win32Impl->hWnd(), &rc);
    UINT width = rc.right - rc.left;
    UINT height = rc.bottom - rc.top;

    UINT createDeviceFlags = 0;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_NULL;
    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
    ID3D11Device* pd3dDevice = nullptr;

    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE
    };
    UINT numDriverTypes = ARRAYSIZE(driverTypes);

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0
    };
    UINT numFeatureLevels = ARRAYSIZE(featureLevels);

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = width;
    sd.BufferDesc.Height = height;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = impl->win32Impl->hWnd();
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;

    HRESULT hr = S_FALSE;

    for ( UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; ++driverTypeIndex )
    {
        driverType = driverTypes[driverTypeIndex];
        hr = D3D11CreateDeviceAndSwapChain(nullptr, driverType, nullptr,
            createDeviceFlags, featureLevels, numFeatureLevels,
            D3D11_SDK_VERSION, &sd, &impl->pSwapChain, &pd3dDevice,
            &featureLevel, &impl->pImmediateContext);
    }
    if ( FAILED(hr) )
        return;

    ID3D11Texture2D* pBackBuffer = nullptr;
    hr = impl->pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&pBackBuffer));
    if ( FAILED(hr) )
        return;

    hr = pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &impl->pRenderTargetView);
    if ( FAILED(hr) )
        return;

    impl->pImmediateContext->OMSetRenderTargets(1, &impl->pRenderTargetView, nullptr);

    D3D11_VIEWPORT vp;
    vp.Width = static_cast<FLOAT>(width);
    vp.Height = static_cast<FLOAT>(height);
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    impl->pImmediateContext->RSSetViewports(1, &vp);

    console->info(Config::Log::DirectX::InitializeSuccess);

    console->info(Config::Log::enterRenderingLoop);
    MSG msg = { 0, };

    impl->win32Impl->Run();

    while ( impl->win32Impl->IsRunning() ) {
        if ( PeekMessage(&msg, impl->win32Impl->hWnd(), 0, 0, PM_REMOVE) )
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            Render();
        }
    }

    if ( impl->pImmediateContext ) impl->pImmediateContext->ClearState();
    if ( impl->pRenderTargetView ) impl->pRenderTargetView->Release();
    if ( impl->pSwapChain ) impl->pSwapChain->Release();
    if ( impl->pImmediateContext ) impl->pImmediateContext->Release();
    if ( pd3dDevice ) pd3dDevice->Release();

    console->info(Config::Log::DirectX::ReleaseSuccess);

    impl->win32Impl->Release();

    Release();
}

void HolographicApp_Win32::Release()
{
    if ( impl ) {
        delete impl; impl = nullptr;
    }
}


void HolographicApp_Win32::Render() const
{
    float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f };
    impl->pImmediateContext->ClearRenderTargetView(impl->pRenderTargetView, ClearColor);
    impl->pSwapChain->Present(0, 0);
}
