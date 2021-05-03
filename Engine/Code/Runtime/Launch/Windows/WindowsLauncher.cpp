#include "Core.h"
#include "OS/Windows/WindowsCommons.h"

#ifdef AE_DEBUG
    #if AE_WINDOWS
        #define _CRTDBG_MAP_ALLOC
        #include <crtdbg.h>
    #endif
#endif

extern "C"
{
    __declspec(dllexport) int NvOptimusEnablement                  = 0x00000001;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

int32
wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int32 nShowCmd)
{
#ifdef AE_DEBUG
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_DEBUG);
    _CrtSetDebugFillThreshold(0);
#endif

    return EXIT_SUCCESS;
}