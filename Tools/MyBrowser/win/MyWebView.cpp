#include "stdafx.h"

#include "MyWebView.h"
#include "WTF/Assertions.h"
#include "WebCore/EmptyClients.h"
#include "WebCore/PageConfiguration.h"
#include "WebCore/LayoutMilestone.h"

using namespace WebCore;


MyWebView::MyWebView(HWND mainWnd, const RECT& clientRect, HINSTANCE hInstance)
    : m_mainWnd(mainWnd)
{
    HRESULT hr = S_OK;

    registerWebViewWindowClass(hInstance);

    m_viewWnd = CreateWindowEx(0, m_myWebViewWindowClassName, 0, WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
        clientRect.left, clientRect.top, clientRect.right - clientRect.left, clientRect.bottom - clientRect.top, m_mainWnd ? m_mainWnd : HWND_MESSAGE, 0, hInstance, this);
    ASSERT(::IsWindow(m_viewWnd));

    if (shouldInitializeTrackPointHack()) {
        // If we detected a registry key belonging to a TrackPoint driver, then create fake trackpoint
        // scrollbars, so the WebView will receive WM_VSCROLL and WM_HSCROLL messages. We create one
        // vertical scrollbar and one horizontal to allow for receiving both types of messages.
        ::CreateWindowW(L"SCROLLBAR", L"FAKETRACKPOINTHSCROLLBAR", WS_CHILD | WS_VISIBLE | SBS_HORZ, 0, 0, 0, 0, m_viewWnd, 0, hInstance, 0);
        ::CreateWindowW(L"SCROLLBAR", L"FAKETRACKPOINTVSCROLLBAR", WS_CHILD | WS_VISIBLE | SBS_VERT, 0, 0, 0, 0, m_viewWnd, 0, hInstance, 0);
    }

    m_page = new Page(pageConfigurationWithEmptyClients());
    m_page->addLayoutMilestones({ DidFirstLayout, DidFirstVisuallyNonEmptyLayout });
    m_frame = &m_page->mainFrame();
    m_frame->init();
    WebCore::Document *doc = m_page->mainFrame().document();
}

bool MyWebView::shouldInitializeTrackPointHack()
{
    static bool shouldCreateScrollbars;
    static bool hasRunTrackPointCheck;

    if (hasRunTrackPointCheck)
        return shouldCreateScrollbars;

    hasRunTrackPointCheck = true;
    const WCHAR trackPointKeys[][50] = { L"Software\\Lenovo\\TrackPoint",
        L"Software\\Lenovo\\UltraNav",
        L"Software\\Alps\\Apoint\\TrackPoint",
        L"Software\\Synaptics\\SynTPEnh\\UltraNavUSB",
        L"Software\\Synaptics\\SynTPEnh\\UltraNavPS2" };

    for (int i = 0; i < 5; ++i) {
        HKEY trackPointKey = nullptr;
        LSTATUS readKeyResult = ::RegOpenKeyExW(HKEY_CURRENT_USER, trackPointKeys[i], 0, KEY_READ, &trackPointKey);
        ::RegCloseKey(trackPointKey);
        if (readKeyResult == ERROR_SUCCESS) {
            shouldCreateScrollbars = true;
            return shouldCreateScrollbars;
        }
    }

    return shouldCreateScrollbars;
}

bool MyWebView::registerWebViewWindowClass(HINSTANCE hInstance)
{
    static bool haveRegisteredWindowClass = false;
    if (haveRegisteredWindowClass)
        return true;

    haveRegisteredWindowClass = true;

    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_DBLCLKS;
    wcex.lpfnWndProc = WebViewWndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = sizeof(MyWebView*);
    wcex.hInstance = hInstance;
    wcex.hIcon = 0;
    wcex.hCursor = ::LoadCursor(0, IDC_ARROW);
    wcex.hbrBackground = 0;
    wcex.lpszMenuName = 0;
    wcex.lpszClassName = m_myWebViewWindowClassName;
    wcex.hIconSm = 0;

    return !!RegisterClassEx(&wcex);
}

LRESULT CALLBACK MyWebView::WebViewWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    MyWebView* thisView = reinterpret_cast<MyWebView*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    switch (message) {
    case WM_CREATE:
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(reinterpret_cast<LPCREATESTRUCT>(lParam)->lpCreateParams));
        break;
    case WM_DESTROY:
        SetWindowLongPtr(hWnd, GWLP_USERDATA, 0);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

WebCore::PageConfiguration MyWebView::pageConfiguration()
{
    return WebCore::pageConfigurationWithEmptyClients();
}