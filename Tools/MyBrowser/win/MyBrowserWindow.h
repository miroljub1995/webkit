#pragma once

#include "stdafx.h"

class MyBrowserWindow {

	static std::wstring s_windowClass;
    HWND m_hMainWnd;

	//LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    void registerClass(HINSTANCE hInstance);

    public:
     MyBrowserWindow(HINSTANCE hInstance);

	 HWND hwnd();


    ~MyBrowserWindow();
};
