# Simple C++ Calculator for Windows

A clean, modern, native Windows desktop calculator built with **C++** and the **Win32 API**, fully configured for **Microsoft Visual Studio**.

---

## 🚀 Quick Start (Visual Studio)

1. **Open the Solution**:
   - Double-click `Calculator.sln` located inside `CalculatorProject\` (or at the root of this folder).
   - Microsoft Visual Studio will open the solution.
2. **Select Configuration & Platform**:
   - Set the configuration to **Debug** (or **Release**).
   - Set the platform to **x64** (or **x86**).
3. **Run the Project**:
   - Press **F5** (or click the green **▶ Local Windows Debugger** button).
   - The calculator window will appear immediately!

---

## 📐 Project Structure

```
CalculatorProject/
├── Calculator.sln             # Visual Studio Solution file
├── Calculator.vcxproj         # Visual C++ Project file
├── Calculator.vcxproj.filters # Solution Explorer organization filters
├── Calculator.vcxproj.user    # Debugger configuration settings
├── Calculator.cpp             # Main C++ source code & UI logic
├── resource.h                 # Resource identifier definitions
├── Calculator.rc              # Windows resource script (icon & version)
├── Calculator.ico             # Application icon
└── README.md                  # Documentation and build guide
```

---

## 🧮 Features & Capabilities

* **Four Basic Arithmetic Operations**:
  * Addition (`+`)
  * Subtraction (`-`)
  * Multiplication (`*` / `×`)
  * Division (`/` / `÷`)
* **Number Input**:
  * Buttons from `0` to `9`.
  * Decimal point (`.`) with smart duplicate prevention.
* **Control Buttons**:
  * Equals (`=`): Computes the pending calculation.
  * Clear (`C`): Resets the entire calculator state and history.
  * Clear Entry (`CE`): Resets only the current number being typed.
  * Backspace (`⌫`): Deletes the last entered digit.
  * Sign toggle (`±`): Negates the current number.
* **Division by Zero Protection**:
  * Safely displays `"Cannot divide by zero"` without crashing or producing undefined behavior.
* **Two-Tier Display Screen**:
  * **Top line**: Shows current expression / history (e.g., `12 + 5 =`).
  * **Bottom line**: Shows current number or calculation result (e.g., `17`).
* **Clean Formatting**:
  * Eliminates trailing zeros (`5.0` displays as `5`).
  * Prevents ugly floating-point noise (`0.1 + 0.2` displays as `0.3`).
* **High-DPI & Modern Styling**:
  * Uses Windows Common Controls v6 for modern button styles.
  * Uses crisp **Segoe UI** typography.
  * Flicker-free double-buffered rendering.
* **Keyboard Support**:
  * You can type numbers and operators directly on your keyboard!

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

## 🛠️ Command-Line Build (MSBuild)

If you wish to build the application from PowerShell or Developer Command Prompt:

```powershell
# Build 64-bit Debug
msbuild Calculator.sln /p:Configuration=Debug /p:Platform=x64

# Build 64-bit Release
msbuild Calculator.sln /p:Configuration=Release /p:Platform=x64
```

The compiled executable will be located in:
`bin\x64\Debug\Calculator.exe` or `bin\x64\Release\Calculator.exe`.

---

## 🧠 Code Architecture

The code is organized into two primary components within `Calculator.cpp`:

1. **`CalculatorModel` Class**:
   - Manages calculation state (`m_currentInput`, `m_storedValue`, `m_pendingOp`).
   - Implements the mathematical state machine independently from GUI code.
   - Handles division by zero, overflow, and chaining operations (e.g., `5 + 3 * 2`).
2. **Win32 GUI Layer (`wWinMain`, `WndProc`)**:
   - Registers the window class and sets up High-DPI awareness.
   - Creates the 20 standard Win32 pushbuttons arranged in a 4x5 grid.
   - Uses double-buffered GDI drawing in `WM_PAINT` to render the display card.
   - Intercepts keyboard input in the message loop for seamless typing.
