# Simple C++ Windows Calculator (`cpp-calculator`)

[![Language](https://img.shields.io/badge/Language-C%2B%2B17-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B17)
[![Platform](https://img.shields.io/badge/Platform-Windows%2010%20%7C%2011-0078D6.svg)](https://www.microsoft.com/windows)
[![API](https://img.shields.io/badge/API-Win32-lightgrey.svg)](https://learn.microsoft.com/en-us/windows/win32/)
[![IDE](https://img.shields.io/badge/IDE-Visual%20Studio-purple.svg)](https://visualstudio.microsoft.com/)
[![License](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)

A clean, native Windows desktop calculator application built with **C++** and the **Win32 API**.

Designed to be lightweight, responsive, and completely standalone without any external framework dependencies (no .NET, no Electron, no Qt required).

---

## 📥 Download Prebuilt Release

You do **not** need Visual Studio or a compiler to run this application.

👉 **[Download the Latest Release (v1.0.0 ZIP)](../../releases/latest)**

### How to Run:
1. Download `cpp-calculator-v1.0.0-windows-x64.zip` from the [Releases](../../releases) tab.
2. Extract the ZIP archive anywhere on your PC.
3. Double-click **`Calculator.exe`** to start calculating immediately!

---

## ✨ Features

* **Basic Arithmetic Operations**:
  * Addition (`+`)
  * Subtraction (`-`)
  * Multiplication (`*`)
  * Division (`/`)
* **Number Input**:
  * Buttons from `0` to `9`.
  * Decimal point (`.`) with smart duplicate prevention.
* **Control Buttons**:
  * **`=`**: Evaluates the mathematical expression.
  * **`C`**: Clears the entire calculation and resets to `0`.
  * **`CE`**: Clears only the current entry without losing previous operands.
  * **`⌫`**: Backspace to delete the last entered digit.
  * **`±`**: Toggles positive / negative sign.
* **Safe Division by Zero**:
  * Displays `"Cannot divide by zero"` in clear crimson text without crashes or undefined behavior.
* **Two-Tier Display Screen**:
  * **Expression history line**: Displays the pending operation (e.g. `12 + 5 =`).
  * **Main display line**: Displays the large, bold result or currently entered number.
* **Modern Windows UI & High-DPI Support**:
  * Clean Segoe UI typography and double-buffered flicker-free rendering.
  * Modern styled buttons via Windows Common Controls v6.
* **Full Keyboard Support**:
  * Type numbers and operators directly from your keyboard or numpad.

---

## ⌨️ Keyboard Shortcuts

| Key | Calculator Action |
| :--- | :--- |
| **0 – 9** | Enter digits |
| **.** or **,** | Decimal point |
| **+** | Addition |
| **-** | Subtraction |
| ***** | Multiplication |
| **/** | Division |
| **Enter** or **=** | Equals / Calculate |
| **Backspace** | Delete last digit |
| **Escape** | Clear all (`C`) |
| **Delete** | Clear entry (`CE`) |

---

## 🛠️ Building from Source

### Prerequisites
* Windows 10 or Windows 11
* Microsoft Visual Studio 2022 (or later) with the **Desktop development with C++** workload installed.

### Option 1: Using Visual Studio IDE
1. Clone this repository:
   ```bash
   git clone https://github.com/USER/cpp-calculator.git
   ```
2. Double-click `Calculator.sln` to open the solution.
3. Set the configuration to **Release** and platform to **x64**.
4. Press **Ctrl + Shift + B** to build, or **F5** to build and run.

### Option 2: Using Developer Command Prompt / MSBuild
```powershell
# Build x64 Release executable
msbuild Calculator.sln /p:Configuration=Release /p:Platform=x64
```
The compiled executable will be placed in `CalculatorProject\bin\x64\Release\Calculator.exe`.

---

## 📂 Repository Structure

```
cpp-calculator/
├── Calculator.sln             # Visual Studio Solution file
├── .gitignore                 # Excludes build artifacts and temporary files
├── LICENSE                    # MIT License
├── README.md                  # Project documentation
└── CalculatorProject/
    ├── Calculator.sln         # Self-contained project solution
    ├── Calculator.vcxproj     # Visual C++ Project configuration
    ├── Calculator.vcxproj.filters # Solution Explorer filters
    ├── Calculator.cpp         # Main C++ source code & UI logic
    ├── resource.h             # Resource definitions
    ├── Calculator.rc          # Windows resource script
    └── Calculator.ico         # Custom application icon
```

---

## 📄 License

This project is licensed under the [MIT License](LICENSE).
