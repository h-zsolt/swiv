//
// Main.cpp
//

#include "pch.h"
#include "Initialize.h"
#include "ScarleGame.h"
#include "Helper.h"
#include "imgui.h"

using namespace DirectX;

namespace
{
    std::unique_ptr<ScarleGame> g_game;
};

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Indicates to hybrid graphics systems to prefer the discrete part by default
extern "C"
{
    __declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

static bool g_fullscreen = false;

int Scarle2020::Initialize(_In_ HINSTANCE _hInstance, _In_opt_ HINSTANCE _hPrevInstance, _In_ LPWSTR _lpCmdLine, _In_ int _nCmdShow, std::unique_ptr<ScarleGame> _game, const std::string& title, bool fullscreen)
{
    _nCmdShow;

#ifdef _DEBUG
        //add a Debug Console to which cout is directed whilst in DEBUG
        if (AllocConsole()) {
            freopen("CONOUT$", "w", stdout);
            SetConsoleTitle(L"Debug Console");
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED);
        }
#endif
    UNREFERENCED_PARAMETER(_hPrevInstance);
    UNREFERENCED_PARAMETER(_lpCmdLine);

    if (!XMVerifyCPUSupport())
        return 1;

    HRESULT hr = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
    if (FAILED(hr))
        return 1;

    g_game = std::move(_game);

    // Register class and create window
    {
        // Register class
        WNDCLASSEXW wcex = {};
        wcex.cbSize = sizeof(WNDCLASSEXW);
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = WndProc;
        wcex.hInstance = _hInstance;
        wcex.hIcon = LoadIconW(_hInstance, L"IDI_ICON");
        wcex.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        wcex.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
        wcex.lpszClassName = L"Scarle2020WindowClass";
        wcex.hIconSm = LoadIconW(wcex.hInstance, L"IDI_ICON");
        if (!RegisterClassExW(&wcex))
            return 1;

        // Create window
        int w, h;
        g_game->GetDefaultSize(w, h);

        RECT rc = { 0, 0, static_cast<LONG>(w), static_cast<LONG>(h) };

        AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

        g_fullscreen = fullscreen;
        HWND hwnd = NULL;
        if (!g_fullscreen)
        {
            hwnd = CreateWindowExW(0, L"Scarle2020WindowClass", Helper::charToWChar(title.c_str()), WS_OVERLAPPEDWINDOW,
                CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, _hInstance,
                nullptr);
        }
        else
        {
            hwnd = CreateWindowExW(WS_EX_TOPMOST, L"Scarle2020WindowClass", Helper::charToWChar(title.c_str()), WS_POPUP,
                CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, _hInstance,
                nullptr);
        }

        if (!hwnd)
            return 1;

        g_fullscreen ?
        ShowWindow(hwnd, SW_SHOWMAXIMIZED) :
        ShowWindow(hwnd, _nCmdShow);

        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(g_game.get()) );

        GetClientRect(hwnd, &rc);

        g_game->Initialize(hwnd, rc.right - rc.left, rc.bottom - rc.top);
    }

    // Main message loop
    MSG msg = {};
    while (WM_QUIT != msg.message)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            g_game->Tick();
        }
    }

    g_game.reset();

    CoUninitialize();

    return (int) msg.wParam;
}

// Windows procedure
LRESULT CALLBACK WndProc(HWND _hWnd, UINT _message, WPARAM _wParam, LPARAM _lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(_hWnd, _message, _wParam, _lParam)) return true;

    PAINTSTRUCT ps;
    HDC hdc;

    static bool s_in_sizemove = false;
    static bool s_in_suspend = false;
    static bool s_minimized = false;

    auto game = reinterpret_cast<ScarleGame*>(GetWindowLongPtr(_hWnd, GWLP_USERDATA));

    switch (_message)
    {
    case WM_PAINT:
        if (s_in_sizemove && game)
        {
            game->Tick();
        }
        else
        {
            hdc = BeginPaint(_hWnd, &ps);
            EndPaint(_hWnd, &ps);
        }
        break;

    case WM_SIZE:
        if (_wParam == SIZE_MINIMIZED)
        {
            if (!s_minimized)
            {
                s_minimized = true;
                if (!s_in_suspend && game)
                    game->onSuspending();
                s_in_suspend = true;
            }
        }
        else if (s_minimized)
        {
            s_minimized = false;
            if (s_in_suspend && game)
                game->onResuming();
            s_in_suspend = false;
        }
        else if (!s_in_sizemove && game)
        {
            game->onWindowSizeChanged(LOWORD(_lParam), HIWORD(_lParam));
        }
        break;

    case WM_ENTERSIZEMOVE:
        s_in_sizemove = true;
        break;

    case WM_EXITSIZEMOVE:
        s_in_sizemove = false;
        if (game)
        {
            RECT rc;
            GetClientRect(_hWnd, &rc);

            game->onWindowSizeChanged(rc.right - rc.left, rc.bottom - rc.top);
        }
        break;

    case WM_GETMINMAXINFO:
        {
            auto info = reinterpret_cast<MINMAXINFO*>(_lParam);
            info->ptMinTrackSize.x = 320;
            info->ptMinTrackSize.y = 200;
        }
        break;

    case WM_ACTIVATEAPP:
        if (game)
        {
            if (_wParam)
            {
                game->onActivated();
            }
            else
            {
                game->onDeactivated();
            }
        }
        Keyboard::ProcessMessage(_message, _wParam, _lParam);
        Mouse::ProcessMessage(_message, _wParam, _lParam);
        break;

    case WM_POWERBROADCAST:
        switch (_wParam)
        {
        case PBT_APMQUERYSUSPEND:
            if (!s_in_suspend && game)
                game->onSuspending();
            s_in_suspend = true;
            return TRUE;

        case PBT_APMRESUMESUSPEND:
            if (!s_minimized)
            {
                if (s_in_suspend && game)
                    game->onResuming();
                s_in_suspend = false;
            }
            return TRUE;
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    case WM_SYSKEYDOWN:
        if (_wParam == VK_RETURN && (_lParam & 0x60000000) == 0x20000000)
        {
            // Implements the classic ALT+ENTER fullscreen toggle
            if (g_fullscreen)
            {
                SetWindowLongPtr(_hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
                SetWindowLongPtr(_hWnd, GWL_EXSTYLE, 0);

                int width = 800;
                int height = 600;
                if (game)
                    game->GetDefaultSize(width, height);

                ShowWindow(_hWnd, SW_SHOWNORMAL);

                SetWindowPos(_hWnd, HWND_TOP, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
            }
            else
            {
                SetWindowLongPtr(_hWnd, GWL_STYLE, 0);
                SetWindowLongPtr(_hWnd, GWL_EXSTYLE, WS_EX_TOPMOST);

                SetWindowPos(_hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

                ShowWindow(_hWnd, SW_SHOWMAXIMIZED);
            }

            g_fullscreen = !g_fullscreen;
        }
        Keyboard::ProcessMessage(_message, _wParam, _lParam);
        break;

    case WM_MENUCHAR:
        // A menu is active and the user presses a key that does not correspond
        // to any mnemonic or accelerator key. Ignore so we don't produce an error beep.
        return MAKELRESULT(0, MNC_CLOSE);

    case WM_INPUT:
    case WM_MOUSEMOVE:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
    case WM_MOUSEWHEEL:
    case WM_XBUTTONDOWN:
    case WM_XBUTTONUP:
    case WM_MOUSEHOVER:
        Mouse::ProcessMessage(_message, _wParam, _lParam);
        break;

    case WM_KEYDOWN:
    case WM_KEYUP:
    case WM_SYSKEYUP:
        Keyboard::ProcessMessage(_message, _wParam, _lParam);
        break;
    }

    return DefWindowProc(_hWnd, _message, _wParam, _lParam);
}


// Exit helper
void ExitGame()
{
    PostQuitMessage(0);
}
