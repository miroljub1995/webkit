/*
 * Copyright (C) 2006, 2008, 2013-2015 Apple Inc.  All rights reserved.
 * Copyright (C) 2009, 2011 Brent Fulgham.  All rights reserved.
 * Copyright (C) 2009, 2010, 2011 Appcelerator, Inc. All rights reserved.
 * Copyright (C) 2013 Alex Christensen. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#include "stdafx.h"
#include "Common.h"

#include <dbghelp.h>
#include <shlobj.h>
#include <wtf/StdLibExtras.h>

// Global Variables:
HINSTANCE hInst;

// Support moving the transparent window
POINT s_windowPosition = { 100, 100 };
SIZE s_windowSize = { 500, 200 };

namespace WebCore {
float deviceScaleFactorForWindow(HWND);
}

void computeFullDesktopFrame()
{
    RECT desktop;
    if (!::SystemParametersInfo(SPI_GETWORKAREA, 0, static_cast<void*>(&desktop), 0))
        return;

    float scaleFactor = WebCore::deviceScaleFactorForWindow(nullptr);

    s_windowPosition.x = 0;
    s_windowPosition.y = 0;
    s_windowSize.cx = scaleFactor * (desktop.right - desktop.left);
    s_windowSize.cy = scaleFactor * (desktop.bottom - desktop.top);
}

BOOL WINAPI DllMain(HINSTANCE dllInstance, DWORD reason, LPVOID)
{
    if (reason == DLL_PROCESS_ATTACH)
        hInst = dllInstance;

    return TRUE;
}

bool getAppDataFolder(_bstr_t& directory)
{
    wchar_t appDataDirectory[MAX_PATH];
    if (FAILED(SHGetFolderPathW(0, CSIDL_LOCAL_APPDATA | CSIDL_FLAG_CREATE, 0, 0, appDataDirectory)))
        return false;

    wchar_t executablePath[MAX_PATH];
    if (!::GetModuleFileNameW(0, executablePath, MAX_PATH))
        return false;

    ::PathRemoveExtensionW(executablePath);

    directory = _bstr_t(appDataDirectory) + L"\\" + ::PathFindFileNameW(executablePath);

    return true;
}

CommandLineOptions parseCommandLine()
{
    CommandLineOptions options;

    int argc = 0;
    WCHAR** argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    for (int i = 1; i < argc; ++i) {
        if (!wcsicmp(argv[i], L"--transparent"))
            options.usesLayeredWebView = true;
        else if (!wcsicmp(argv[i], L"--desktop"))
            options.useFullDesktop = true;
        else if (!wcsicmp(argv[i], L"--performance"))
            options.pageLoadTesting = true;
        else if (!wcsicmp(argv[i], L"--wk1") || !wcsicmp(argv[i], L"--legacy"))
            options.windowType = BrowserWindowType::WebKitLegacy;
#if ENABLE(WEBKIT)
        else if (!wcsicmp(argv[i], L"--wk2") || !wcsicmp(argv[i], L"--webkit"))
            options.windowType = BrowserWindowType::WebKit;
#endif
        else if (!options.requestedURL)
            options.requestedURL = argv[i];
    }

    return options;
}

std::wstring replaceString(std::wstring src, const std::wstring& oldValue, const std::wstring& newValue)
{
    if (src.empty() || oldValue.empty())
        return src;

    size_t pos = 0;
    while ((pos = src.find(oldValue, pos)) != src.npos) {
        src.replace(pos, oldValue.length(), newValue);
        pos += newValue.length();
    }

    return src;
}
