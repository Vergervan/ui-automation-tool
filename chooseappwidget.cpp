#include "chooseappwidget.h"
#include "ui_chooseappwidget.h"

ChooseAppWidget::ChooseAppWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChooseAppWidget)
{
    //setWindowFlags(Qt::Window | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint);
    setWindowFlags(Qt::Window);
    setWindowModality(Qt::WindowModal);
    ui->setupUi(this);
    fillList();
}

ChooseAppWidget::~ChooseAppWidget()
{
    delete ui;
}

void ChooseAppWidget::fillList()
{
    HANDLE hProcessSnap;
    PROCESSENTRY32 pe32;

    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if(hProcessSnap == INVALID_HANDLE_VALUE)
    {
        return;
    }

    pe32.dwSize = sizeof(PROCESSENTRY32);

    if(!Process32First(hProcessSnap, &pe32))
    {
        qDebug() << "Error: Process32First";
        CloseHandle(hProcessSnap);
        return;
    }

    do
    {
        std::wstring wstr = pe32.szExeFile;
        std::string str(wstr.begin(), wstr.end());
        HandleData data = findMainWindow(pe32.th32ProcessID);
        data.processName = QString(str.c_str());
        if(IsWindow(data.windowHandle))
        {
            QListWidgetItem* item = new QListWidgetItem(QString(str.c_str()));
            handleList.insert(item, data);
            ui->listWidget->addItem(item);
            //qDebug() << "Process: " << data.processName;
        }
    }
    while(Process32Next( hProcessSnap, &pe32 ));
}

HandleData ChooseAppWidget::findMainWindow(unsigned long process_id)
{
    HandleData data;
    data.processId = process_id;
    data.windowHandle = 0;
    EnumWindows(enumWindowsCallback, (LPARAM)&data);
    return data;
}

BOOL CALLBACK ChooseAppWidget::enumWindowsCallback(HWND handle, LPARAM lParam)
{
    HandleData& data = *(HandleData*)lParam;
    unsigned long process_id = 0;
    GetWindowThreadProcessId(handle, &process_id);
    if (data.processId != process_id || !isMainWindow(handle))
        return TRUE;
    data.windowHandle = handle;
    return FALSE;
}

BOOL ChooseAppWidget::isMainWindow(HWND handle)
{
    return GetWindow(handle, GW_OWNER) == (HWND)0 && IsWindowVisible(handle);
}

void ChooseAppWidget::on_chooseButton_clicked()
{
    emit appInfoEvent(this->item, handleList[ui->listWidget->currentItem()]);
    this->close();
}

