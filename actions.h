
#ifndef ACTIONS_H
#define ACTIONS_H

#include <qstring.h>
#include <QStringList>
#include <windows.h>
#include <vector>

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
    std::vector<DWORD> keys;
};

#endif // ACTIONS_H
