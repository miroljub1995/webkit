
#pragma once

#include <Windows.h>
#include "WebCore/config.h"
#include "WebCore/Page.h"
#include "WebCore/PageConfiguration.h"

class MyWebView {
public:
    MyWebView(HWND mainWnd, const RECT& clientRect, HINSTANCE hInstance);

private:
    HWND m_mainWnd;
    HWND m_viewWnd;
    const LPCWSTR m_myWebViewWindowClassName = L"MyWebViewWindowClass";
    WebCore::Page* m_page;
    WebCore::Frame* m_frame;

	bool shouldInitializeTrackPointHack();
    bool registerWebViewWindowClass(HINSTANCE hInstance);
    static LRESULT CALLBACK WebViewWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    WebCore::PageConfiguration pageConfiguration();
};