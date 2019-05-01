set(MyBrowser_INCLUDE_DIRECTORIES
    ${WTF_FRAMEWORK_HEADERS_DIR}
    ${JavaScriptCore_FRAMEWORK_HEADERS_DIR}
    ${JavaScriptCore_PRIVATE_FRAMEWORK_HEADERS_DIR}
    ${PAL_FRAMEWORK_HEADERS_DIR}
    ${WebCore_PRIVATE_FRAMEWORK_HEADERS_DIR}
    ${WebKitLegacy_FRAMEWORK_HEADERS_DIR}
    ${WebKit_FRAMEWORK_HEADERS_DIR}
    ${WebKit_PRIVATE_FRAMEWORK_HEADERS_DIR}
)

set(MyBrowser_SOURCES
    AccessibilityDelegate.cpp
    BrowserWindow.cpp
    Common.cpp
    DOMDefaultImpl.cpp
    MainWindow.cpp
    PageLoadTestClient.cpp
    PrintWebUIDelegate.cpp
    ResourceLoadDelegate.cpp
    WebDownloadDelegate.cpp
    MiniBrowserLib.rc
    MiniBrowserWebHost.cpp
    WebKitLegacyBrowserWindow.cpp
    WinMain.cpp
    stdafx.cpp
)

set(MyBrowser_LIBRARIES
    DbgHelp
    WTF
    WebKitLegacy
    WebKitLegacyGUID
    comctl32
    comsupp
    comsuppw
    shlwapi
)

if (USE_CF)
    list(APPEND MyBrowser_LIBRARIES
        ${COREFOUNDATION_LIBRARY}
    )
endif ()

if (ENABLE_WEBKIT)
    add_definitions(-DENABLE_WEBKIT)
    list(APPEND MyBrowser_SOURCES
        WebKitBrowserWindow.cpp
    )
    list(APPEND MyBrowser_LIBRARIES WebKit)
endif ()

set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${MSVC_RUNTIME_LINKER_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:wWinMainCRTStartup")

if (${WTF_PLATFORM_WIN_CAIRO})
    add_definitions(-DWIN_CAIRO)
endif ()
add_definitions(-D_UNICODE)
include_directories(${MyBrowser_INCLUDE_DIRECTORIES})
add_library(MyBrowserLib SHARED ${MyBrowser_SOURCES})
target_link_libraries(MyBrowserLib ${MyBrowser_LIBRARIES})

add_executable(MyBrowser WIN32 ${TOOLS_DIR}/win/DLLLauncher/DLLLauncherMain.cpp MiniBrowser.rc)
target_link_libraries(MyBrowser shlwapi)
set_target_properties(MyBrowser PROPERTIES OUTPUT_NAME "MyBrowser")

add_dependencies(MyBrowser MyBrowserLib)
