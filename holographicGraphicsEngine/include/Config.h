#pragma once

#include <string>

namespace Config
{
    const std::string registerClassExFailed = "Call to RegisterClassEx failed!";
    const std::string createWindowFailed = "Call to CreateWindow failed!";
    const std::string windowOpened = "Window opened";
    const std::string enterRenderingLoop = "Enter Rendering loop";

    namespace DirectX
    {
        const std::string InitializeSuccess = "DirectX initialization success";
        const std::string ReleaseSuccess = "Released DirectX related instances";
    }

    namespace Window
    {
        const auto Width = 800;
        const auto Height = 600;
        const auto Title = _T("HolographicGraphicsEngine");
        const auto WindowClass = _T("holographicGraphicsEngine");
    }
}
