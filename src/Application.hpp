#pragma once
#include <QtGui/QFont>
#include <QtGui/QFontDatabase>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <functional>
#include <unordered_map>

class Calculator : public QMainWindow {
  Q_OBJECT

public:
  explicit Calculator(QWidget *parent = nullptr);

private:
  QLabel *previewDisplay; // Shows ongoing calculation
  QLineEdit *display;     // Shows current number/result
  QFont fontAwesome;      // Font Awesome font

  double result = 0.0;
  QString operation;
  bool waitingForNumber = true;
  QString calculationHistory; // Tracks the full expression

  // Font Awesome Unicode mappings
  struct FontAwesome {
    static constexpr const char *BACKSPACE = "\uf55a"; // fa-backspace
    static constexpr const char *PLUS = "\uf067";      // fa-plus
    static constexpr const char *MINUS = "\uf068";     // fa-minus
    static constexpr const char *TIMES = "\uf00d";     // fa-times
    static constexpr const char *DIVIDE = "\uf529";    // fa-divide
    static constexpr const char *EQUALS = "\uf52c";    // fa-equals
    static constexpr const char *PERCENT = "\uf295";   // fa-percent
    static constexpr const char *PLUS_MINUS =
        "\uf067\uf068";                          // Custom combination
    static constexpr const char *DOT = "\uf192"; // fa-dot-circle
    // Numbers 0-9 using Font Awesome number icons
    static constexpr const char *NUM_0 =
        "0"; // Keep as regular text for clarity
    static constexpr const char *NUM_1 = "1";
    static constexpr const char *NUM_2 = "2";
    static constexpr const char *NUM_3 = "3";
    static constexpr const char *NUM_4 = "4";
    static constexpr const char *NUM_5 = "5";
    static constexpr const char *NUM_6 = "6";
    static constexpr const char *NUM_7 = "7";
    static constexpr const char *NUM_8 = "8";
    static constexpr const char *NUM_9 = "9";
  };

  // Function lookup table with Font Awesome icons
  std::unordered_map<QString, std::function<void()>> actions = {
      // Digits
      {"0", [this] { inputDigit("0"); }},
      {"1", [this] { inputDigit("1"); }},
      {"2", [this] { inputDigit("2"); }},
      {"3", [this] { inputDigit("3"); }},
      {"4", [this] { inputDigit("4"); }},
      {"5", [this] { inputDigit("5"); }},
      {"6", [this] { inputDigit("6"); }},
      {"7", [this] { inputDigit("7"); }},
      {"8", [this] { inputDigit("8"); }},
      {"9", [this] { inputDigit("9"); }},

      // Operations
      {".",
       [this] {
         if (waitingForNumber) {
           display->setText("0.");
         } else if (!display->text().contains('.')) {
           display->setText(display->text() + ".");
         }
         waitingForNumber = false;
       }},

      {"±",
       [this] {
         const auto value = -display->text().toDouble();
         display->setText(QString::number(value));
         updatePreview();
       }},

      {"AC",
       [this] {
         display->setText("0");
         result = 0.0;
         operation = "";
         calculationHistory = "";
         waitingForNumber = true;
         previewDisplay->setText("");
         updatePreview();
       }},

      {"%",
       [this] {
         const auto value = display->text().toDouble();
         display->setText(QString::number(value / 100.0));
         waitingForNumber = true;
         updatePreview();
       }},

      {"C",
       [this] {
         display->setText("0");
         waitingForNumber = true;
         updatePreview();
       }},

      {"CE",
       [this] {
         display->setText("0");
         result = 0.0;
         operation = "";
         calculationHistory = "";
         waitingForNumber = true;
         updatePreview();
       }},

      {"←",
       [this] {
         const auto text = display->text();
         display->setText(text.length() > 1 ? text.left(text.length() - 1)
                                            : "0");
         if (display->text() == "0") {
           waitingForNumber = true;
         }
         updatePreview();
       }},

      {"=",
       [this] {
         if (!operation.isEmpty()) {
           calculate();
           operation = "";
           calculationHistory = "";
           waitingForNumber = true;
           previewDisplay->setText("");
         }
       }},

      {"+", [this] { performOperation("+"); }},
      {"-", [this] { performOperation("-"); }},
      {"×", [this] { performOperation("×"); }},
      {"÷", [this] { performOperation("÷"); }}};

  void loadFontAwesome() {
    // Load Font Awesome from resources or file
    int fontId = QFontDatabase::addApplicationFont(":/fonts/fa-solid-900.ttf");
    if (fontId == -1) {
      // Fallback: try loading from fonts directory
      fontId = QFontDatabase::addApplicationFont("fonts/fa-solid-900.ttf");
    }

    if (fontId != -1) {
      QStringList fontFamilies = QFontDatabase::applicationFontFamilies(fontId);
      if (!fontFamilies.isEmpty()) {
        fontAwesome = QFont(fontFamilies.at(0));
        fontAwesome.setPixelSize(16);
      }
    }
  }

  QString getButtonIcon(const QString &buttonText) {
    // Map button text to Font Awesome icons
    if (buttonText == "←")
      return FontAwesome::BACKSPACE;
    if (buttonText == "+")
      return FontAwesome::PLUS;
    if (buttonText == "-")
      return FontAwesome::MINUS;
    if (buttonText == "×")
      return FontAwesome::TIMES;
    if (buttonText == "÷")
      return FontAwesome::DIVIDE;
    if (buttonText == "=")
      return FontAwesome::EQUALS;
    if (buttonText == "%")
      return FontAwesome::PERCENT;
    if (buttonText == "±")
      return "±"; // Keep as text or create custom icon
    if (buttonText == ".")
      return "•"; // Use bullet point for decimal

    // For numbers and other operations, keep as text
    return buttonText;
  }

  void inputDigit(const QString &digit) {
    if (waitingForNumber || display->text() == "0") {
      display->setText(digit);
      waitingForNumber = false;
    } else {
      display->setText(display->text() + digit);
    }
    updatePreview();
  }

  void calculate() {
    if (!operation.isEmpty()) {
      const auto value = display->text().toDouble();
      if (operation == "+") {
        result = result + value;
      } else if (operation == "-") {
        result = result - value;
      } else if (operation == "×") {
        result = result * value;
      } else if (operation == "÷" && (value != 0.0)) {
        result = result / value;
      } else if (operation == "÷" && (value == 0.0)) {
        // Handle division by zero
        display->setText("Error");
        previewDisplay->setText("Cannot divide by zero");
        calculationHistory = "";
        operation = "";
        waitingForNumber = true;
        return;
      }
      display->setText(formatNumber(result));
    }
  }

  void performOperation(const QString &op) {
    if (!operation.isEmpty()) {
      // Chain operations: complete previous calculation
      calculationHistory += " " + display->text() + " " + operation;
      calculate();
    } else {
      // First operation: start new calculation
      result = display->text().toDouble();
      calculationHistory = display->text();
    }
    calculationHistory += " " + op;
    operation = op;
    waitingForNumber = true;
    updatePreview();
  }

  void updatePreview() {
    if (calculationHistory.isEmpty()) {
      previewDisplay->setText("");
    } else if (operation.isEmpty()) {
      // Show completed calculation
      previewDisplay->setText(calculationHistory);
    } else {
      // Show ongoing calculation
      QString preview = calculationHistory;
      if (!waitingForNumber) {
        preview += " " + display->text();
      }
      previewDisplay->setText(preview);
    }
  }

  QString formatNumber(double number) {
    // Format number to avoid unnecessary decimals
    if (number == (long long)number) {
      return QString::number((long long)number);
    } else {
      return QString::number(number, 'g', 10);
    }
  }
};