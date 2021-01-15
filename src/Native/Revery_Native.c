#include <stdio.h>

#include <caml/alloc.h>
#include <caml/callback.h>
#include <caml/memory.h>
#include <caml/mlvalues.h>
#include <string.h>

#include "config.h"
#ifdef USE_WIN32
#include "ReveryWin32.h"
#include <combaseapi.h>
#include <windows.h>
#include <commctrl.h>
#elif USE_COCOA
#include "ReveryCocoa.h"
#import "ReveryAppDelegate.h"
#elif USE_GTK
#include "ReveryGtk.h"
#include <gtk/gtk.h>
#endif
#include "utilities.h"

CAMLprim value revery_initializeApp() {
#ifdef USE_COCOA
    SDLAppDelegate *sdlDelegate = [NSApp delegate];
    ReveryAppDelegate *delegate = [ReveryAppDelegate newWithSDLDelegate:sdlDelegate];
    [NSApp setDelegate:delegate];
#elif USE_WIN32
    HRESULT hr = CoInitialize(NULL);
    if (hr != S_OK) {
        fprintf(stderr, "WARNING: COM initialization failed.");
    }
#elif USE_GTK
    gtk_init(0, NULL);
#endif
    return Val_unit;
}

CAMLprim value revery_uninitializeApp() {
#ifdef USE_WIN32
    CoUninitialize();
#endif
    return Val_unit;
}


CAMLprim value revery_initializeWindow(value vWin) {
    CAMLparam1(vWin);
    void *win = (void *)revery_unwrapPointer(vWin);
#ifdef USE_WIN32
    /* This flag often gets unset when the window decoration is removed.
       This Chromium comment is the source of this fix:
       https://chromium.googlesource.com/chromium/src.git/+/46.0.2478.0/chrome/browser/ui/views/apps/chrome_native_app_window_views_win.cc#71
    */
    HWND window = (HWND)win;
    int currentStyle = GetWindowLong(window, GWL_STYLE);
    SetWindowLong(window, GWL_STYLE, currentStyle | WS_CAPTION);
    inputSetMainWindow_win32(window);

    INITCOMMONCONTROLSEX icc;
    icc.dwSize = sizeof(icc);
    icc.dwICC = ICC_STANDARD_CLASSES;
    InitCommonControlsEx(&icc);

    MessageBox(NULL, TEXT("Error registering window class."), TEXT("Error"), MB_ICONERROR | MB_OK);
#else
    UNUSED(win);
#endif
    CAMLreturn(Val_unit);
}
