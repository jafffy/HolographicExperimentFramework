#include <cstdio>

#include <iostream>
#include <memory>

#include <Windows.h>
#include <tchar.h>

#include <D3D11.h>

#include <spdlog/spdlog.h>

#include "HolographicGraphicsEngine.h"

#include "Config.h"

namespace spd = spdlog;

static auto g_IsRunning = false;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        g_IsRunning = false;
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

class HolographicApp_Win32_Impl
{
    friend class HolographicApp_Win32;

    ID3D11DeviceContext* pImmediateContext = nullptr;
    IDXGISwapChain* pSwapChain = nullptr;
    ID3D11RenderTargetView* pRenderTargetView = nullptr;
};

HolographicApp_Win32::HolographicApp_Win32(int argc, char** argv)
{
    impl = new HolographicApp_Win32_Impl();

    auto console = spd::stderr_color_mt("console");

    WNDCLASSEX wcex;
    wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= GetModuleHandle(nullptr);
	wcex.hIcon			= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
	wcex.hCursor		= LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground	= reinterpret_cast<HBRUSH>(COLOR_WINDOW+1);
	wcex.lpszMenuName	= nullptr;
	wcex.lpszClassName	= Config::Window::WindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

	if (!RegisterClassEx(&wcex)) {
        console->error(Config::registerClassExFailed);

        return;
	}

	const auto hWnd = CreateWindow(
		Config::Window::WindowClass,
		Config::Window::Title,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		Config::Window::Width, Config::Window::Height,
		nullptr, nullptr, wcex.hInstance, nullptr);
	if (!hWnd) {
	    console->error(Config::createWindowFailed);

        return;
	}

	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

    console->info(Config::windowOpened);

    // DirectX Initialize
    RECT rc;
    GetClientRect(hWnd, &rc);
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
    sd.OutputWindow = hWnd;
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

    console->info(Config::DirectX::InitializeSuccess);

    console->info(Config::enterRenderingLoop);
	MSG msg = { 0, };
    g_IsRunning = true;

	while (g_IsRunning) {
		if (PeekMessage(&msg, hWnd, 0, 0, PM_REMOVE))
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

    console->info(Config::DirectX::ReleaseSuccess);
	UnregisterClass(Config::Window::WindowClass, wcex.hInstance);

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
