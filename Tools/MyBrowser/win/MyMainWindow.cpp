
#include "stdafx.h"
#include "MyMainWindow.h"
#include "MyBrowserLibResource.h"

namespace WebCore {
float deviceScaleFactorForWindow(HWND);
}

/*
static std::wstring loadString(int id)
{
    constexpr size_t length = 100;
    wchar_t buff[length];
    LoadString(hInst, id, buff, length);
    return buff;
}*/

std::wstring MyMainWindow::s_windowClass;

LRESULT CALLBACK MyMainWindow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    MyMainWindow* thisWindow = reinterpret_cast<MyMainWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    switch (message) {
    case WM_CREATE:
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(reinterpret_cast<LPCREATESTRUCT>(lParam)->lpCreateParams));
        break;
    case WM_DESTROY:
        SetWindowLongPtr(hWnd, GWLP_USERDATA, 0);
        PostQuitMessage(0);
        break;
    case WM_SIZE:
        thisWindow->resizeSubViews();
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void MyMainWindow::resizeSubViews()
{
    float scaleFactor = WebCore::deviceScaleFactorForWindow(m_hMainWnd);

    RECT rcClient;
    GetClientRect(m_hMainWnd, &rcClient);

    //int height = scaleFactor * urlBarHeight;
    //int width = scaleFactor * controlButtonWidth;

    MoveWindow(m_browserWindow->hwnd(), rcClient.left, rcClient.top, rcClient.right, rcClient.bottom, TRUE);
}

void MyMainWindow::registerClass(HINSTANCE hInstance)
{
    static bool initialized = false;
    if (initialized)
        return;
    initialized = true;

    s_windowClass = L"MYBROWSER";

    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MYBROWSER));
    wcex.hCursor = LoadCursor(0, IDC_ARROW);
    wcex.hbrBackground = 0;
    wcex.lpszMenuName = MAKEINTRESOURCE(IDC_MYBROWSER);
    wcex.lpszClassName = s_windowClass.c_str();
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    HRESULT hr = RegisterClassEx(&wcex);
}

MyMainWindow::MyMainWindow(HINSTANCE hInstance)
{
    registerClass(hInstance);

    std::wstring title = L"My Browser";

    m_hMainWnd = CreateWindow(s_windowClass.c_str(), title.c_str(), WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, 0, 0, hInstance, this);

    m_browserWindow = MyBrowserWindow::create(m_hMainWnd, 0, hInstance, false, false);
    m_browserWindow->init();
    //m_browserWindow->loadHTMLString(BSTR(L"<html><body><div>Ovo je stranica</div></body></html>"));
    m_browserWindow->loadURL(L"https://www.google.com");
    //((WebView*)(&(m_browserWindow->m_webView)))->page()
}

HWND MyMainWindow::hwnd()
{
    return m_hMainWnd;
}

HRESULT MyMainWindow::run(_In_ HACCEL hAccelTable)
{
    MSG msg{};

    while (GetMessage(&msg, 0, 0, 0)) {
        //performMessageLoopTasks();

        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return S_OK;
}

MyMainWindow::~MyMainWindow()
{
}
