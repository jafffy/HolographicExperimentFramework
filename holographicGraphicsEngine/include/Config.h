#pragma once

#include <tchar.h>

namespace Config
{
    namespace Log
    {
        const std::string win32_initialization_logger = "Win32 Initialization";
        const std::string direct3d11_initialization_logger = "Direct3D11 Initialization";
        const std::string console = "console";

        const std::string registerClassExFailed = "Call to RegisterClassEx failed!";
        const std::string createWindowFailed = "Call to CreateWindow failed!";
        const std::string windowOpened = "Window opened";
        const std::string enterRenderingLoop = "Enter Rendering loop";

        namespace DirectX
        {
            const std::string InitializeSuccess = "DirectX initialization success";
            const std::string ReleaseSuccess = "Released DirectX related instances";
        }
    }

    namespace Window
    {
        const auto Width = 800;
        const auto Height = 600;
        const auto Title = _T("HolographicGraphicsEngine");
        const auto WindowClass = _T("holographicGraphicsEngine");
    }
}
