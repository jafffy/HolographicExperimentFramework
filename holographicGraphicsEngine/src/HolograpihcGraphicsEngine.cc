#include <cstdio>

#include <iostream>
#include <memory>

#include <D3D11.h>

#include <spdlog/spdlog.h>

#include "Config.h"

#include "HolographicGraphicsEngine.h"

#include "internal/win32/Win32Impl.h"
#include "internal/win32/Direct3D11Impl.h"

namespace spd = spdlog;

class HolographicApp_Win32_Impl
{
    friend class HolographicApp_Win32;
    Internal::Win32Impl* win32Impl = nullptr;
    Internal::Direct3D11Impl* direct3D11Impl = nullptr;

    HolographicApp_Win32_Impl()
    {
        win32Impl = new Internal::Win32Impl();
        direct3D11Impl = new Internal::Direct3D11Impl();
    }

    ~HolographicApp_Win32_Impl()
    {
        if ( win32Impl )
        {
            delete win32Impl;
            win32Impl = nullptr;
        }

        if ( direct3D11Impl )
        {
            delete direct3D11Impl;
            direct3D11Impl = nullptr;
        }
    }

    HWND hWnd() const { return win32Impl->hWnd(); }
    ID3D11DeviceContext* ImmediateContext() const
    {
        return direct3D11Impl->ImmediateContext();
    }
    IDXGISwapChain* SwapChain() const
    {
        return direct3D11Impl->SwapChain();
    }
    ID3D11RenderTargetView* RenderTargetView() const
    {
        return direct3D11Impl->RenderTargetView();
    }
};

HolographicApp_Win32::HolographicApp_Win32(int argc, char** argv)
{
    impl = new HolographicApp_Win32_Impl();

    impl->win32Impl->Initialize();
    impl->direct3D11Impl->Initialize(impl->win32Impl->hWnd());

    auto console = spd::stderr_color_mt(Config::Log::console);

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
    impl->ImmediateContext()->ClearRenderTargetView(impl->RenderTargetView(), ClearColor);
    impl->SwapChain()->Present(0, 0);
}
