// TitleBarCustomizer.h
#ifndef TITLEBARCUSTOMIZER_H
#define TITLEBARCUSTOMIZER_H

#include <QtCore/QObject>
#include <QtGui/QWindow>
#include <QtWidgets/QWidget>

#ifdef Q_OS_WIN
#include <dwmapi.h>
#include <windows.h>
#pragma comment(lib, "dwmapi.lib")
#endif

#ifdef Q_OS_LINUX
#include <QtCore/QLoggingCategory>
#include <QtGui/QGuiApplication>
// Only include X11 headers if explicitly available
#ifdef QT_FEATURE_xcb
#include <xcb/xcb.h>
#endif
#endif

// Cross-platform title bar customization functions
void customizeWindowsTitleBar(WId windowId);
void customizeMacTitleBar(WId windowId);
void customizeLinuxTitleBar(WId windowId, QWidget *widget = nullptr);

// Main function to customize title bar based on platform
inline void customizeTitleBar(WId windowId, QWidget *widget = nullptr) {
#ifdef Q_OS_WIN
  customizeWindowsTitleBar(windowId);
#elif defined(Q_OS_MAC)
  customizeMacTitleBar(windowId);
#elif defined(Q_OS_LINUX)
  customizeLinuxTitleBar(windowId, widget);
#endif
}

#ifdef Q_OS_WIN
inline void customizeWindowsTitleBar(WId windowId) {
  if (windowId) {
    HWND hwnd = (HWND)windowId;
    // Set title bar background color
    COLORREF titleBarColor = RGB(45, 45, 55);
    DwmSetWindowAttribute(hwnd, DWMWA_CAPTION_COLOR, &titleBarColor,
                          sizeof(titleBarColor));
    // Set title bar text color to white
    COLORREF textColor = RGB(255, 255, 255);
    DwmSetWindowAttribute(hwnd, DWMWA_TEXT_COLOR, &textColor,
                          sizeof(textColor));
    // Optional: Enable dark mode for window chrome
    BOOL darkMode = TRUE;
    DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &darkMode,
                          sizeof(darkMode));
  }
}
#else
inline void customizeWindowsTitleBar(WId windowId) { Q_UNUSED(windowId); }
#endif

#ifdef Q_OS_LINUX
inline void customizeLinuxTitleBar(WId windowId, QWidget *widget) {
  if (!widget)
    return;

  // Disable problematic transparency attributes for Linux
  widget->setAttribute(Qt::WA_TranslucentBackground, false);
  widget->setAttribute(Qt::WA_NoSystemBackground, false);

  // Set a solid background instead of transparency
  widget->setStyleSheet(R"(
        QMainWindow {
            background: rgb(45, 45, 55);
            border-radius: 8px;
            border: 1px solid rgba(70, 70, 70, 0.8);
        }
    )");

  // Remove window opacity that causes full transparency
  if (auto *window = widget->window()) {
    window->setWindowOpacity(1.0);
  }

  // Basic X11 window customization without QNativeInterface
#ifdef QT_FEATURE_xcb
  if (QGuiApplication::platformName() == "xcb") {
    // Basic X11 window management without private Qt interfaces
    // Most customization should be handled through desktop themes
    // and the stylesheet above
  }
#endif

  Q_UNUSED(windowId); // Suppress unused parameter warning
}
#else
inline void customizeLinuxTitleBar(WId windowId, QWidget *widget) {
  Q_UNUSED(windowId);
  Q_UNUSED(widget);
}
#endif

#endif // TITLEBARCUSTOMIZER_H