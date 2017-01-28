#pragma once

class HolographicApp_Win32_Impl;

class HolographicApp_Win32 {
public:
    HolographicApp_Win32(int argc, char **argv);

    bool Initialize() const;
    void Release();

    void Run();

private:
    void Render() const;

    HolographicApp_Win32_Impl* impl = nullptr;
};