// holographicGraphicsEngineExample.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <HolographicGraphicsEngine.h>

int main(int argc, char** argv)
{
    HolographicApp_Win32 app(argc, argv);

    if ( !app.Initialize() )
        return 1;

    app.Run();

    app.Release();

    return 0;
}

