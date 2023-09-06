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
        switch (wParam)
        {
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
            emit instance().mousePressedEvent(wParam, mousePoint.x, mousePoint.y);
            UnhookWindowsHookEx(instance().mouseHook);
            break;
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

    static std::set<DWORD> keysDown;

    PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT) lParam;

    if(p != nullptr){
        switch(wParam)
        {
        case WM_SYSKEYDOWN:
        case WM_KEYDOWN:
            switch(p->vkCode)
            {
            case VK_RCONTROL: case VK_RMENU:
            case VK_LEFT: case VK_UP: case VK_RIGHT: case VK_DOWN: // arrow keys
            case VK_PRIOR: case VK_NEXT: // page up and page down
            case VK_END: case VK_HOME:
            case VK_INSERT: case VK_DELETE:
            case VK_DIVIDE: // numpad slash
            case VK_NUMLOCK:
            case VK_SNAPSHOT: // print screen
            case VK_LWIN: case VK_RWIN:
            case VK_APPS:
            //case VK_RETURN:
                p->scanCode |= 0x100; // set extended bit
            default:
                keysDown.insert(p->scanCode);
                break;
            }
            break;
        case WM_SYSKEYUP:
        case WM_KEYUP:
            if(keysDown.size() > 0)
            {
                emit instance().keyPressedEvent(keysDown);
                keysDown.clear();
                UnhookWindowsHookEx(instance().keyboardHook);
            }
            break;
        }
    }
    return CallNextHookEx(NULL, Code, wParam, lParam);
}
