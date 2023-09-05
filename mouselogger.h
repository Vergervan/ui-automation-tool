#ifndef MOUSELOGGER_H
#define MOUSELOGGER_H

#include <QObject>
#include <QDebug>
#include <windows.h>
#include <winuser.h>

class MouseLogger : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(MouseLogger)
public:
    static MouseLogger &instance();
    explicit MouseLogger(QObject *parent = nullptr);
    virtual ~MouseLogger(){}
    void HandleMouseMoveHook();
    void UnhookMouseMove();
    void HandleMouseHooks();
    void HandleKeyboardHooks();
    // Static method that will act as a callback-function
    static LRESULT CALLBACK mouseMoveProc(int Code, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK mouseProc(int Code, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK keyboardProc(int Code, WPARAM wParam, LPARAM lParam);

signals:
    // The signal, which will report the occurrence of an event
    void mousePressedEvent(unsigned long, int, int);
    void keyPressedEvent(LPARAM);
    void hotkeyPressedEvent(unsigned long);
    void mouseMovesEvent();
    void maximizeEvent();

public slots:

private:
    // hook handler
    HHOOK   mouseMoveHook;
    HHOOK   mouseHook;
    HHOOK   keyboardHook;
};

#endif // MOUSELOGGER_H
