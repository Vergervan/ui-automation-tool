
#include "widget.h"
#include "ui_widget.h"


Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    setStyleSheet("QPushButton {color: black;}");

    //MouseLogger::instance().HandleKeyboardHooks();
    QObject::connect(&MouseLogger::instance(), &MouseLogger::mousePressedEvent, this, &Widget::onMousePressed);
    QObject::connect(&MouseLogger::instance(), &MouseLogger::keyPressedEvent, this, &Widget::onKeyPressed);
    QObject::connect(&MouseLogger::instance(), &MouseLogger::mouseMovesEvent, this, &Widget::stopLoop);
    //QObject::connect(&MouseLogger::instance(), &MouseLogger::maximizeEvent, this, &Widget::showNormal);
    ui->removeButton->setEnabled(false);
    ui->editButton->setEnabled(false);
    ui->startButton->setEnabled(false);
    ui->upItemButton->setEnabled(false);
    ui->downItemButton->setEnabled(false);

    auto model =  ui->listWidget->model();
    connect(model, &QAbstractItemModel::rowsInserted, this, [this]{
        if(loopBuffers.size() == 0)
            updateLoopCount(1);
        ui->removeButton->setEnabled(true);
        ui->startButton->setEnabled(true);
        if(ui->listWidget->count() >= 2)
        {
            ui->upItemButton->setEnabled(true);
            ui->downItemButton->setEnabled(true);
        }
    });
    connect(model, &QAbstractItemModel::rowsRemoved, this, [this]{
        if(ui->listWidget->count() == 0)
        {
            updateLoopCount(0);
            ui->removeButton->setEnabled(false);
            ui->startButton->setEnabled(false);
        }
        if(ui->listWidget->count() < 2)
        {
            ui->upItemButton->setEnabled(false);
            ui->downItemButton->setEnabled(false);
        }
    });
}

Widget::~Widget()
{
    delete ui;
}

void Widget::onMousePressed(unsigned long but, int x, int y)
{
    ActionInfo info;
    QString str = "%1 %2";
    QListWidgetItem* item = new QListWidgetItem("Mouse click: " + str.arg(x).arg(y));

    info.type = ActionType::MouseClick;
    info.code = but;
    info.x = x;
    info.y = y;

    this->actionMap.insert(item, info);

    ui->listWidget->addItem(item);

    //SetWindowPos((HWND) winId(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    this->showNormal();
    QThread::msleep(130);
    this->activateWindow();
    //SetWindowPos((HWND) winId(), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}

void Widget::onKeyPressed(LPARAM lParam)
{
      PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT) lParam;
//    std::array<wchar_t, 128> buffer{};
//    memset(buffer.data(), 0, 128);
//    int count = ::GetKeyNameText(static_cast<LONG>(lParam), buffer.data(), static_cast<int>(buffer.size()));
//    std::string str(buffer.data(), count);
//    std::array<wchar_t, 9> ws;
//    memset(ws.data(), 0, sizeof(wchar_t) * ws.size());
//    wsprintf(ws.data(), L"%08X", MAKELANGID( LANG_INVARIANT, SUBLANG_NEUTRAL ));
//    //csLangId.Format( L"%08X", MAKELANGID( LANG_INVARIANT, SUBLANG_NEUTRAL ) );
//    HKL hLocale = LoadKeyboardLayout( (LPCTSTR)ws.data(), KLF_ACTIVATE );
//    HKL hPrevious = ActivateKeyboardLayout( hLocale, KLF_SETFORPROCESS );

//    std::array<wchar_t, 128> buffer{};
//    memset(buffer.data(), 0, sizeof(wchar_t) * buffer.size());
//    GetKeyNameText(lParam, buffer.data(), static_cast<int>(buffer.size()));
//    GetKeyNameTextW(lParam, buffer.data(), static_cast<int>(buffer.size()));

    HKL layouts[2];
    int hklCnt = GetKeyboardLayoutList(2, layouts);

    unsigned int scanCode = MapVirtualKeyEx(p->vkCode, MAPVK_VK_TO_VSC_EX, layouts[0]);

    qDebug() << scanCode;

    // because MapVirtualKey strips the extended bit for some keys
    switch (p->vkCode)
    {
    case VK_LEFT: case VK_UP: case VK_RIGHT: case VK_DOWN: // arrow keys
    case VK_PRIOR: case VK_NEXT: // page up and page down
    case VK_END: case VK_HOME:
    case VK_INSERT: case VK_DELETE:
    case VK_DIVIDE: // numpad slash
    case VK_NUMLOCK:
    {
        scanCode |= 0x100; // set extended bit
        break;
    }
    }

    wchar_t keyName[50];
    int count = GetKeyNameText(scanCode, keyName, sizeof(keyName));
    std::wstring ws(keyName, count+1);
    std::string str(ws.begin(), ws.end());


//    std::string str(buffer.begin(), buffer.end());
//    qDebug() << str;
//    ActivateKeyboardLayout( hPrevious, KLF_SETFORPROCESS );
//    UnloadKeyboardLayout( hLocale );

    ActionInfo info;
    QListWidgetItem* item = new QListWidgetItem(QString("Key press: %1").arg(str.c_str()));

    info.type = ActionType::KeyPress;
    info.code = p->vkCode;

    this->actionMap.insert(item, info);

    ui->listWidget->addItem(item);
}

void Widget::onHotkeyPressed(unsigned long key)
{
    Q_UNUSED(key)
}

void Widget::on_keyActionButton_clicked()
{
    MouseLogger::instance().HandleKeyboardHooks();

}

void Widget::on_clickActionButton_clicked()
{
    MouseLogger::instance().HandleMouseHooks();
    this->showMinimized();
}

void Widget::on_removeButton_clicked()
{
    auto data = actionMap[ui->listWidget->currentItem()];
    if(data.type == ActionType::LoopedBuffer)
    {
        removeFromLoopBuffers(data.list.count());
        updateLoopCount(findMaxLoopBuffer());
    }
    actionMap.remove(ui->listWidget->currentItem());
    delete ui->listWidget->currentItem();
}

void Widget::on_listWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    Q_UNUSED(previous)

    switch(actionMap[current].type)
    {
    case ActionType::PasteBuffer:
    case ActionType::OpenApp:
    case ActionType::LoopedBuffer:
    case ActionType::Delay:
        ui->editButton->setEnabled(true);
        break;
    default:
        ui->editButton->setEnabled(false);
        break;
    }
    ui->removeButton->setEnabled(current == nullptr ? false : true);
}


void Widget::on_startButton_clicked()
{
    MouseLogger::instance().HandleMouseMoveHook();

    POINT remindMouse;
    //WINDOWPLACEMENT wndplc;
    GetCursorPos(&remindMouse);

    int defDelay = 500;

    this->hide();

    auto list = ui->listWidget->findItems("*", Qt::MatchWildcard);

    for(int i = 0; i < loopCount; i++)
    {
        for (auto& listItem : list)
        {
            if(isStopLoop)
            {
                qDebug() << "Loop is stopped";
                isStopLoop = false;
                SetCursorPos(remindMouse.x, remindMouse.y);
                this->mainHwnd = 0;
                this->show();
                return;
            }
            auto item = actionMap[listItem];
            QThread::msleep(defDelay);
            switch(item.type)
            {
            case ActionType::OpenApp:
    //            if(GetWindowPlacement(item.data.windowHandle, &wndplc)){
    //                qDebug() << wndplc.showCmd;
    //            }
                ShowWindow(item.data.windowHandle, SW_SHOW);
                ShowWindow(item.data.windowHandle, SW_SHOWMAXIMIZED);
                SetForegroundWindow(item.data.windowHandle);
//                ShowWindow(item.data.windowHandle, SW_SHOWMAXIMIZED);
//                SetFocus(item.data.windowHandle);
//                SetWindowPos(item.data.windowHandle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
//                SetWindowPos(item.data.windowHandle, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOMOVE);
                this->mainHwnd = item.data.windowHandle;
                break;
            case ActionType::MouseClick:
                sendMouseClick(item.code, item.x, item.y);
                qDebug() << "Mouse click at " << item.x << ":" << item.y;
                break;
            case ActionType::KeyPress:
                break;
            case ActionType::PasteBuffer:
                sendPasteText(this->mainHwnd, item.text);
                break;
            case ActionType::LoopedBuffer:
                if(i < item.list.count())
                    sendPasteText(this->mainHwnd, item.list.at(i));
                break;
            case ActionType::Delay:
                QThread::msleep(item.text.toInt());
                break;
            }
        }
    }
    MouseLogger::instance().UnhookMouseMove();
    isStopLoop = false;
    SetCursorPos(remindMouse.x, remindMouse.y);
    this->mainHwnd = 0;
    this->show();
}

void Widget::stopLoop()
{
    this->isStopLoop = true;
    qDebug() << "Stop loop slot called";
}

void Widget::sendPasteText(HWND wnd, QString str)
{
    Q_UNUSED(wnd)
    int len = str.length()+1;
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, len);
    memcpy(GlobalLock(hMem), str.toStdString().c_str(), len);
    GlobalUnlock(hMem);
    OpenClipboard(0);
    EmptyClipboard();
    SetClipboardData(CF_TEXT, hMem);
    CloseClipboard();

    //SendMessage(wnd, WM_PASTE, 0, 0);

    HKL kbl = GetKeyboardLayout(0);

    INPUT inputs[2] = {};
    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = VK_CONTROL;

    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = VkKeyScanEx('V',kbl);

    SendInput(2, inputs, sizeof(INPUT));

    QThread::msleep(50);

    inputs[0].ki.dwFlags = KEYEVENTF_KEYUP;
    inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;

    SendInput(2, inputs, sizeof(INPUT));
}

void Widget::sendMouseClick(unsigned long but, int x, int y)
{
    INPUT inputs[2] = {};

    SetCursorPos(x, y);
//    inputs[0].type = INPUT_MOUSE;
//    inputs[0].mi.dx = x; // desired X coordinate
//    inputs[0].mi.dy = y; // desired Y coordinate
//    inputs[0].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;

    inputs[0].type = INPUT_MOUSE;
    inputs[0].mi.dwFlags = getMouseInputDown(but);

    inputs[1].type = INPUT_MOUSE;
    inputs[1].mi.dwFlags = getMouseInputUp(but);

    SendInput(2, inputs, sizeof(INPUT));
}

unsigned long Widget::getMouseInputDown(unsigned long but)
{
    switch(but)
    {
    case WM_LBUTTONDOWN:
        return MOUSEEVENTF_LEFTDOWN;
    case WM_LBUTTONUP:
        return MOUSEEVENTF_LEFTDOWN;
    case WM_MBUTTONDOWN:
        return MOUSEEVENTF_MIDDLEDOWN;
    case WM_MBUTTONUP:
        return MOUSEEVENTF_MIDDLEDOWN;
    case WM_RBUTTONDOWN:
        return MOUSEEVENTF_RIGHTDOWN;
    case WM_RBUTTONUP:
        return MOUSEEVENTF_RIGHTDOWN;
    }
    return 0;
}

unsigned long Widget::getMouseInputUp(unsigned long but)
{
    switch(but)
    {
    case WM_LBUTTONDOWN:
        return MOUSEEVENTF_LEFTUP;
    case WM_LBUTTONUP:
        return MOUSEEVENTF_LEFTUP;
    case WM_MBUTTONDOWN:
        return MOUSEEVENTF_MIDDLEUP;
    case WM_MBUTTONUP:
        return MOUSEEVENTF_MIDDLEUP;
    case WM_RBUTTONDOWN:
        return MOUSEEVENTF_RIGHTUP;
    case WM_RBUTTONUP:
        return MOUSEEVENTF_RIGHTUP;
    }
    return 0;
}

void Widget::on_editButton_clicked()
{
    auto data = actionMap[ui->listWidget->currentItem()];
    if(data.type == ActionType::PasteBuffer || data.type == ActionType::Delay)
    {
        EditPasteWidget* wnd = new EditPasteWidget(this);
        connect(wnd, &EditPasteWidget::editTextEvent, this, &Widget::getEditText);
        wnd->setItem(ui->listWidget->currentItem());
        wnd->setDefaultText(actionMap[ui->listWidget->currentItem()].text);
        wnd->setActionType(data.type);
        if(data.type == ActionType::Delay)
            wnd->setIntCheck(true);
        wnd->show();
    }else if(data.type == ActionType::OpenApp)
    {
        ChooseAppWidget* wnd = new ChooseAppWidget(this);
        connect(wnd, &ChooseAppWidget::appInfoEvent, this, &Widget::getProcessInfo);
        wnd->setItem(ui->listWidget->currentItem());
        wnd->show();
    }else if(data.type == ActionType::LoopedBuffer)
    {
        LoopedBufferWidget* wnd = new LoopedBufferWidget(this);
        connect(wnd, &LoopedBufferWidget::saveListEvent, this, &Widget::getLoopedBufferList);
        wnd->setItem(ui->listWidget->currentItem(), data.list);
        wnd->show();
    }
}


void Widget::on_pasteBufferButton_clicked()
{
    EditPasteWidget* wnd = new EditPasteWidget(this);
    connect(wnd, &EditPasteWidget::editTextEvent, this, &Widget::getEditText);
    wnd->setActionType(ActionType::PasteBuffer);
    wnd->show();
}

void Widget::updateLoopCount(int x)
{
    this->loopCount = x;
    ui->currentLoopsLabel->setText(QString("Loops Count: %1").arg(x));
}

void Widget::getLoopedBufferList(QListWidgetItem* item, QStringList list)
{
    if(item == nullptr)
    {
        ActionInfo info;
        QListWidgetItem* item = new QListWidgetItem(QString("Looped Buffer: %1").arg(list.count()));

        info.type = ActionType::LoopedBuffer;
        info.list = list;

        actionMap.insert(item, info);

        ui->listWidget->addItem(item);
    }else
    {
        removeFromLoopBuffers(actionMap[item].list.count());
        actionMap[item].list = list;
        item->setText(QString("Looped Buffer: %1").arg(list.count()));
    }
    loopBuffers.emplace_back(list.count());
//    qDebug() << "Looped buffers count: " << loopBuffers.size();
//    qDebug() << "Prepare to update loop count: " << loopCount << " " << list.count();
//    updateLoopCount(findMaxLoopBuffer());
    if(loopCount < list.count())
        updateLoopCount(list.count());
}

void Widget::getEditText(QListWidgetItem* item, ActionType type, QString str)
{
    if(item == nullptr)
    {
        ActionInfo info;
        switch(type)
        {
        case ActionType::PasteBuffer:
            item = new QListWidgetItem("Paste buffer:\n" + str);
            break;
        case ActionType::Delay:
            item = new QListWidgetItem("Delay (ms): " + str);
            break;
        default:
            break;
        }

        info.type = type;
        info.text = str;

        this->actionMap.insert(item, info);
        ui->listWidget->addItem(item);
        return;
    }
    actionMap[item].text = str;
    item->setText(type == ActionType::PasteBuffer ? ("Paste buffer:\n" + str) : ("Delay (ms): " + str));
}

void Widget::getProcessInfo(QListWidgetItem* item, HandleData data)
{
    if(item == nullptr)
    {
        ActionInfo info;
        QListWidgetItem* item = new QListWidgetItem("Open App: " + data.processName);

        info.type = ActionType::OpenApp;
        info.data = data;

        actionMap.insert(item, info);

        ui->listWidget->addItem(item);
        return;
    }

    actionMap[item].data = data;
    item->setText("Open App: " + data.processName);
}

void Widget::on_openAppButton_clicked()
{
    ChooseAppWidget* wnd = new ChooseAppWidget(this);
    connect(wnd, &ChooseAppWidget::appInfoEvent, this, &Widget::getProcessInfo);
    wnd->show();
}


void Widget::on_upItemButton_clicked()
{
    int currentRow = ui->listWidget->currentRow();
    QListWidgetItem* currentItem = ui->listWidget->takeItem(currentRow);
    ui->listWidget->insertItem(currentRow - 1, currentItem);
    ui->listWidget->setCurrentItem(currentItem);
}


void Widget::on_downItemButton_clicked()
{
    int currentRow = ui->listWidget->currentRow();
    QListWidgetItem* currentItem = ui->listWidget->takeItem(currentRow);
    ui->listWidget->insertItem(currentRow + 1, currentItem);
    ui->listWidget->setCurrentItem(currentItem);
}


void Widget::on_loopedBufferButton_clicked()
{
    LoopedBufferWidget* wnd = new LoopedBufferWidget(this);
    connect(wnd, &LoopedBufferWidget::saveListEvent, this, &Widget::getLoopedBufferList);
    wnd->show();
}

void Widget::on_delayButton_clicked()
{
    EditPasteWidget* wnd = new EditPasteWidget(this);
    connect(wnd, &EditPasteWidget::editTextEvent, this, &Widget::getEditText);
    wnd->setActionType(ActionType::Delay);
    wnd->setIntCheck(true);
    wnd->show();
}

void Widget::removeFromLoopBuffers(int x)
{
    for(int i = 0; i < (int)loopBuffers.size(); i++)
    {
        if(loopBuffers.at(i) == x)
        {
            loopBuffers.erase(loopBuffers.begin()+i);
            break;
        }
    }
}
int Widget::findMaxLoopBuffer()
{
    int max = 0;
    for(auto num : loopBuffers)
    {
        if(num > max)
            max = num;
    }
    return max;
}

