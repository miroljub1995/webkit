#pragma once

#include "stdafx.h"

class MyMainWindow {

private:
    static std::wstring s_windowClass;
    HWND m_hMainWnd;

    static LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    void registerClass(HINSTANCE hInstance);
    void resizeSubViews();

public:
    MyMainWindow(HINSTANCE hInstance);
    HWND hwnd();
    HRESULT run(_In_ HACCEL hAccelTable);

    ~MyMainWindow();
};
