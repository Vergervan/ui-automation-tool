#include "mouselogger.h"

MouseLogger::MouseLogger(QObject *parent) : QObject(parent){}

MouseLogger &MouseLogger::instance()
{
    static MouseLogger _instance;
    return _instance;
}

void MouseLogger::HandleMouseMoveHook()
{
    HINSTANCE hInstance = GetModuleHandle(NULL);

    mouseMoveHook = SetWindowsHookEx(WH_MOUSE_LL, mouseMoveProc, hInstance, 0);

    if (mouseMoveHook == NULL) {
        qWarning() << "Mouse Move Hook failed";
    }
}

void MouseLogger::UnhookMouseMove()
{
    UnhookWindowsHookEx(instance().mouseMoveHook);
    qDebug() << "Mouse move unhooked";
}

void MouseLogger::HandleMouseHooks()
{
    HINSTANCE hInstance = GetModuleHandle(NULL);

    mouseHook = SetWindowsHookEx(WH_MOUSE_LL, mouseProc, hInstance, 0);

    if (mouseHook == NULL) {
        qWarning() << "Mouse Hook failed";
    }
}

void MouseLogger::HandleKeyboardHooks()
{
    HINSTANCE hInstance = GetModuleHandle(NULL);

    keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, keyboardProc, hInstance, 0);
    if(keyboardHook == NULL){
        qWarning() << "Keyboard Hook failed";
    }
}

LRESULT CALLBACK MouseLogger::mouseMoveProc(int Code, WPARAM wParam, LPARAM lParam)
{
    Q_UNUSED(Code)

    MOUSEHOOKSTRUCT * pMouseStruct = (MOUSEHOOKSTRUCT *)lParam;

    if(pMouseStruct != nullptr) {
        switch (wParam) {
        case WM_MOUSEMOVE:
            emit instance().mouseMovesEvent();
            instance().UnhookMouseMove();
            qDebug() << "WM_MOUSEMOVE";
            break;
        default:
            break;
        }
    }
    return CallNextHookEx(NULL, Code, wParam, lParam);
}

LRESULT CALLBACK MouseLogger::mouseProc(int Code, WPARAM wParam, LPARAM lParam)
{
    Q_UNUSED(Code)

    MOUSEHOOKSTRUCT * pMouseStruct = (MOUSEHOOKSTRUCT *)lParam;

    POINT mousePoint;
    GetCursorPos(&mousePoint);

    if(pMouseStruct != nullptr) {
        switch (wParam) {
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
            emit instance().mousePressedEvent(wParam, mousePoint.x, mousePoint.y);
            UnhookWindowsHookEx(instance().mouseHook);
            break;
//        case WM_MOUSEMOVE:
//            qDebug() << "WM_MOUSEMOVE";
//            break;
//        case WM_MOUSEWHEEL:
//            qDebug() << "WM_MOUSEWHEEL";
//            break;
        default:
            break;
        }
    }

    // You need to return back to the chain hook event handlers
    return CallNextHookEx(NULL, Code, wParam, lParam);
}


LRESULT CALLBACK MouseLogger::keyboardProc(int Code, WPARAM wParam, LPARAM lParam)
{
    Q_UNUSED(Code)

    PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT) lParam;

    if(p != nullptr){
        switch(wParam){
        case WM_SYSKEYDOWN:
        case WM_KEYDOWN:
            qDebug() << "Scan code: " << p->scanCode;
            emit instance().keyPressedEvent(lParam);
            UnhookWindowsHookEx(instance().keyboardHook);
            break;
        }
    }
    return CallNextHookEx(NULL, Code, wParam, lParam);
}
