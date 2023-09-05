
#ifndef ACTIONS_H
#define ACTIONS_H

#include <qstring.h>
#include <QStringList>
#include <windows.h>

enum ActionType{
    MouseClick, KeyPress, PasteBuffer, OpenApp, LoopedBuffer, Delay
};

struct HandleData {
    unsigned long processId;
    HWND windowHandle;
    QString processName;
};

struct ActionInfo{
    ActionType type;
    int x, y;
    QString text;
    unsigned long code;
    HandleData data;
    QStringList list;
};

#endif // ACTIONS_H
