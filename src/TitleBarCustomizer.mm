// TitleBarCustomizer.mm
#include "TitleBarCustomizer.h"

#ifdef Q_OS_MAC
#import <Cocoa/Cocoa.h>
#import <QuartzCore/QuartzCore.h>

void customizeMacTitleBar(WId windowId) {
  NSView *nsview = reinterpret_cast<NSView *>(windowId);
  NSWindow *nswindow = [nsview window];

  if (nswindow != nullptr) {
    // Make title bar transparent and blend with content
    nswindow.titlebarAppearsTransparent = YES;

    // Set the background color to match calculator theme
    nswindow.backgroundColor = [NSColor colorWithRed:42.0 / 255.0
                                               green:42.0 / 255.0
                                                blue:52.0 / 255.0
                                               alpha:0.99];

    // Optional: Hide title text for cleaner look
    nswindow.titleVisibility = NSWindowTitleHidden;

    // Optional: Make window slightly blurred for modern macOS look
    NSVisualEffectView *effectView = [[NSVisualEffectView alloc] init];
    effectView.material = NSVisualEffectMaterialHUDWindow;
    effectView.state = NSVisualEffectStateActive;
    effectView.blendingMode = NSVisualEffectBlendingModeBehindWindow;

    // Set the effect view as the content view's superview background
    [nswindow.contentView setWantsLayer:YES];
    nswindow.contentView.layer.backgroundColor =
        [[NSColor colorWithRed:42.0 / 255.0
                         green:42.0 / 255.0
                          blue:52.0 / 255.0
                         alpha:0.99] CGColor];
  }
}

#else
void customizeMacTitleBar(WId windowId) {
  // No-op on non-macOS platforms
  Q_UNUSED(windowId)
}
#endif