#include "MyBrowserWindow.h"
#include "MyBrowserLibResource.h"
/*
static std::wstring loadString(int id)
{
    constexpr size_t length = 100;
    wchar_t buff[length];
    LoadString(hInst, id, buff, length);
    return buff;
}*/

std::wstring MyBrowserWindow::s_windowClass;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
void MyBrowserWindow::registerClass(HINSTANCE hInstance)
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

MyBrowserWindow::MyBrowserWindow(HINSTANCE hInstance)
{
    registerClass(hInstance);

    std::wstring title = L"My Browser";

    m_hMainWnd = CreateWindow(s_windowClass.c_str(), title.c_str(), WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, 0, 0, hInstance, nullptr);
}

HWND MyBrowserWindow::hwnd()
{
    return m_hMainWnd;
}

MyBrowserWindow::~MyBrowserWindow()
{
}
