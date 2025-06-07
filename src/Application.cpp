#include "Application.hpp"
#include "TitleBarCustomizer.h"
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>

Calculator::Calculator(QWidget *parent) : QMainWindow(parent) {
  // Load Font Awesome first
  loadFontAwesome();

  setCentralWidget([this] {
    auto *widget = new QWidget;
    auto *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->setSpacing(4);

    // Preview display (shows ongoing calculation)
    previewDisplay = new QLabel("");
    previewDisplay->setAlignment(Qt::AlignRight);
    previewDisplay->setStyleSheet(R"(QLabel {
              background: transparent;
              color: rgba(255, 255, 255, 0.7);
              font-size: 14px;
              font-weight: 300;
              padding: 4px 16px 0px 16px;
              margin: 0px;
          })");
    previewDisplay->setMinimumHeight(24);
    previewDisplay->setMaximumHeight(24);
    previewDisplay->setWordWrap(false);
    previewDisplay->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    layout->addWidget(previewDisplay);

    // Main display
    display = new QLineEdit("0");
    display->setReadOnly(true);
    display->setAlignment(Qt::AlignRight);

    // Platform-specific display styling
#ifdef Q_OS_LINUX
    display->setStyleSheet(R"(QLineEdit {
              background: rgba(60, 60, 70, 0.95);
              color: white;
              font-size: 32px;
              font-weight: 500;
              border-radius: 8px;
              border: 1px solid rgba(80, 80, 90, 0.8);
              padding: 12px 4px;
              margin: 0px;
          })");
#else
    display->setStyleSheet(R"(QLineEdit {
              background: rgba(42, 42, 53, 0.85);
              color: white;
              font-size: 32px;
              font-weight: 500;
              border-radius: 8px;
              padding: 12px 4px;
              margin: 0px;
          })");
#endif

    display->setMinimumHeight(60);
    layout->addWidget(display);

    // Buttons with Font Awesome icons
    auto *grid = new QGridLayout;
    grid->setSpacing(6);
    grid->setContentsMargins(0, 0, 0, 0);

    // Button configuration: {display_text, icon_text, row, col, type}
    const std::vector<std::tuple<QString, QString, int, int, QString>> buttons =
        {{"AC", "AC", 0, 0, "function"},
         {"±", "±", 0, 1, "function"},
         {"%", QString(FontAwesome::PERCENT), 0, 2, "function"},
         {"÷", QString(FontAwesome::DIVIDE), 0, 3, "operator"},

         {"7", "7", 1, 0, "number"},
         {"8", "8", 1, 1, "number"},
         {"9", "9", 1, 2, "number"},
         {"×", QString(FontAwesome::TIMES), 1, 3, "operator"},

         {"4", "4", 2, 0, "number"},
         {"5", "5", 2, 1, "number"},
         {"6", "6", 2, 2, "number"},
         {"-", QString(FontAwesome::MINUS), 2, 3, "operator"},

         {"1", "1", 3, 0, "number"},
         {"2", "2", 3, 1, "number"},
         {"3", "3", 3, 2, "number"},
         {"+", QString(FontAwesome::PLUS), 3, 3, "operator"},

         {"←", QString(FontAwesome::BACKSPACE), 4, 0, "function"},
         {"0", "0", 4, 1, "number"},
         {".", "•", 4, 2, "number"},
         {"=", QString(FontAwesome::EQUALS), 4, 3, "operator"}};

    for (const auto &[actionKey, displayText, row, col, type] : buttons) {
      auto *button = new QPushButton(displayText);

      // Set Font Awesome font for icon buttons
      bool useIconFont = (displayText == QString(FontAwesome::BACKSPACE) ||
                          displayText == QString(FontAwesome::PLUS) ||
                          displayText == QString(FontAwesome::MINUS) ||
                          displayText == QString(FontAwesome::TIMES) ||
                          displayText == QString(FontAwesome::DIVIDE) ||
                          displayText == QString(FontAwesome::EQUALS) ||
                          displayText == QString(FontAwesome::PERCENT));

      if (useIconFont && !fontAwesome.family().isEmpty()) {
        button->setFont(fontAwesome);
      }

      // Set fixed size for circular buttons
      button->setFixedSize(38, 38);

      QString buttonStyle;
      if (type == "number") {
#ifdef Q_OS_LINUX
        buttonStyle = R"(QPushButton {
                      background: rgba(90, 90, 95, 0.95);
                      color: white;
                      border: 1px solid rgba(110, 110, 115, 0.8);
                      border-radius: 19px;
                      font-size: 18px;
                      font-weight: 400;
                  }
                  QPushButton:hover {
                      background: rgba(115, 115, 120, 1.0);
                      border: 1px solid rgba(130, 130, 135, 0.9);
                  }
                  QPushButton:pressed {
                      background: rgba(129, 129, 135, 1.0);
                      border: 1px solid rgba(70, 70, 75, 1.0);
                  })";
#else
        buttonStyle = R"(QPushButton {
                      background: rgba(81, 81, 83, 0.85);
                      color: white;
                      border: none;
                      border-radius: 19px;
                      font-size: 18px;
                      font-weight: 400;
                  }
                  QPushButton:hover {
                      background: rgba(115, 115, 115, 0.9);
                  }
                  QPushButton:pressed {
                      background: rgba(129, 129, 132, 0.95);
                      border: 1px solid rgba(70, 70, 70, 1.0);
                  })";
#endif
      } else if (type == "function") {
#ifdef Q_OS_LINUX
        buttonStyle = R"(QPushButton {
                      background: rgba(120, 120, 125, 0.95);
                      color: white;
                      border: 1px solid rgba(140, 140, 145, 0.8);
                      border-radius: 19px;
                      font-size: 17px;
                      font-weight: 500;
                  }
                  QPushButton:hover {
                      background: rgba(160, 160, 165, 1.0);
                      border: 1px solid rgba(180, 180, 185, 0.9);
                  }
                  QPushButton:pressed {
                      background: rgba(142, 142, 147, 1.0);
                      border: 1px solid rgba(130, 130, 135, 1.0);
                  })";
#else
        buttonStyle = R"(QPushButton {
                      background: rgba(111, 112, 115, 0.85);
                      color: white;
                      border: none;
                      border-radius: 19px;
                      font-size: 17px;
                      font-weight: 500;
                  }
                  QPushButton:hover {
                      background: rgba(180, 180, 180, 0.9);
                  }
                  QPushButton:pressed {
                      background: rgba(142, 142, 144, 0.95);
                      border: 1px solid rgba(130, 130, 130, 1.0);
                  })";
#endif
      } else { // operator
        buttonStyle = R"(QPushButton {
                      background: rgba(255, 149, 0, 0.9);
                      color: white;
                      border: none;
                      border-radius: 19px;
                      font-size: 18px;
                      font-weight: 500;
                  }
                  QPushButton:hover {
                      background: rgba(255, 165, 30, 0.95);
                  }
                  QPushButton:pressed {
                      background: rgba(230, 133, 14, 1.0);
                      border: 1px solid rgba(200, 110, 0, 1.0);
                  })";
      }

      button->setStyleSheet(buttonStyle);
      grid->addWidget(button, row, col);

      // Connect button to action using the actionKey
      connect(button, &QPushButton::clicked, [this, actionKey] {
        if (actions.contains(actionKey)) {
          actions[actionKey]();
        }
      });
    }

    layout->addLayout(grid);
    return widget;
  }());

  // Set window properties
  setFixedSize(200, 320);

  // Platform-specific window setup
#ifdef Q_OS_LINUX
  // Linux: Use solid background instead of transparency
  setWindowFlags(Qt::Window);
  // Don't set translucent background attributes on Linux

#elif defined(Q_OS_MAC)
  // macOS: Keep existing transparency behavior
  setAttribute(Qt::WA_TranslucentBackground, true);
  setAttribute(Qt::WA_NoSystemBackground, true);
  setWindowFlags(Qt::Window);
  setWindowOpacity(0.99);

#elif defined(Q_OS_WIN)
  // Windows: Enable transparency
  setAttribute(Qt::WA_TranslucentBackground, true);
  setAttribute(Qt::WA_NoSystemBackground, true);
  setWindowFlags(Qt::Window);
  setWindowOpacity(0.99);
#endif

  // Customize title bar color using cross-platform helper
  customizeTitleBar(winId(), this);

  // Platform-specific main window styling
#ifdef Q_OS_LINUX
  setStyleSheet(R"(
          QMainWindow {
              background: rgb(45, 45, 55);
              border-radius: 8px;
              border: 1px solid rgba(70, 70, 70, 0.8);
          }
      )");
#else
  setStyleSheet(R"(
          QMainWindow {
              background: rgba(45, 45, 55, 0.85);
              border-radius: 8px;
          }
      )");
#endif
}