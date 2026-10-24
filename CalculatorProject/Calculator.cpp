// ============================================================================
// Simple C++ Calculator for Windows
// ============================================================================
// A native Win32 desktop application that demonstrates how to build a clean,
// functional, and modern calculator using C++ and the Windows API.
//
// Key Features:
//  - Basic arithmetic operations: Addition (+), Subtraction (-),
//    Multiplication (*), Division (/)
//  - Number buttons 0-9, Decimal point (.), Equals (=), Clear (C)
//  - Extended helpers: Clear Entry (CE), Backspace (⌫), Sign toggle (±)
//  - Graceful division-by-zero error handling ("Cannot divide by zero")
//  - Full keyboard input support (numpad and main keyboard keys)
//  - High-DPI awareness and modern Windows visual styles
// ============================================================================

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <commctrl.h>
#include <string>
#include <sstream>
#include <iomanip>
#include <cmath>

#include "resource.h"

// Link against Common Controls library and request version 6.0 (modern themed controls)
#pragma comment(lib, "comctl32.lib")
#pragma comment(linker, "\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

// ============================================================================
// Control Identifiers (IDs for all buttons)
// ============================================================================
#define IDC_BTN_0           1000
#define IDC_BTN_1           1001
#define IDC_BTN_2           1002
#define IDC_BTN_3           1003
#define IDC_BTN_4           1004
#define IDC_BTN_5           1005
#define IDC_BTN_6           1006
#define IDC_BTN_7           1007
#define IDC_BTN_8           1008
#define IDC_BTN_9           1009
#define IDC_BTN_DOT         1010
#define IDC_BTN_ADD         1011
#define IDC_BTN_SUB         1012
#define IDC_BTN_MUL         1013
#define IDC_BTN_DIV         1014
#define IDC_BTN_EQU         1015
#define IDC_BTN_CLEAR       1016
#define IDC_BTN_CLEARENTRY  1017
#define IDC_BTN_BACKSPACE   1018
#define IDC_BTN_NEGATE      1019

// Supported arithmetic operations
enum class Operation {
    None,
    Add,
    Subtract,
    Multiply,
    Divide
};

// ============================================================================
// Calculator Model / Logic Class
// ============================================================================
// Encapsulates all calculation logic, state transitions, and error handling.
// Keeping calculation logic separate from Win32 GUI drawing keeps code clean
// and easy for beginners to understand.
// ============================================================================
class CalculatorModel {
public:
    CalculatorModel() {
        Reset();
    }

    // Reset all calculator state (called by 'C' button)
    void Reset() {
        m_currentInput = L"0";
        m_storedValue = 0.0;
        m_pendingOp = Operation::None;
        m_isNewInput = true;
        m_hasError = false;
        m_errorMessage.clear();
        m_expressionText.clear();
    }

    // Clear only current entry (called by 'CE' button)
    void ClearEntry() {
        if (m_hasError) {
            Reset();
        } else {
            m_currentInput = L"0";
            m_isNewInput = true;
        }
    }

    // User clicked a digit button (0-9)
    void InputDigit(int digit) {
        if (m_hasError) {
            Reset();
        }

        wchar_t ch = static_cast<wchar_t>(L'0' + digit);

        if (m_isNewInput) {
            m_currentInput = ch;
            m_isNewInput = false;
        } else {
            // Prevent entering excessively long numbers (cap at 16 digits)
            if (m_currentInput.length() >= 16) {
                return;
            }
            if (m_currentInput == L"0") {
                m_currentInput = ch; // Replace initial leading 0
            } else {
                m_currentInput += ch;
            }
        }
    }

    // User clicked decimal point (.)
    void InputDecimal() {
        if (m_hasError) {
            Reset();
        }

        if (m_isNewInput) {
            m_currentInput = L"0.";
            m_isNewInput = false;
        } else {
            // Ensure only one decimal point exists in current number
            if (m_currentInput.find(L'.') == std::wstring::npos) {
                m_currentInput += L".";
            }
        }
    }

    // Toggle positive / negative sign (±)
    void ToggleSign() {
        if (m_hasError) return;

        if (m_currentInput == L"0" || m_currentInput.empty()) {
            return;
        }

        if (m_currentInput[0] == L'-') {
            m_currentInput.erase(0, 1);
        } else {
            m_currentInput = L"-" + m_currentInput;
        }
    }

    // Delete last digit (Backspace ⌫)
    void Backspace() {
        if (m_hasError) {
            Reset();
            return;
        }

        if (m_isNewInput) {
            return; // Don't alter previous calculation results with backspace
        }

        if (m_currentInput.length() > 1) {
            m_currentInput.pop_back();
            // If only a negative sign remains, reset back to 0
            if (m_currentInput == L"-" || m_currentInput.empty()) {
                m_currentInput = L"0";
                m_isNewInput = true;
            }
        } else {
            m_currentInput = L"0";
            m_isNewInput = true;
        }
    }

    // Set operator (+, -, *, /)
    void SetOperator(Operation op) {
        if (m_hasError) return;

        double currentVal = ParseCurrentInput();

        // Support chaining operations: e.g. 5 + 3 + 2 evaluates 5 + 3 first
        if (m_pendingOp != Operation::None && !m_isNewInput) {
            if (!ExecutePendingOperation(currentVal)) {
                return; // Error occurred during calculation (e.g. division by zero)
            }
        } else {
            m_storedValue = currentVal;
        }

        m_pendingOp = op;
        m_isNewInput = true;
        m_expressionText = FormatNumber(m_storedValue) + L" " + GetOpSymbol(op);
    }

    // Perform calculation (called by '=' button)
    void CalculateEquals() {
        if (m_hasError) return;

        if (m_pendingOp == Operation::None) {
            // No pending operator, just display current number with "="
            double val = ParseCurrentInput();
            m_expressionText = FormatNumber(val) + L" =";
            m_isNewInput = true;
            return;
        }

        double secondOperand = ParseCurrentInput();
        Operation op = m_pendingOp;
        double firstOperand = m_storedValue;

        if (!ExecutePendingOperation(secondOperand)) {
            // Division by zero or overflow occurred
            m_expressionText = FormatNumber(firstOperand) + L" " + GetOpSymbol(op) + L" 0 =";
            return;
        }

        // Show full expression above the result (e.g. "12 + 5 =")
        m_expressionText = FormatNumber(firstOperand) + L" " + GetOpSymbol(op) + L" " + FormatNumber(secondOperand) + L" =";
        m_pendingOp = Operation::None;
        m_isNewInput = true;
    }

    // Text to display in the main result area
    std::wstring GetDisplayText() const {
        if (m_hasError) {
            return m_errorMessage;
        }
        return m_currentInput;
    }

    // Smaller expression text to display above the main number
    std::wstring GetExpressionText() const {
        return m_expressionText;
    }

    bool HasError() const {
        return m_hasError;
    }

private:
    double ParseCurrentInput() const {
        try {
            return std::stod(m_currentInput);
        } catch (...) {
            return 0.0;
        }
    }

    // Executes the pending binary operation: (m_storedValue [op] secondOperand)
    bool ExecutePendingOperation(double secondOperand) {
        double result = 0.0;

        switch (m_pendingOp) {
        case Operation::Add:
            result = m_storedValue + secondOperand;
            break;
        case Operation::Subtract:
            result = m_storedValue - secondOperand;
            break;
        case Operation::Multiply:
            result = m_storedValue * secondOperand;
            break;
        case Operation::Divide:
            // Check for division by zero
            if (std::abs(secondOperand) < 1e-15) {
                m_hasError = true;
                m_errorMessage = L"Cannot divide by zero";
                m_currentInput = L"0";
                m_pendingOp = Operation::None;
                m_isNewInput = true;
                return false;
            }
            result = m_storedValue / secondOperand;
            break;
        default:
            result = secondOperand;
            break;
        }

        // Check for numerical overflow or invalid floating-point results
        if (std::isnan(result) || std::isinf(result)) {
            m_hasError = true;
            m_errorMessage = L"Overflow";
            m_currentInput = L"0";
            m_pendingOp = Operation::None;
            m_isNewInput = true;
            return false;
        }

        m_storedValue = result;
        m_currentInput = FormatNumber(result);
        return true;
    }

    static std::wstring GetOpSymbol(Operation op) {
        switch (op) {
        case Operation::Add:      return L"+";
        case Operation::Subtract: return L"−";
        case Operation::Multiply: return L"×";
        case Operation::Divide:   return L"÷";
        default:                  return L"";
        }
    }

    // Formats a double into a clean string without trailing zeros
    static std::wstring FormatNumber(double val) {
        if (std::isnan(val) || std::isinf(val)) {
            return L"Error";
        }
        // Avoid displaying negative zero (-0)
        if (std::abs(val) < 1e-12) {
            val = 0.0;
        }

        std::wostringstream oss;
        oss << std::setprecision(12) << val;
        return oss.str();
    }

    std::wstring m_currentInput;
    std::wstring m_expressionText;
    std::wstring m_errorMessage;
    double m_storedValue{ 0.0 };
    Operation m_pendingOp{ Operation::None };
    bool m_isNewInput{ true };
    bool m_hasError{ false };
};

// Global calculator model instance
static CalculatorModel g_Calculator;

// Global fonts
static HFONT g_hFontMain   = nullptr; // Big font for calculation result
static HFONT g_hFontError  = nullptr; // Font for error messages
static HFONT g_hFontExpr   = nullptr; // Small font for expression history
static HFONT g_hFontButton = nullptr; // Font for button labels

// ============================================================================
// Button Grid Definition
// ============================================================================
struct ButtonInfo {
    int id;
    const wchar_t* text;
    int row;
    int col;
};

// 4 Columns x 5 Rows Grid layout:
//   Row 0: [ C ]   [ CE ]  [ ⌫ ]   [ ÷ ]
//   Row 1: [ 7 ]   [ 8 ]   [ 9 ]   [ × ]
//   Row 2: [ 4 ]   [ 5 ]   [ 6 ]   [ − ]
//   Row 3: [ 1 ]   [ 2 ]   [ 3 ]   [ + ]
//   Row 4: [ ± ]   [ 0 ]   [ . ]   [ = ]
static const ButtonInfo g_Buttons[] = {
    // Row 0: Clear & operator buttons
    { IDC_BTN_CLEAR,      L"C",   0, 0 },
    { IDC_BTN_CLEARENTRY, L"CE",  0, 1 },
    { IDC_BTN_BACKSPACE,  L"⌫",   0, 2 },
    { IDC_BTN_DIV,        L"/",   0, 3 },

    // Row 1
    { IDC_BTN_7,          L"7",   1, 0 },
    { IDC_BTN_8,          L"8",   1, 1 },
    { IDC_BTN_9,          L"9",   1, 2 },
    { IDC_BTN_MUL,        L"×",   1, 3 },

    // Row 2
    { IDC_BTN_4,          L"4",   2, 0 },
    { IDC_BTN_5,          L"5",   2, 1 },
    { IDC_BTN_6,          L"6",   2, 2 },
    { IDC_BTN_SUB,        L"−",   2, 3 },

    // Row 3
    { IDC_BTN_1,          L"1",   3, 0 },
    { IDC_BTN_2,          L"2",   3, 1 },
    { IDC_BTN_3,          L"3",   3, 2 },
    { IDC_BTN_ADD,        L"+",   3, 3 },

    // Row 4
    { IDC_BTN_NEGATE,     L"±",   4, 0 },
    { IDC_BTN_0,          L"0",   4, 1 },
    { IDC_BTN_DOT,        L".",   4, 2 },
    { IDC_BTN_EQU,        L"=",   4, 3 }
};

// Forward declarations
LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void CreateApplicationFonts();
void DestroyApplicationFonts();
void ExecuteCommand(HWND hwnd, int controlId);
bool HandleKeyDown(HWND hwnd, WPARAM wParam);
bool HandleChar(HWND hwnd, wchar_t ch);

// ============================================================================
// WinMain - Application Entry Point
// ============================================================================
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, PWSTR /*pCmdLine*/, int nCmdShow) {
    // Enable Per-Monitor High-DPI awareness if available on this Windows version
    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
    if (hUser32) {
        typedef BOOL(WINAPI* SetProcessDpiContextFunc)(DPI_AWARENESS_CONTEXT);
        auto setDpiContext = reinterpret_cast<SetProcessDpiContextFunc>(
            GetProcAddress(hUser32, "SetProcessDpiAwarenessContext"));
        if (setDpiContext) {
            setDpiContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
        }
    }

    // Initialize Common Controls
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_STANDARD_CLASSES;
    InitCommonControlsEx(&icex);

    // Create custom UI fonts
    CreateApplicationFonts();

    // Register Window Class
    const wchar_t CLASS_NAME[] = L"SimpleCalculatorWindowClass";

    WNDCLASSEXW wcex = {};
    wcex.cbSize        = sizeof(WNDCLASSEXW);
    wcex.style         = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc   = WndProc;
    wcex.cbClsExtra    = 0;
    wcex.cbWndExtra    = 0;
    wcex.hInstance     = hInstance;
    wcex.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CALCULATOR));
    if (!wcex.hIcon) {
        wcex.hIcon     = LoadIcon(nullptr, IDI_APPLICATION);
    }
    wcex.hCursor       = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = CreateSolidBrush(RGB(243, 243, 243));
    wcex.lpszMenuName  = nullptr;
    wcex.lpszClassName = CLASS_NAME;
    wcex.hIconSm       = wcex.hIcon;

    if (!RegisterClassExW(&wcex)) {
        MessageBoxW(nullptr, L"Failed to register window class.", L"Error", MB_ICONERROR);
        return 0;
    }

    // Set fixed client window dimensions: 320 x 388 pixels
    RECT rcClient = { 0, 0, 320, 388 };
    DWORD windowStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
    AdjustWindowRectEx(&rcClient, windowStyle, FALSE, 0);

    int winWidth  = rcClient.right - rcClient.left;
    int winHeight = rcClient.bottom - rcClient.top;

    // Center window on primary monitor
    int screenX = (GetSystemMetrics(SM_CXSCREEN) - winWidth) / 2;
    int screenY = (GetSystemMetrics(SM_CYSCREEN) - winHeight) / 2;

    HWND hwnd = CreateWindowExW(
        0,
        CLASS_NAME,
        L"Calculator",
        windowStyle,
        screenX, screenY, winWidth, winHeight,
        nullptr, nullptr, hInstance, nullptr
    );

    if (!hwnd) {
        MessageBoxW(nullptr, L"Failed to create calculator window.", L"Error", MB_ICONERROR);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Main message loop with keyboard accelerator/typing interception
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        // Intercept keyboard events globally so the user can freely type
        if (msg.message == WM_KEYDOWN) {
            if (HandleKeyDown(hwnd, msg.wParam)) {
                continue; // Consumed by calculator
            }
        } else if (msg.message == WM_CHAR) {
            if (HandleChar(hwnd, static_cast<wchar_t>(msg.wParam))) {
                continue; // Consumed by calculator
            }
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Cleanup resources
    DestroyApplicationFonts();
    DeleteObject(wcex.hbrBackground);

    return static_cast<int>(msg.wParam);
}

// ============================================================================
// Font Creation and Teardown
// ============================================================================
void CreateApplicationFonts() {
    // Segoe UI is the modern standard font across Windows
    g_hFontMain = CreateFontW(
        -30, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI"
    );

    g_hFontError = CreateFontW(
        -18, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI"
    );

    g_hFontExpr = CreateFontW(
        -14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI"
    );

    g_hFontButton = CreateFontW(
        -18, 0, 0, 0, FW_MEDIUM, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI"
    );
}

void DestroyApplicationFonts() {
    if (g_hFontMain)   { DeleteObject(g_hFontMain);   g_hFontMain = nullptr; }
    if (g_hFontError)  { DeleteObject(g_hFontError);  g_hFontError = nullptr; }
    if (g_hFontExpr)   { DeleteObject(g_hFontExpr);   g_hFontExpr = nullptr; }
    if (g_hFontButton) { DeleteObject(g_hFontButton); g_hFontButton = nullptr; }
}

// ============================================================================
// Window Procedure (Message Handling)
// ============================================================================
LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE: {
        auto createStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
        HINSTANCE hInst = createStruct->hInstance;

        // Button layout parameters
        const int btnStartX = 14;
        const int btnStartY = 100;
        const int btnGap    = 8;
        const int btnWidth  = 67;
        const int btnHeight = 48;

        // Create the 20 calculator buttons in a clean 4x5 grid
        for (const auto& btn : g_Buttons) {
            int x = btnStartX + btn.col * (btnWidth + btnGap);
            int y = btnStartY + btn.row * (btnHeight + btnGap);

            HWND hBtn = CreateWindowExW(
                0,
                L"BUTTON",
                btn.text,
                WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP,
                x, y, btnWidth, btnHeight,
                hwnd,
                reinterpret_cast<HMENU>(static_cast<INT_PTR>(btn.id)),
                hInst,
                nullptr
            );

            if (hBtn && g_hFontButton) {
                SendMessage(hBtn, WM_SETFONT, reinterpret_cast<WPARAM>(g_hFontButton), TRUE);
            }
        }
        return 0;
    }

    case WM_COMMAND: {
        int controlId = LOWORD(wParam);
        ExecuteCommand(hwnd, controlId);
        return 0;
    }

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        RECT rcClient;
        GetClientRect(hwnd, &rcClient);
        int clientWidth  = rcClient.right - rcClient.left;
        int clientHeight = rcClient.bottom - rcClient.top;

        // Double buffering to guarantee completely flicker-free rendering
        HDC memDC = CreateCompatibleDC(hdc);
        HBITMAP memBitmap = CreateCompatibleBitmap(hdc, clientWidth, clientHeight);
        HGDIOBJ oldBitmap = SelectObject(memDC, memBitmap);

        // 1. Paint window background
        HBRUSH bgBrush = CreateSolidBrush(RGB(243, 243, 243));
        FillRect(memDC, &rcClient, bgBrush);
        DeleteObject(bgBrush);

        // 2. Draw display card container (white card with subtle border)
        RECT rcCard = { 14, 12, clientWidth - 14, 88 };
        HBRUSH cardBrush = CreateSolidBrush(RGB(255, 255, 255));
        HPEN borderPen   = CreatePen(PS_SOLID, 1, RGB(220, 220, 220));
        HGDIOBJ oldBrush = SelectObject(memDC, cardBrush);
        HGDIOBJ oldPen   = SelectObject(memDC, borderPen);

        RoundRect(memDC, rcCard.left, rcCard.top, rcCard.right, rcCard.bottom, 10, 10);

        SelectObject(memDC, oldPen);
        SelectObject(memDC, oldBrush);
        DeleteObject(borderPen);
        DeleteObject(cardBrush);

        // 3. Draw secondary expression text (e.g. "12 + 5 =")
        SetBkMode(memDC, TRANSPARENT);
        HGDIOBJ oldFont = SelectObject(memDC, g_hFontExpr);
        SetTextColor(memDC, RGB(115, 115, 115));
        RECT rcExpr = { rcCard.left + 12, rcCard.top + 8, rcCard.right - 14, rcCard.top + 28 };
        std::wstring exprText = g_Calculator.GetExpressionText();
        DrawTextW(memDC, exprText.c_str(), -1, &rcExpr, DT_RIGHT | DT_SINGLELINE | DT_VCENTER);

        // 4. Draw main display value or error message
        if (g_Calculator.HasError()) {
            SelectObject(memDC, g_hFontError);
            SetTextColor(memDC, RGB(215, 38, 38)); // Crimson red for error
        } else {
            SelectObject(memDC, g_hFontMain);
            SetTextColor(memDC, RGB(25, 25, 25));   // Dark slate for result
        }
        RECT rcMain = { rcCard.left + 12, rcCard.top + 32, rcCard.right - 14, rcCard.bottom - 8 };
        std::wstring dispText = g_Calculator.GetDisplayText();
        DrawTextW(memDC, dispText.c_str(), -1, &rcMain, DT_RIGHT | DT_SINGLELINE | DT_VCENTER);

        SelectObject(memDC, oldFont);

        // Copy backbuffer to screen
        BitBlt(hdc, 0, 0, clientWidth, clientHeight, memDC, 0, 0, SRCCOPY);

        SelectObject(memDC, oldBitmap);
        DeleteObject(memBitmap);
        DeleteDC(memDC);

        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_ERASEBKGND:
        // Handled in WM_PAINT via double buffering to eliminate flicker
        return 1;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    default:
        return DefWindowProcW(hwnd, uMsg, wParam, lParam);
    }
}

// ============================================================================
// Action Dispatcher (Handles button clicks and key presses)
// ============================================================================
void ExecuteCommand(HWND hwnd, int controlId) {
    switch (controlId) {
    case IDC_BTN_0: g_Calculator.InputDigit(0); break;
    case IDC_BTN_1: g_Calculator.InputDigit(1); break;
    case IDC_BTN_2: g_Calculator.InputDigit(2); break;
    case IDC_BTN_3: g_Calculator.InputDigit(3); break;
    case IDC_BTN_4: g_Calculator.InputDigit(4); break;
    case IDC_BTN_5: g_Calculator.InputDigit(5); break;
    case IDC_BTN_6: g_Calculator.InputDigit(6); break;
    case IDC_BTN_7: g_Calculator.InputDigit(7); break;
    case IDC_BTN_8: g_Calculator.InputDigit(8); break;
    case IDC_BTN_9: g_Calculator.InputDigit(9); break;

    case IDC_BTN_DOT:         g_Calculator.InputDecimal(); break;
    case IDC_BTN_ADD:         g_Calculator.SetOperator(Operation::Add); break;
    case IDC_BTN_SUB:         g_Calculator.SetOperator(Operation::Subtract); break;
    case IDC_BTN_MUL:         g_Calculator.SetOperator(Operation::Multiply); break;
    case IDC_BTN_DIV:         g_Calculator.SetOperator(Operation::Divide); break;
    case IDC_BTN_EQU:         g_Calculator.CalculateEquals(); break;
    case IDC_BTN_CLEAR:       g_Calculator.Reset(); break;
    case IDC_BTN_CLEARENTRY:  g_Calculator.ClearEntry(); break;
    case IDC_BTN_BACKSPACE:   g_Calculator.Backspace(); break;
    case IDC_BTN_NEGATE:      g_Calculator.ToggleSign(); break;
    default:
        return;
    }

    // Trigger display repaint to show the updated numbers
    RECT rcDisplay = { 14, 12, 306, 88 };
    InvalidateRect(hwnd, &rcDisplay, FALSE);
    UpdateWindow(hwnd);
}

// ============================================================================
// Keyboard Event Handling
// ============================================================================
bool HandleKeyDown(HWND hwnd, WPARAM wParam) {
    switch (wParam) {
    case VK_RETURN:
        ExecuteCommand(hwnd, IDC_BTN_EQU);
        return true;
    case VK_BACK:
        ExecuteCommand(hwnd, IDC_BTN_BACKSPACE);
        return true;
    case VK_ESCAPE:
        ExecuteCommand(hwnd, IDC_BTN_CLEAR);
        return true;
    case VK_DELETE:
        ExecuteCommand(hwnd, IDC_BTN_CLEARENTRY);
        return true;
    default:
        return false;
    }
}

bool HandleChar(HWND hwnd, wchar_t ch) {
    if (ch >= L'0' && ch <= L'9') {
        ExecuteCommand(hwnd, IDC_BTN_0 + (ch - L'0'));
        return true;
    }

    switch (ch) {
    case L'+':
        ExecuteCommand(hwnd, IDC_BTN_ADD);
        return true;
    case L'-':
        ExecuteCommand(hwnd, IDC_BTN_SUB);
        return true;
    case L'*':
        ExecuteCommand(hwnd, IDC_BTN_MUL);
        return true;
    case L'/':
        ExecuteCommand(hwnd, IDC_BTN_DIV);
        return true;
    case L'.':
    case L',': // Common on European numpads
        ExecuteCommand(hwnd, IDC_BTN_DOT);
        return true;
    case L'=':
        ExecuteCommand(hwnd, IDC_BTN_EQU);
        return true;
    case L'c':
    case L'C':
        ExecuteCommand(hwnd, IDC_BTN_CLEAR);
        return true;
    default:
        return false;
    }
}
